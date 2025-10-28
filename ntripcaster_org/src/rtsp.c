/* rtsp.c
 * - RTSP functions
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
#include <math.h>

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
#include "sock.h"
#include "ntrip.h"
#include "rtsp.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "client.h"
#include "connection.h"
#include "log.h"
#include "source.h"
#include "rtp.h"
#include "logtime.h"
#include "memory.h"
#include "avl_functions.h"
#include "vars.h"
#include "authenticate/basic.h"
#include "pool.h"

extern server_info_t info;

void rtsp_client_login(connection_t *con, ntrip_request_t *req) {
  put_client(con);
  con->food.client->type = rtsp_client_e;

  write_log(LOG_DEFAULT, "Accepted RTSP client %d from [%s], %d clients connected", con->id, con_host (con), info.num_clients);

  thread_rename("Rtsp Client Thread");

  rtsp_client_func(con, req);
}

void *rtsp_client_func(connection_t *con, ntrip_request_t *req) {
  mythread_t *mt;
  char lines[BUFSIZE] = "";
  char time[50];
  int methodRes;
  int res;

  mt = thread_get_mythread();

  sock_set_blocking(con->sock, SOCK_BLOCKNOT);

  do {
    xa_debug (2, "DEBUG: rtsp_client_func: method [%s]", req->method->method);

    if (req->method->protocol == rtsp_e) {
      methodRes = ((*(req->method->execute_func))(con, req));
      if (methodRes != 1) xa_debug (2, "DEBUG: rtsp method failed!");
    } else {
      ntrip_write_message(con, RTSP_NOT_IMPLEMENTED, req->cseq, get_formatted_time(HEADER_TIME, time));
      xa_debug (2, "DEBUG: rtsp method not implemented");
    }

    while ((res = readable_timeo(con->sock, 1)) == 0) my_sleep(500000);

    if (res < 0) {
      xa_debug (2, "DEBUG: readable timeout result < 0");
      break;
    }

    res = sock_read_lines_with_timeout(con->sock, lines, BUFSIZE);

    if (res <= BUFSIZE) {
      xa_debug (2, "DEBUG: sock_read_lines_with_timeout = %d", res);
      break;
    }

    if (ntrip_read_header(con, lines, req) != 1) {
      ntrip_write_message(con, RTSP_BAD_REQUEST, req->cseq, get_formatted_time(HEADER_TIME, time));
      xa_debug (2, "DEBUG: bad rtsp request");
      break;
    }
  } while (thread_alive (mt) && (req->method != NULL));

  write_log(LOG_DEFAULT, "Exiting RTSP client %d from [%s], %d clients connected", con->id, con_host (con), info.num_clients);

  if (con->session_id > -1) delete_session_by_id(con->session_id);

  close_connection (con);

  return NULL;
}

rtsp_session_t *create_rtsp_session() {
  rtsp_session_t *session = (rtsp_session_t *)nmalloc(sizeof(rtsp_session_t));
  session->id = rand()+1;
  session->state = INIT_STATE;
  session->creation_time = -1;
  session->timeout_time = -1;
  session->server_port = -1;
  session->client_port = -1;
  session->transport_ttl = 127;
  session->udp_sockfd = -1;
  session->con = NULL;
  session->mount = NULL;
  session->transport_ip = NULL;
  return session;
}

/* must have session_mutex. ajd */
int add_session_no_mutex(rtsp_session_t *session) {
  rtsp_session_t *s = NULL;

  s = (rtsp_session_t *)avl_insert(info.rtsp_sessions, session);

  if (s == NULL)
    return 1;
  else
    return 0;
}

void clean_session_tree() {
  avl_traverser trav = {0};
  rtsp_session_t *s;
  long int time = get_time();
  int count = avl_count(info.rtsp_sessions);
  int max = (int)(sqrt(((double)(count + MAX_SESSIONS)/(double)MAX_SESSIONS)-1.0)*count)+1;

  thread_mutex_lock(&info.session_mutex);

  while ((s = avl_traverse (info.rtsp_sessions, &trav)) != NULL) {
    if ((s->timeout_time < time) && (s->con == NULL)) {
      delete_session_no_mutex(s);
      zero_trav(&trav);
      max--;
    }
    if (max <= 0) break;
  }

  thread_mutex_unlock(&info.session_mutex);
}

/* must have session_mutex to call this. ajd */
rtsp_session_t *find_session(long int id) {
  rtsp_session_t *s;
  rtsp_session_t search;

  search.id = id;
  s = (rtsp_session_t *)avl_find(info.rtsp_sessions, &search);

  if (s != NULL) s->timeout_time = get_time() + info.session_timeout;

  return s;
}

rtsp_session_t *get_new_session(void) {
  rtsp_session_t *session = NULL;

  thread_mutex_lock(&info.session_mutex);

  if (avl_count(info.rtsp_sessions) < MAX_SESSIONS) {
    session = create_rtsp_session();
    while (add_session_no_mutex(session) != 1)
      session->id = rand()+1;
    xa_debug(1, "get_new_session %d %d %d %p", avl_count(info.rtsp_sessions),
    MAX_SESSIONS, session->id, session);
    session->creation_time = get_time();
    session->timeout_time = session->creation_time + info.session_timeout;
  }

  thread_mutex_unlock(&info.session_mutex);

  return session;
}

int delete_session(rtsp_session_t *session) {
  rtsp_session_t *s;

  thread_mutex_lock(&info.session_mutex);
  s = (rtsp_session_t *)avl_delete(info.rtsp_sessions, session);

  if (s != NULL) {
    free_session(s);
    thread_mutex_unlock(&info.session_mutex);
    return 1;
  }

  thread_mutex_unlock(&info.session_mutex);

  return 0;
}

/* must have session_mutex to call this. ajd */
int delete_session_no_mutex(rtsp_session_t *session) {
  rtsp_session_t *s;

  s = (rtsp_session_t *)avl_delete(info.rtsp_sessions, session);

  if (s != NULL) {
    free_session(s);
    return 1;
  }

  return 0;
}

int delete_session_by_id(long int id) {
  rtsp_session_t *s;

  thread_mutex_lock(&info.session_mutex);

  s = find_session(id);

  if (s != NULL) {
    s = (rtsp_session_t *)avl_delete(info.rtsp_sessions, s);
    free_session(s);
    thread_mutex_unlock(&info.session_mutex);
    return 1;
  }

  thread_mutex_unlock(&info.session_mutex);

  return 0;
}

void free_session(rtsp_session_t *session) {
  if (session->mount != NULL)
  {
    nfree(session->mount);
  }
  if (session->transport_ip != NULL)
  {
    nfree(session->transport_ip);
  }
  if (session->con != NULL) {
    kick_connection(session->con, "Session deleted");
  } else {
    if (session->udp_sockfd > -1) sock_close (session->udp_sockfd);
  }
  nfree(session);
}

int setup_session(rtsp_session_t *session, connection_t *con, ntrip_request_t *req) {
  char time[50];

  if (session == NULL) {
    ntrip_write_message(con, RTSP_INTERNAL_SERVER_ERROR, req->cseq, get_formatted_time(HEADER_TIME, time));
    xa_debug (1, "DEBUG: setup_session: session NULL!");
    return 0;
  }

  xa_debug(1, "setup_session %p %p %p", session->mount, session->transport_ip,
  session->con);
  if (session->mount != NULL)
  {
    nfree(session->mount);
  }
  if (session->transport_ip != NULL)
  {
    nfree(session->transport_ip);
  }
  if ((session->con != NULL) && (session->con->food.client != NULL))
    kick_connection(session->con, "Setup session with existing connection");

  if (rtsp_parse_transport(con, session) != 1) {
    ntrip_write_message(con, RTSP_UNSUPPORTED_TRANSPORT, req->cseq, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  session->udp_sockfd = sock_get_udp_socket(session->transport_ip, session->client_port);

  if (session->udp_sockfd == SOCKET_ERROR) {
    ntrip_write_message(con, RTSP_INTERNAL_SERVER_ERROR, req->cseq, get_formatted_time(HEADER_TIME, time));
    xa_debug (1, "DEBUG: setup_session: could not get udp socket!");
    return 0;
  }

  session->server_port = get_socket_port(session->udp_sockfd);
  session->mount = strndup(req->path, BUFSIZE);
  session->con = NULL;
  session->state = READY_STATE;
  xa_debug(1, "setup_session sockfd %d port %d mount %s state %d req %p con %p",
  session->udp_sockfd, session->server_port, session->mount, session->state,
  req, con);

  req->sessid = session->id; // important!!. ajd
  con->session_id = session->id;

  return 1;
}

/* con->user and con->food.client->source are not set here. ajd */
connection_t *rtsp_create_client_connection(rtsp_session_t *session) {
  connection_t *con;

  if (session->udp_sockfd == SOCKET_ERROR) {
    xa_debug (1, "DEBUG: rtsp_create_client_connection: invalid udp socket!");
    return NULL;
  }

  con = create_connection();
  con->sock = session->udp_sockfd;
  put_client(con);
  con->food.client->type = rtsp_client_listener_e;
  con->id = new_id();
  con->type = client_e;
  con->host = strndup(session->transport_ip, BUFSIZE);
  con->data_protocol = rtp_e;
  con->trans_encoding = not_chunked_e;
  con->connect_time = get_time();
  con->session_id = session->id;
  con->rtp = rtp_create();
  con->rtp->datagram->ssrc = htonl(con->session_id);

  return con;
}

connection_t *rtsp_create_source_connection(rtsp_session_t *session)
{
  connection_t *con;

  if (session->udp_sockfd == SOCKET_ERROR)
  {
    xa_debug (1, "DEBUG: rtsp_create_source_connection: invalid udp socket!");
    return NULL;
  }

  con = create_connection();
  con->sock = session->udp_sockfd;
  put_source(con);
  con->food.source->audiocast.mount = my_strdup(session->mount);
  con->food.source->connected = SOURCE_CONNECTED;
  con->food.source->type = rtsp_client_source_e;
  add_global_stats(con->food.source);
  con->id = new_id();
  con->type = source_e;
  con->host = strndup(session->transport_ip, BUFSIZE);
  con->data_protocol = rtp_e;
  con->trans_encoding = not_chunked_e;
  con->connect_time = get_time();
  con->session_id = session->id;
  con->rtp = rtp_create();

  return con;
}

void rtsp_remove_connection_from_session(connection_t *con, long int id)
{
  rtsp_session_t *session;

  if (id > -1)
  {
    thread_mutex_lock (&info.session_mutex);
    session = find_session(id);

    if ((session != NULL) && (session->con != NULL))
    {
      if (con == session->con)
      {
        xa_debug (2, "DEBUG: removing rtp connection %ld from session %ld", session->con->id, session->con->session_id);
        session->con = NULL;
      }
    }
    thread_mutex_unlock (&info.session_mutex);
  }
}

int rtsp_parse_transport(connection_t *con, rtsp_session_t *session) {
  const char *t;
  char transport[BUFSIZE];
  char part[BUFSIZE];
  char *c;
  int go_on = 1;

  t = get_con_variable(con, "Transport");

  if (t == NULL) return 0;

  strncpy(transport, t, BUFSIZE);
  transport[BUFSIZE-1] = 0;

  if (splitc(part, transport, ',') != NULL) {
    strncpy(transport, part, BUFSIZE);
    transport[BUFSIZE-1] = 0;
  }

  if (strncmp(transport, "RTP/GNSS", 8) != 0) {
    write_log(LOG_DEFAULT, "WARNING: No RTP/GNSS Transport: %s", transport);
  }

  session->transport_ip = strndup(con->host, BUFSIZE);

  do {
    if (splitc(part, transport, ';') == NULL) {
      strncpy(part, transport, BUFSIZE);
      go_on = 0;
    }
    c = strchr(part, '=');
    if (c != NULL) {
      c++;
      if (strncmp(part, "destination", 11) == 0) {
        if (session->transport_ip != NULL)
        {
          nfree(session->transport_ip);
        }
        session->transport_ip = strndup(c, BUFSIZE-1);
      } else if (strncmp(part, "client_port", 11) == 0) {
        session->client_port = atoi(c);
      } else if (strncmp(part, "ttl", 3) == 0) {
        session->transport_ttl = atoi(c);
      }
    }
  } while (go_on);

  xa_debug (1, "DEBUG: rtsp_parse_transport: ip %s,  client port %d, ttl %d", session->transport_ip, session->client_port, session->transport_ttl);

  if (session->client_port > -1)
    return 1;
  else
    return 0;
}

int rtsp_options(connection_t *con, ntrip_request_t *req) {
  char buf[BUFSIZE];

  get_ntrip_method_string(buf);

  ntrip_write_message(con, RTSP_OPTIONS_OK, req->cseq, buf);

  write_log(LOG_DEFAULT, "Accepted rtsp options from client %d from [%s]", con->id, con_host (con));

  return 1;
}

int rtsp_describe(connection_t *con, ntrip_request_t *req) {
  FILE *ifp;
  int size = 0;

  ifp = fopen("../conf/sourcetable.dat","r");

  if (ifp != NULL) size = get_file_size(ifp);

  ntrip_write_message(con, RTSP_DESCRIBE_OK, req->cseq, "gnss/sourcetable", size);

  if (ifp != NULL) {
    sock_write_file(ifp, con->sock);
    fclose(ifp);
  }

  sock_write_line (con->sock, "\r\n");

  write_log(LOG_DEFAULT, "Accepted rtsp describe from client %d from [%s]", con->id, con_host (con));

  return 1;
}

int rtsp_setup(connection_t *con, ntrip_request_t *req) {
  rtsp_session_t *session;
  char time[50];
  const char *var;
  alias_t *wasalias = 0;
  connection_t *source = NULL;

  if (req->sessid > -1){
    ntrip_write_message(con, RTSP_AGGREGATE_NOT_ALLOWED, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  var = get_con_variable(con, "Ntrip-Component");

  if ((var == NULL) || (strncasecmp(var, "ntripclient", 6) == 0)) { // rtsp client.

    if (!authenticate_user_request (con, req, client_e)) {
      ntrip_write_message(con, RTSP_NOT_AUTHORIZED, req->cseq, get_formatted_time(HEADER_TIME, time),req->path);
      return 0;
    }

    thread_mutex_lock (&info.source_mutex);
    source = find_mount_with_req(req, &wasalias);
    thread_mutex_unlock (&info.source_mutex);

    if (source == NULL)  {
      ntrip_write_message(con, RTSP_SETUP_WRONG_MOUNT, req->cseq, get_formatted_time(HEADER_TIME, time),req->path);
      return 0;
    } else if(wasalias && !authenticate_user_request (con, wasalias->real, client_e)) {
      ntrip_write_message(con, RTSP_NOT_AUTHORIZED, req->cseq, get_formatted_time(HEADER_TIME, time),req->path);
      return 0;
    }

  } else { // rtsp source.

    if (!authenticate_user_request (con, req, source_e)) {
      ntrip_write_message(con, RTSP_NOT_AUTHORIZED, req->cseq, get_formatted_time(HEADER_TIME, time),req->path);
      return 0;
    }

    thread_mutex_lock (&info.source_mutex);

    if (mount_exists (req->path)) {
      thread_mutex_unlock(&info.source_mutex);
      ntrip_write_message(con, RTSP_SETUP_MOUNT_CONFLICT, req->cseq, get_formatted_time(HEADER_TIME, time),req->path);
      return 0;
    }

    thread_mutex_unlock (&info.source_mutex);
  }

  session = get_new_session();

  if (session == NULL) {
    ntrip_write_message(con, RTSP_SERVICE_UNAVAILABLE, req->cseq, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (setup_session(session, con, req) != 1) {
    xa_debug (1, "DEBUG: rtsp_setup: could not setup session!");
    delete_session(session);
    return 0;
  }

  ntrip_write_message(con, RTSP_SETUP_OK, req->cseq, req->sessid, session->client_port, session->server_port, get_formatted_time(HEADER_TIME, time), "gnss/data");

  write_log(LOG_DEFAULT, "Accepted rtsp setup from client %d (NTRIP Component: %s) from [%s], session %d", con->id, nullcheck_string(var), con_host (con), session->id);
  return 1;
}

int rtsp_play(connection_t *con, ntrip_request_t *req) {
  char time[50];
  rtsp_session_t *session;
  connection_t *source = NULL;

  thread_mutex_lock (&info.double_mutex);
  thread_mutex_lock (&info.source_mutex);
  thread_mutex_lock (&info.session_mutex);

  session = find_session(req->sessid);

  if (session == NULL) {
    thread_mutex_unlock (&info.session_mutex);
    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);
    ntrip_write_message(con, RTSP_SESSION_NOT_FOUND, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->state != READY_STATE) {
    thread_mutex_unlock (&info.session_mutex);
    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);
    ntrip_write_message(con, RTSP_METHOD_NOT_VALID, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->con != NULL) {
    session->con->food.client->alive = CLIENT_UNPAUSED;
  } else {
    source = find_mount(session->mount);

                xa_debug(2, "DEBUG: rtsp_play() id %d Group %s", con->id, con->group ? con->group : "<none>");
    if (source == NULL)  {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      ntrip_write_message(con, RTSP_PLAY_WRONG_MOUNT, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    if ((info.num_clients >= info.max_clients) || (source->food.source->num_clients >= info.max_clients_per_source)) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      if (info.num_clients >= info.max_clients)
        xa_debug (2, "DEBUG: inc > imc: %lu %lu", info.num_clients, info.max_clients);
      else if (source->food.source->num_clients >= info.max_clients_per_source)
        xa_debug (2, "DEBUG: snc > smc: %lu %lu", source->food.source->num_clients, info.max_clients_per_source);
      else
        xa_debug (1, "ERROR: Erroneous number of clients");

      ntrip_write_message(con, RTSP_SERVICE_UNAVAILABLE, req->cseq, get_formatted_time(HEADER_TIME, time));
      return 0;
    } else if (!check_ip_restrictions(con)) {
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      ntrip_write_message(con, HTTP_SERVICE_UNAVAILABLE, get_formatted_time(HEADER_TIME, time));
      kick_not_connected (con, "Server Full (too many accesses from IP)");
      return 0;
    }

    if (!add_group_connection(con)) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      ntrip_write_message(con, RTSP_SERVICE_UNAVAILABLE, req->cseq, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    session->con = rtsp_create_client_connection(session);

    if (session->con == NULL) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      ntrip_write_message(con, RTSP_INTERNAL_SERVER_ERROR, req->cseq, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    xa_debug (2, "DEBUG: rtsp_play: created client rtp connection %ld, session %ld, socket %d", session->con->id, session->con->session_id, session->con->sock);

    sock_set_blocking(session->con->sock, SOCK_BLOCKNOT);
    session->con->food.client->source = source->food.source;
    session->con->food.client->virgin = 1;

    session->con->headervars = create_header_vars();
    add_varpair2(session->con->headervars, nstrdup("User-Agent"), nstrdup(get_user_agent(con)));

    thread_mutex_lock(&info.client_mutex);
    avl_insert(info.clients, session->con);
    thread_mutex_unlock(&info.client_mutex);

    util_increase_total_clients();
    pool_add(session->con);
  }

  session->state = PLAY_STATE;

  thread_mutex_unlock (&info.session_mutex);
  thread_mutex_unlock (&info.source_mutex);
  thread_mutex_unlock (&info.double_mutex);

  ntrip_write_message(con, RTSP_PLAY_OK, req->cseq, req->sessid);

  if(strcmp(source->food.source->audiocast.mount, req->path)) {
    write_log(LOG_DEFAULT, "Accepted rtsp play from client %d from [%s] on mountpoint [%s] alias [%s], session %d", con->id, con_host (con), source->food.source->audiocast.mount, req->path, session->id);
  }
  else {
    write_log(LOG_DEFAULT, "Accepted rtsp play from client %d from [%s] on mountpoint [%s], session %d", con->id, con_host (con), source->food.source->audiocast.mount, session->id);
  }

  return 1;
}

int rtsp_post(connection_t *con, ntrip_request_t *req) {
  char time[50];
  rtsp_session_t *session;

  thread_mutex_lock (&info.double_mutex);
  thread_mutex_lock (&info.source_mutex);
  thread_mutex_lock (&info.session_mutex);

  session = find_session(req->sessid);

  if (session == NULL) {
    thread_mutex_unlock (&info.session_mutex);
    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);
    ntrip_write_message(con, RTSP_SESSION_NOT_FOUND, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->state != READY_STATE) {
    thread_mutex_unlock (&info.session_mutex);
    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);
    ntrip_write_message(con, RTSP_METHOD_NOT_VALID, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->con != NULL) {
    session->con->food.source->connected = SOURCE_CONNECTED;
  } else {
    if (mount_exists(session->mount)) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);
      ntrip_write_message(con, RTSP_POST_MOUNT_CONFLICT, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    if ((info.num_sources + 1) > info.max_sources) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);
      ntrip_write_message(con, RTSP_SERVICE_UNAVAILABLE, req->cseq, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    session->con = rtsp_create_source_connection(session);

    if (session->con == NULL) {
      thread_mutex_unlock (&info.session_mutex);
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);
      ntrip_write_message(con, RTSP_INTERNAL_SERVER_ERROR, req->cseq, get_formatted_time(HEADER_TIME, time));
      return 0;
    }

    xa_debug (2, "DEBUG: rtsp_post: created source rtp connection %ld, session %ld, socket %d", session->con->id, session->con->session_id, session->con->sock);

    session->con->headervars = create_header_vars();
    add_varpair2(session->con->headervars, nstrdup("User-Agent"), nstrdup(get_user_agent(con)));

    add_source();
    avl_insert(info.sources, session->con);

    thread_create("Source Thread", source_rtsp_function, (void *)session->con);
  }

  session->state = POST_STATE;

  thread_mutex_unlock (&info.session_mutex);
  thread_mutex_unlock (&info.source_mutex);
  thread_mutex_unlock (&info.double_mutex);

  ntrip_write_message(con, RTSP_POST_OK, req->cseq, req->sessid);

  write_log(LOG_DEFAULT, "Accepted rtsp post from source %d from [%s], session %d", con->id, con_host (con), session->id);

  return 1;
}

int rtsp_pause(connection_t *con, ntrip_request_t *req) {
  char time[50];
  rtsp_session_t *session;

  thread_mutex_lock (&info.session_mutex);

  session = find_session(req->sessid);

  if (session == NULL) {
    thread_mutex_unlock (&info.session_mutex);
    ntrip_write_message(con, RTSP_SESSION_NOT_FOUND, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if ((session->state != PLAY_STATE) && (session->state != POST_STATE)) {
    thread_mutex_unlock (&info.session_mutex);
    ntrip_write_message(con, RTSP_METHOD_NOT_VALID, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->con != NULL) {
    if (session->con->type == source_e)
      session->con->food.source->connected = SOURCE_PAUSED;
    else
      session->con->food.client->alive = CLIENT_PAUSED;
//    session->con = NULL;
  }

  session->state = READY_STATE;

  thread_mutex_unlock (&info.session_mutex);

  ntrip_write_message(con, RTSP_PAUSE_OK, req->cseq, req->sessid);

  write_log(LOG_DEFAULT, "Accepted rtsp pause from client %d from [%s], session %d", con->id, con_host (con), session->id);

  return 1;
}

int rtsp_get_parameter(connection_t *con, ntrip_request_t *req) {
  char time[50];
  rtsp_session_t *session;

  thread_mutex_lock (&info.session_mutex);

  session = find_session(req->sessid);

  if (session == NULL) {
    thread_mutex_unlock (&info.session_mutex);
    ntrip_write_message(con, RTSP_SESSION_NOT_FOUND, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  thread_mutex_unlock (&info.session_mutex);

  ntrip_write_message(con, RTSP_GET_PARAMETER_OK, req->cseq, req->sessid);

  xa_debug (1, "DEBUG: Accepted rtsp get_parameter from client %d from [%s], session %d", con->id, con_host (con), req->sessid);

  return 1;
}

int rtsp_teardown(connection_t *con, ntrip_request_t *req) {
  char time[50];
  rtsp_session_t *session;

  thread_mutex_lock (&info.session_mutex);

  session = find_session(req->sessid);

  if (session == NULL) {
    thread_mutex_unlock (&info.session_mutex);
    ntrip_write_message(con, RTSP_SESSION_NOT_FOUND, req->cseq, req->sessid, get_formatted_time(HEADER_TIME, time));
    return 0;
  }

  if (session->con != NULL) {
    kick_connection(session->con, "RTSP teardown");

    session->con = NULL;
    session->udp_sockfd = -1;
  }

  if (delete_session_no_mutex(session) != 1) {
    write_log(LOG_DEFAULT, "WARNING: Could not delete session %d", req->sessid);
  }

  thread_mutex_unlock (&info.session_mutex);

  ntrip_write_message(con, RTSP_TEARDOWN_OK, req->cseq, req->sessid);

  write_log(LOG_DEFAULT, "Accepted rtsp teardown from client %d from [%s], session %d", con->id, con_host (con), req->sessid);

  return 1;
}
