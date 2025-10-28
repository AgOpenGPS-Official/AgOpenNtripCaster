/* ntrip.c
 * - Ntrip protocol related functions
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Informatik Centrum Dortmund http://www.icd.de
 *
 *
 * Based on the GNU General Public License published Icecast 1.3.12
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#include "definitions.h"
#include <stdio.h>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
# ifndef __USE_BSD
#  define __USE_BSD
# endif
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#if defined (_WIN32)
#include <windows.h>
#define strncasecmp strnicmp
#else
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "connection.h"
#include "log.h"
#include "sock.h"
#include "source.h"
#include "rtsp.h"
#include "client.h"
#include "avl_functions.h"
#include "vars.h"
#include "memory.h"
#include "admin.h"

extern server_info_t info;
avl_tree *header_elements;
avl_tree *ntrip1_0_messages;
avl_tree *ntrip2_0_messages;

ntrip_method_t ntrip_methods[] = { // rtsp. ajd
  {"GET", http_e, http_client_login, NULL},
  {"POST", http_e, http_source_login, NULL},
  {"SOURCE", unknown_protocol_e, http_source_login, NULL},
  {"ADMIN", unknown_protocol_e, admin_login, NULL},

  {"SETUP", rtsp_e, rtsp_client_login, rtsp_setup},
  {"PLAY", rtsp_e, rtsp_client_login, rtsp_play},
  {"RECORD", rtsp_e, rtsp_client_login, rtsp_post},
  {"TEARDOWN", rtsp_e, rtsp_client_login, rtsp_teardown},
  {"PAUSE", rtsp_e, rtsp_client_login, rtsp_pause},
  {"OPTIONS", rtsp_e, rtsp_client_login, rtsp_options},
  {"DESCRIBE", rtsp_e, rtsp_client_login, rtsp_describe},
  {"GET_PARAMETER", rtsp_e, rtsp_client_login, rtsp_get_parameter},
  { (char *) NULL, (protocol_t) NULL, (ntripcaster_function *) NULL, (ntripcaster_int_function *) NULL}
};

ntrip_message_t ntrip1_0_message[] = {
  { HTTP_GET_STREAM_OK, unknown_protocol_e, "ICY 200 OK", 200,  {-1} },
  { HTTP_GET_SOURCETABLE_OK, unknown_protocol_e, "SOURCETABLE 200 OK", 200, {104,8,105,3,4,-1} },
  { HTTP_GET_STREAM_WRONG_MOUNT, http_e, "Not Found", 404,  {104,8,-1} },
  { HTTP_GET_NOT_AUTHORIZED, http_e, "Unauthorized", 401,   {8,7,3,200,-1} },

  { HTTP_SOURCE_OK, unknown_protocol_e, "ICY 200 OK", 200,  {-1} },
//  { HTTP_SOURCE_OK, unknown_protocol_e, "OK", 200,    {-1} },
  { HTTP_SOURCE_MOUNT_CONFLICT, unknown_protocol_e, "ERROR - Mount Point Taken or Invalid", 409, {-1} },
  { HTTP_SOURCE_NOT_AUTHORIZED, unknown_protocol_e, "ERROR - Bad Password", 401, {-1} },

  { HTTP_BAD_REQUEST, http_e, "Bad Request", 400,     {104,8,-1} },
  { HTTP_FORBIDDEN, http_e, "Forbidden", 403,       {104,8,-1} },
  { HTTP_NOT_ACCEPTABLE, http_e, "Not Acceptable", 406,     {104,8,-1} },
  { HTTP_NOT_IMPLEMENTED, http_e, "Not Implemented", 501,   {104,8,-1} },
  { HTTP_SERVICE_UNAVAILABLE, http_e, "Service Unavailable", 503, {104,8,-1} },

  { -1, -1, (char *)NULL, -1, {} }
};

ntrip_message_t ntrip2_0_message[] = {
  { HTTP_GET_STREAM_OK, http_e, "OK", 200,      {102,103,8,100,101,105,3,5,-1} },
  { HTTP_GET_SOURCETABLE_OK, http_e, "OK", 200,       {102,106,103,8,105,3,4,-1} },
  { HTTP_GET_STREAM_WRONG_MOUNT, http_e, "Not Found", 404,  {102,103,8,105,-1} },
  { HTTP_GET_NOT_AUTHORIZED, http_e, "Unauthorized", 401,   {102,103,8,7,105,-1} },

  { HTTP_SOURCE_OK, http_e, "OK", 200,        {102,103,8,100,101,105,5,-1} },
  { HTTP_SOURCE_MOUNT_CONFLICT, http_e, "Conflict", 409,    {102,103,8,105,-1} },
  { HTTP_SOURCE_NOT_AUTHORIZED, http_e, "Unauthorized", 401,  {102,103,8,7,105,-1} },

// Hack: Bad request: Caster will React like Not implemented
//  { HTTP_BAD_REQUEST, http_e, "Bad Request", 400,     {102,103,8,107,105,-1} },
  { HTTP_BAD_REQUEST, http_e, "Not Implemented", 501,     {102,103,8,105,-1} },
  { HTTP_FORBIDDEN, http_e, "Forbidden", 403,       {102,103,8,105,-1} },
  { HTTP_NOT_ACCEPTABLE, http_e, "Not Acceptable", 406,     {102,103,8,105,-1} },
  { HTTP_NOT_IMPLEMENTED, http_e, "Not Implemented", 501,   {102,103,8,105,-1} },
  { HTTP_SERVICE_UNAVAILABLE, http_e, "Service Unavailable", 503, {102,103,8,105,-1} },

  { RTSP_OPTIONS_OK, rtsp_e, "OK", 200,         {0,6,-1} },
  { RTSP_DESCRIBE_OK, rtsp_e, "OK", 200,        {0,3,4,-1} },
  { RTSP_SETUP_OK, rtsp_e, "OK", 200,         {0,1,2,102,103,8,-1} },
  { RTSP_SETUP_WRONG_MOUNT, rtsp_e, "Not Found", 404,     {0,102,103,8,-1} },
  { RTSP_SETUP_MOUNT_CONFLICT, rtsp_e, "Conflict", 409,     {0,102,103,8,-1} },
//  { RTSP_SETUP_NOT_AUTHORIZED, rtsp_e, "Unauthorized", 401,   {0,102,103,8,7,-1} },
  { RTSP_PLAY_OK, rtsp_e, "OK", 200,        {0,1,-1} },
  { RTSP_PLAY_WRONG_MOUNT, rtsp_e, "Not Found", 404,    {0,1,102,103,8,-1} },
  { RTSP_POST_OK, rtsp_e, "OK", 200,        {0,1,-1} },
  { RTSP_POST_MOUNT_CONFLICT, rtsp_e, "Conflict", 409,    {0,1,102,103,8,-1} },
  { RTSP_PAUSE_OK, rtsp_e, "OK", 200,         {0,1,-1} },
  { RTSP_TEARDOWN_OK, rtsp_e, "OK", 200,        {0,1,-1} },
  { RTSP_GET_PARAMETER_OK, rtsp_e, "OK", 200,         {0,1,-1} },
  { RTSP_BAD_REQUEST, rtsp_e, "Bad Request", 400,         {0,102,103,8,-1} },
  { RTSP_NOT_AUTHORIZED, rtsp_e, "Unauthorized", 401,         {0,102,103,8,7,-1} },
  { RTSP_SESSION_NOT_FOUND, rtsp_e, "Session Not Found", 454,       {0,1,102,103,8,-1} },
  { RTSP_METHOD_NOT_VALID, rtsp_e, "Method Not Valid In This State", 455,   {0,1,102,103,8,-1} },
  { RTSP_AGGREGATE_NOT_ALLOWED, rtsp_e, "Aggregate Operation Not Allowed", 459,   {0,1,102,103,8,-1} },
  { RTSP_UNSUPPORTED_TRANSPORT, rtsp_e, "Unsupported Transport", 461,     {0,102,103,8,-1} },
  { RTSP_INTERNAL_SERVER_ERROR, rtsp_e, "Internal Server Error", 500,     {0,102,103,8,-1} },
  { RTSP_NOT_IMPLEMENTED, rtsp_e, "Not Implemented", 501,       {0,102,103,8,-1} },
  { RTSP_SERVICE_UNAVAILABLE, rtsp_e, "Service Unavailable", 503,     {0,102,103,8,-1} },

  { UDP_GET_STREAM_OK, http_e, "OK", 200,       {102,103,8,100,101,105,3,5,1,-1} },
  { UDP_SOURCE_OK, http_e, "OK", 200,         {102,103,8,100,101,105,5,1,-1} },

  { -1, -1, (char *)NULL, -1, {} }
};

ntrip_header_element_t ntrip_header_element[] = {
// change while runtime
  { 0, "CSeq", "%d" },
  { 1, "Session", "%d" },
  { 2, "Transport", "RTP/GNSS;unicast;client_port=%d;server_port=%d" },
  { 3, "Content-Type", "%s" },
  { 4, "Content-Length", "%d" },
  { 5, "Transfer-Encoding", "%s" },
  { 6, "Allow", "%s" },
  { 7, "WWW-Authenticate", "Basic realm=\"%s\"" },
  { 8, "Date", "%s" },
// do not change while runtime
  { 100, "Cache-Control", "no-store,no-cache,max-age=0" },
  { 101, "Pragma", "no-cache" },
  { 102, "Ntrip-Version", NULL }, // must be initialized
  { 103, "Server", NULL }, // must be initialized
  { 104, "Server", NULL }, // for NTRIP1.0 compatibility. must be initialized
  { 105, "Connection", "close"},
  { 106, "Ntrip-Flags", "st_filter,st_auth,st_match,st_strict,rtsp,plain_rtp"},
  { 107, "Content-Type", "text/html" }, // Hack for Error Handling
  { 200, "Connection", "close\r\n\r\n<!DOCTYPE html>\r\n<html><head><title>401 Unauthorized</title></head>" DEFAULT_BODY_TAG "\r\n<h1 style=\"text-align:center;\">The server does not recognize your privileges to the requested entity/stream</h1>\r\n</body></html>" },

  { -1, (char *)NULL, (char *)NULL }
};

void ntrip_init() {
  int c=0;
  char buf[50];
  ntrip_header_element_t *he;

  header_elements = avl_create (compare_header_elements, &info);
  ntrip1_0_messages = avl_create (compare_messages, &info);
  ntrip2_0_messages = avl_create (compare_messages, &info);

  while (ntrip1_0_message[c].type  != -1) {
    avl_replace (ntrip1_0_messages, &ntrip1_0_message[c]);
    c++;
  }

  c=0;
  while (ntrip2_0_message[c].type  != -1) {
    avl_replace (ntrip2_0_messages, &ntrip2_0_message[c]);
    c++;
  }

  c=0;
  while (ntrip_header_element[c].index != -1) {
    avl_replace (header_elements, &ntrip_header_element[c]);
    c++;
  }

  snprintf(buf, 50, "Ntrip/%s", info.ntripversion);
  he = get_header_element(102);
  he->value = strdup(buf);
  snprintf(buf, 50, "NTRIP BKG Caster/%s", info.version);
  he = get_header_element(103);
  he->value = strdup(buf);
  snprintf(buf, 50, "NTRIP BKG Caster %s/%s", info.version, info.ntripversion);
  he = get_header_element(104);
  he->value = strdup(buf);
}

ntrip_method_t *get_ntrip_method(char *name, int protocol) {
  int mIndex = 0;

  xa_debug(3, "get_ntrip_method: name %s", name);

  while (ntrip_methods[mIndex].method != NULL) {
    if ((strncmp(name, ntrip_methods[mIndex].method, strlen(ntrip_methods[mIndex].method)) == 0) && (protocol == ntrip_methods[mIndex].protocol)) {

      xa_debug(3, "get_ntrip_method: found method [%s]", ntrip_methods[mIndex].method);
      return &ntrip_methods[mIndex];
    }
    mIndex++;
  }


  return NULL;
}

char *get_ntrip_method_string(char *buf) {
  int mIndex = 0;
  int len;

  buf[0] = '\0';

  while (ntrip_methods[mIndex].method != NULL) {
    if (ntrip_methods[mIndex].protocol == rtsp_e) {
      catsnprintf(buf, BUFSIZE, "%s", ntrip_methods[mIndex].method);
    }
    mIndex++;
  }

  len = strlen(buf);

  if (len > 0) buf[len-1] = '\0';

  return buf;
}

ntrip_header_element_t *get_header_element(int index) {
  ntrip_header_element_t *he;
  ntrip_header_element_t search;

  search.index = index;

  he = (ntrip_header_element_t *)avl_find(header_elements, &search);

  return he;
}

ntrip_message_t *get_ntrip_message(int type, avl_tree *tree) {
  ntrip_message_t *me;
  ntrip_message_t search;

  search.type = type;

  me = (ntrip_message_t *)avl_find(tree, &search);

  return me;
}

void add_header_string(int header_element[], char *buf) {
  int i=0;
  char linebuf[BUFSIZE];
  ntrip_header_element_t *he;

  while (header_element[i] > -1) {
    he = get_header_element(header_element[i]);
    if (he != NULL) {
      snprintf(linebuf, BUFSIZE, "%s: %s\r\n", he->name, he->value);
      strcat(buf, linebuf);
    }
    i++;
  }
}

void ntrip_write_message(connection_t *con, int type, ...) {
  char fmt[BUFSIZE];
  char sendbuf[BUFSIZE];
  ntrip_message_t *msg;
  va_list ap;

  if (con->com_protocol == ntrip1_0_e)
    msg = get_ntrip_message(type, ntrip1_0_messages);
  else
    msg = get_ntrip_message(type, ntrip2_0_messages);


  if (msg == NULL) return;

  if (msg->protocol == http_e) {
    snprintf(fmt, BUFSIZE, "HTTP/1.1 %d %s\r\n", msg->code, msg->message);
  } else if (msg->protocol == rtsp_e) {
    snprintf(fmt, BUFSIZE, "RTSP/1.0 %d %s\r\n", msg->code, msg->message);
  } else {
    snprintf(fmt, BUFSIZE, "%s\r\n", msg->message);
  }

  add_header_string(msg->header_element, fmt);

  va_start (ap, type);
  vsnprintf(sendbuf, BUFSIZE, fmt, ap);
  va_end(ap);

  sock_write_line_con(con, sendbuf);

  xa_debug(1, "ntrip_write_message: %s: connection %d from [%s] written: [%s]", msg->message, con->id, con_host(con), sendbuf);
}

int ntrip_read_header(connection_t *con, char *header, ntrip_request_t *req) {
  char line[BUFSIZE];
  const char *var;
  int go_on = 1;

  xa_debug(2, "Connection %ld, read NTRIP header: [%s]; http_chunk is %s", con->id, header, (con->http_chunk==NULL)?"null":"NOT null");

  if (!con || !header) {
    write_log(LOG_DEFAULT, "WARNING: ntrip_read_header() called with NULL pointer");
    return 0;
  }

  if (splitc(line, header, '\n') == NULL) {
    xa_debug(1, "Invalid NTRIP header");
    return 0;
  }

  if (con->headervars == NULL) con->headervars = create_header_vars ();

  zero_request(req);
  var = get_con_variable(con, "CSeq");
  if (var != NULL) req->cseq = atoi(var);
  var = get_con_variable(con, "Session");
  if (var != NULL) req->sessid = atol(var);
  build_request(con, line, req);



  if (req->method == NULL) {
    req->cseq++; // Hack, command was not found, maybee we did oversee some.
    xa_debug(1, "Invalid method");
    return 0;
  }

  decode_url_string(req->path);
  do {
    if (splitc(line, header, '\n') == NULL) {
      strncpy(line, header, BUFSIZE);
      line[BUFSIZE-1] = 0;
      go_on = 0;
    }
    extract_header_vars (line, con->headervars);
  } while (go_on);

  var = get_con_variable(con, "Ntrip-Version");

        if ((var == NULL) && (req->method==NULL)) {
    write_log(LOG_DEFAULT, "WARNING: ntrip_read_header() called with empty req_method - no content in request?");
    return 0;
  }

  if (((var == NULL) && (req->method->protocol != rtsp_e)) || ((var != NULL) && (strstr(var, "1.0") != NULL))) {
    con->com_protocol = ntrip1_0_e;
    con->trans_encoding = not_chunked_e;
  } else if(con->sock <= 0) { /* UDP mode */
    con->com_protocol = ntrip2_0_e;
    con->trans_encoding = not_chunked_e;
  } else {
    con->com_protocol = ntrip2_0_e;
    con->trans_encoding = chunked_e;

    if (con->http_chunk == NULL)
      con->http_chunk = ntrip_create_http_chunk();
    else
      ntrip_zero_http_chunk(con->http_chunk);
  }
  var = get_con_variable(con, "CSeq");
  if (var != NULL) req->cseq = atoi(var);
  var = get_con_variable(con, "Session");
  if (var != NULL) req->sessid = atol(var);

  xa_debug(2, "read header: Ntripversion %s Cseq %d Session %d Transferencoding %s", (con->com_protocol==ntrip1_0_e)?"1.0":"2.0",req->cseq,req->sessid,(con->trans_encoding==not_chunked_e)?"not chunked":"chunked");

  return 1;
}

http_chunk_t *ntrip_create_http_chunk() {
  http_chunk_t *hc;

  hc = (http_chunk_t *)nmalloc(sizeof(http_chunk_t));
  ntrip_zero_http_chunk(hc);

  return hc;
}

void ntrip_zero_http_chunk(http_chunk_t *hc) {
  hc->buf[0] = '\0';
  hc->left = -1;
  hc->off = 0;
  hc->finish = 0;
}
