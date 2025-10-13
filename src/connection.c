/* connection.c
 * - Connection functions
 *
 * Copyright (c) 2003
 * German Federal Agency for Cartography and Geodesy (BKG)
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Informatik Centrum Dortmund http://www.icd.de
 *
 * The BKG disclaims any liability nor responsibility to any person or entity
 * with respect to any loss or damage caused, or alleged to be caused,
 * directly or indirectly by the use and application of the NTRIP technology.
 *
 * For latest information and updates, access:
 * http://igs.ifag.de/index_ntrip.htm
 *
 * Georg Weber
 * BKG, Frankfurt, Germany, June 2003-06-13
 * E-mail: euref-ip@bkg.bund.de
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
#include "definitions.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/time.h>
#else
#include <winsock.h>
#endif

#include "avl.h"
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "connection.h"
#include "log.h"
#include "ntripcaster_resolv.h"
#include "sock.h"
#include "rtsp.h"
#include "client.h"
#include "source.h"
#include "admin.h"
#include "logtime.h"
#include "restrict.h"
#include "memory.h"
#include "http.h"
#include "vars.h"
#include "commands.h"

extern server_info_t info;
const char cnull[] = "(null)";

/*
 * This is called to handle a brand new connection, in it's own thread.
 * Nothing is know about the type of the connection.
 * Assert Class: 3
 */
void *handle_connection(void *arg) {
  connection_t *con = (connection_t *)arg;
  char line[BUFSIZE];
  ntrip_request_t req;
  int res;
  char time[50];

  thread_init();

  if (!con) {
    write_log(LOG_DEFAULT, "handle_connection: got NULL connection");
    thread_exit(0);
    return NULL;
  }

  if (info.reverse_lookups) con->hostname = reverse(con->host);

  if (!allowed_no_policy (con, unknown_connection_e)) {
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Access denied (internal acl list, generic connection)");
    thread_exit(0);
    return NULL;
  }

  if(con->sock > 0)
  {
    sock_set_blocking(con->sock, SOCK_BLOCKNOT);

    /* Fill line[] with the user header, ends with \n\n */
    if ((res = sock_read_lines_with_timeout(con->sock, line, BUFSIZE)) <= BUFSIZE) {
      write_log(LOG_DEFAULT, "handle_connnection(): Socket error on connection %d", con->id);
      kick_not_connected(con, "Socket error");
      thread_exit(0);
      return NULL;
    }
  }
  else
  {
    int i, pos = 0;
    for(i = 0; i < con->udpbuffers->len; ++i)
    {
      if(con->udpbuffers->buffer[i] != '\r')
        line[pos++] = con->udpbuffers->buffer[i];
      line[pos] = '\0';
    }
    line[con->udpbuffers->len] = 0;
  }
/*
  if (strncmp(line, "SOURCE ", 7) == 0) {
    if (ntrip_read_old_source_header(con, line, &req) != 1) {
      ntrip_write_message(con, HTTP_BAD_REQUEST, get_formatted_time(HEADER_TIME, time));
      kick_not_connected(con, "Invalid header");
      thread_exit(0);
      return NULL;
    }
  } else {*/
  if (ntrip_read_header(con, line, &req) != 1) {
    ntrip_write_message(con, HTTP_BAD_REQUEST, get_formatted_time(HEADER_TIME, time));
    kick_not_connected(con, "Invalid header");
    thread_exit(0);
    return NULL;
  }
//  }

  if (req.method != NULL) {
    ((*(req.method->login_func))(con, &req));
    thread_exit(0);
    return NULL;
  }

  ntrip_write_message(con, HTTP_NOT_IMPLEMENTED, get_formatted_time(HEADER_TIME, time));
  kick_not_connected(con, "Method not implemented");

  thread_exit(0);
  return NULL;
}

connection_t *
create_connection()
{
  connection_t *con = (connection_t *) nmalloc (sizeof (connection_t));
  con->type = unknown_connection_e;
  //con->headervars = NULL;
  con->sin = NULL;
  con->udpbuffers = NULL;
  con->hostname = NULL;
  con->headervars = NULL;
  con->food.source = NULL;
  con->food.client = NULL; // rtsp. ajd
  con->group = NULL; // added. IMPORTANT!!!. ajd
  con->res = NULL;
  con->ghost = 0; // added. ajd
  con->sock = -1;
  con->sinlen = 0;

  /* rtsp. ajd */
  con->com_protocol = ntrip2_0_e;
  con->data_protocol = tcp_e;
  con->trans_encoding = chunked_e;
  con->session_id = -1;
  con->rtp = NULL;
  con->http_chunk = NULL;

#ifdef HAVE_TLS
  con->tls_socket = 0;
#endif /* HAVE_TLS */
  return con;
}

connection_t *
get_connection (sock_t *sock)
{
  int sockfd;
  socklen_t sin_len;
  connection_t *con;
  fd_set rfds;
  struct timeval tv;
  int i, maxport = 0;
  struct sockaddr_in *sin = (struct sockaddr_in *)nmalloc(sizeof(struct sockaddr_in));

  /* PARANOIA here */
  if (!sin)
  {
    write_log (LOG_DEFAULT, "WARNING: Weird stuff in get_connection. nmalloc returned NULL sin");
    return NULL;
  }

  /* setup sockaddr structure */
  sin_len = sizeof(struct sockaddr_in);
  memset(sin, 0, sin_len);

  /* try to accept a connection */
  FD_ZERO(&rfds);

  for (i = 0; i < MAXLISTEN; i++) {
    if (sock_valid (sock[i])) {
      FD_SET(sock[i], &rfds);
      if (sock[i] > maxport)
        maxport = sock[i];
    }
  }
  maxport += 1;

  tv.tv_sec = 0;
  tv.tv_usec = 30000;

  if (select(maxport, &rfds, NULL, NULL, &tv) > 0) {
    for (i = 0; i < MAXLISTEN; i++) {
      if (sock_valid (sock[i]) && FD_ISSET(sock[i], &rfds))
        break;
    }
  } else {
    nfree(sin);
    return NULL;
  }

  sockfd = sock_accept(sock[i], (struct sockaddr *)sin, &sin_len);

  if (sockfd >= 0) {
    con = create_connection();
    if (!sin)
    {
      xa_debug (1, "ERROR: NULL sockaddr struct, wft???");
      return NULL;
    }

    con->host = create_malloced_ascii_host(&(sin->sin_addr));
    con->sock = sockfd;
    con->sin = sin;
    con->sinlen = sin_len;
    xa_debug (2, "DEBUG: Getting new connection on socket %d from host %s", sockfd, con->host ? con->host : "(null)");
    con->hostname = NULL;
    con->headervars = NULL;
    con->id = new_id ();
    con->connect_time = get_time ();
#ifdef HAVE_LIBWRAP
    if (!sock_check_libwrap(sockfd, unknown_connection_e))
    {
      kick_not_connected (con, "Access denied (tcp wrappers) [generic connection]");
      return NULL;
    }
#endif

    return con; /* We got a one */
  }

  /* FIXME (don't use strerror) */
  if (!is_recoverable (errno))
    xa_debug (1, "WARNING: accept() failed with on socket %d, max: %d, [%d:%s]", sock[i], maxport,
        errno, strerror(errno));
  nfree (sin);
  return NULL;
}

void
describe_connection (const com_request_t *req, const connection_t *describecon)
{
  char buf[BUFSIZE];
  avl_traverser trav = {0};
  const varpair_t *vp;

  if (!req || !describecon)
  {
    xa_debug (1, "WARNING: describe_connection() called with NULL pointers");
    return;
  }

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_START, "Connection info");

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_MISC, "Connection id: %lu", describecon->id);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_MISC, "Connection socket: %d", describecon->sock);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_MISC, "Connect time: %s", nntripcaster_time (get_time () - describecon->connect_time, buf));
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_MISC, "Connection host and ip: %s [%s]", describecon->hostname ? describecon->hostname : "(null)", describecon->host);

  if (describecon->headervars)
  {
    admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_HEADERS_START, "Header variables:");
    while ((vp = avl_traverse (describecon->headervars, &trav)))
      admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_HEADERS_ENTRY, "'%s' = '%s'", vp->name, vp->value);
    admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_HEADERS_END, "End of header variable listing");
  }

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CON_END, "End of connection info");
}

const char *get_user_agent (connection_t *con) {
  const char *res;

  if (!con) return cnull;

  res = get_con_variable (con, "User-Agent");

  if (!res) {
    res = get_con_variable (con, "User-agent");
    if (!res) res = get_con_variable (con, "user-agent");
  }

        if (!res)
    return cnull;
  else
    return res;
}

const char *get_source_agent (connection_t *con) {
  const char *res;

  if (!con) return cnull;

  if (con->com_protocol == ntrip2_0_e) {
    res = get_user_agent (con);
  } else {
    res = get_con_variable (con, "Source-Agent");
    if (!res) {
      res = get_con_variable (con, "Source-agent");
      if (!res) {
        res = get_con_variable (con, "source-agent");
      }
    }
  }

        if (!res)
    return cnull;
  else
    return res;
}

void
build_source_con_line_with_opts (connection_t *con, char *line, int *opt, int maxlen)
{
  char buf[BUFSIZE];

  line[0] = '\0';
  buf[0] = '\0';

  /* Build the line */
  if (opt[SOURCE_SHOW_ID])
    catsnprintf (line, BUFSIZE, "[Id: %d] ", con->id);
  if (opt[SOURCE_SHOW_SOCKET])
    catsnprintf (line, BUFSIZE, "[Sock: %d] ", con->sock);
  if (opt[SOURCE_SHOW_CTIME])
  {
    char ct[100];
    get_string_time (ct, con->connect_time, REGULAR_DATETIME);
    catsnprintf (line, BUFSIZE, "[Time of connect: %s] ", ct);
  }
  if (opt[SOURCE_SHOW_IP] && con->host)
    catsnprintf (line, BUFSIZE, "[IP: %s] ", con->host);
  if (opt[SOURCE_SHOW_HOST] && con->hostname)
    catsnprintf (line, BUFSIZE, "[Host: %s] ", con->hostname);
  if (opt[SOURCE_SHOW_AGENT])
    catsnprintf (line, BUFSIZE, "[Source Agent: %s] ", get_source_agent(con));
  if (opt[SOURCE_SHOW_STATE])
    catsnprintf (line, BUFSIZE, "[State: %d] ", con->food.source->connected);
  if (opt[SOURCE_SHOW_TYPE])
    catsnprintf (line, BUFSIZE, "[Type: %s] ", source_type(con));
  if (opt[SOURCE_SHOW_CLIENTS])
    catsnprintf (line, BUFSIZE, "[Clients: %d] ", con->food.source->num_clients);
  if (opt[SOURCE_SHOW_PRIO])
    catsnprintf (line, BUFSIZE, "[Priority: %d] ", con->food.source->priority);
  if (opt[SOURCE_SHOW_MOUNT])
    catsnprintf (line, BUFSIZE, "[Mountpoint: %s] ", nullcheck_string (con->food.source->audiocast.mount));
  if (opt[SOURCE_SHOW_READ])
    catsnprintf (line, BUFSIZE, "[KBytes read: %lu] ", con->food.source->stats.read_kilos);
  if (opt[SOURCE_SHOW_WRITTEN])
    catsnprintf (line, BUFSIZE, "[KBytes written: %lu] ", con->food.source->stats.write_kilos);
  if (opt[SOURCE_SHOW_CONNECTS])
    catsnprintf (line, BUFSIZE, "[Client connections: %lu] ", con->food.source->stats.client_connections);
  if (opt[SOURCE_SHOW_TIME])
    catsnprintf (line, BUFSIZE, "[Connected for: %s] ", nntripcaster_time (get_time() - con->connect_time, buf));
}

connection_t *get_nontrip_connection() { // nontrip.
  int sockfd;
  socklen_t sin_len;
  connection_t *con;
  struct timeval tv;
  struct sockaddr_in *sin;
  avl_traverser trav = {0};
  nontripsource_t *nsource;
  fd_set rfds;
  int maxfd = 0;
  int res;

  FD_ZERO(&rfds);

  while ((nsource = avl_traverse (info.nontripsources, &trav))) {
    if (sock_valid (nsource->listen_sock)) {
      FD_SET(nsource->listen_sock, &rfds);
      if (nsource->listen_sock > maxfd) maxfd = nsource->listen_sock;
    }
  }
  maxfd += 1;

  tv.tv_sec = 0;
  tv.tv_usec = 300000;

  res = select(maxfd, &rfds, NULL, NULL, &tv);

  if (res <= 0) return NULL;

  zero_trav(&trav);
  while ((nsource = avl_traverse (info.nontripsources, &trav))) {
    if (sock_valid (nsource->listen_sock) && FD_ISSET(nsource->listen_sock, &rfds)) {
      xa_debug (3, "Getting new connection for source %s socket %d", nsource->mount, nsource->listen_sock);
      break;
    }
  }

  sin = (struct sockaddr_in *)nmalloc(sizeof(struct sockaddr_in));

  if (!sin) {
    write_log (LOG_DEFAULT, "WARNING: Weird stuff in get_nontrip_connection. nmalloc returned NULL sin");
    return NULL;
  }

  sin_len = sizeof(struct sockaddr_in);
  memset(sin, 0, sin_len);

  sockfd = sock_accept(nsource->listen_sock, (struct sockaddr *)sin, &sin_len);

  if (sockfd < 0) {
    xa_debug (1, "WARNING: accept() failed with on socket %d, maxfd: %d, [%d:%s]", nsource->listen_sock, maxfd, errno, strerror(errno));
    nfree (sin);
    return NULL;
  }

  con = create_connection();
  con->host = create_malloced_ascii_host(&(sin->sin_addr));
  con->sock = sockfd;
  con->sin = sin;
  con->sinlen = sin_len;
  xa_debug (2, "DEBUG: Getting new connection on socket %d from host %s", sockfd, con->host ? con->host : "(null)");
  con->hostname = NULL;
  con->headervars = NULL;
  con->id = new_id ();
  con->connect_time = get_time ();
  con->nontripsrc = nsource;

#ifdef HAVE_LIBWRAP
  if (!sock_check_libwrap(sockfd, unknown_connection_e)) {
    kick_not_connected (con, "Access denied (tcp wrappers) [generic connection]");
    return NULL;
  }
#endif

  return con;
}

void *handle_nontrip_connection(void *arg) { // nontrip. ajd
  connection_t *con = (connection_t *)arg;
  source_t *source;
  char slash[BUFSIZE];

  thread_init();

  if (!con) {
    write_log(LOG_DEFAULT, "handle_nontrip_connection: got NULL connection");
    thread_exit(0);
  }

  if (info.reverse_lookups) con->hostname = reverse(con->host);
  sock_set_blocking(con->sock, SOCK_BLOCK);

  put_source(con);
  con->food.source->type = nontrip_source_e;
  source = con->food.source;

  add_varpair2 (con->headervars, "Source-Agent", "NoNTRIP Source");

  if (con->nontripsrc->mount[0] == '/') {
    source->audiocast.mount = my_strdup(con->nontripsrc->mount);
  } else {
    snprintf(slash, BUFSIZE, "/%s", con->nontripsrc->mount);
    source->audiocast.mount = my_strdup(slash);
  }

  thread_mutex_lock(&info.source_mutex);

  if (mount_exists (source->audiocast.mount) || (source->audiocast.mount[0] == '\0')) {
    thread_mutex_unlock(&info.source_mutex);
    kick_connection (con, "Invalid Mount Point");
    return NULL;
  }

  if ((info.num_sources + 1) > info.max_sources) {
    thread_mutex_unlock(&info.source_mutex);
    kick_connection (con, "Server Full (too many streams)");
    return NULL;
  }

  add_source();
  avl_insert(info.sources, con);

  thread_mutex_unlock(&info.source_mutex);

  source->connected = SOURCE_CONNECTED;

  write_log (LOG_DEFAULT, "Accepted encoder (NoNTRIP) on mountpoint %s and port %d from %s. %d sources connected", source->audiocast.mount, con->nontripsrc->port, con_host(con), info.num_sources);

  thread_rename("NoNTRIP Source Thread");
  source_func(con);

  thread_exit(0);
  return NULL;
}
