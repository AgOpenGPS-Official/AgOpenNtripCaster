/* client.c
 * - Client functions
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
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "client.h"
#include "threads.h"
#include "connection.h"
#include "log.h"
#include "source.h"
#include "sock.h"
#include "rtsp.h"
#include "restrict.h"
#include "memory.h"
#include "admin.h"
#include "http.h"
#include "vars.h"
#include "commands.h"
#include "authenticate/basic.h"
#include "sourcetable.h"
#include "match.h"
#include "pool.h"
#include "logtime.h"
#include "sourcetable.h"

#include <signal.h>

extern server_info_t info;

static int isntripclient(connection_t *con)
{
  int res = 1;
  const char *useragent = get_user_agent(con);
  if(useragent)
  {
    while(*useragent && (
    tolower(useragent[0]) != 'n' ||
    tolower(useragent[1]) != 't' ||
    tolower(useragent[2]) != 'r' ||
    tolower(useragent[3]) != 'i' ||
    tolower(useragent[4]) != 'p'))
      ++useragent;
    if(!*useragent)
      res = 0;
  }
  return res;
}

void http_client_login(connection_t *con, ntrip_request_t *req) {
  connection_t *source;
  const char *var;
  char time[50];
  alias_t *wasalias = 0;

  xa_debug(3, "http client login...");

  if (!con) {
    write_log(LOG_DEFAULT, "WARNING: http_client_login called with NULL pointer");
    return;
  }

  if (info.throttle_on) {
    ntrip_write_message(con, HTTP_NOT_ACCEPTABLE, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Bandwidth usage too high (throttling)");
    return;
  }
#ifdef HAVE_LIBWRAP
  if (con->sock > 0 && !sock_check_libwrap (con->sock, client_e)) {
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Access denied (tcp wrappers)");
    return;
  }
#endif
  if (!allowed(con, client_e)) {
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Access denied (internal acl list, client connection)");
    return;
  }

  if (is_empty_request(req)) {
    if(!isntripclient(con))
      con->com_protocol = ntrip2_0_e;
    if(con->udpbuffers && !info.sourcetable_via_udp)
    {
      kick_not_connected (con, "No sourcetable via UDP allowed");
      return;
    }
    send_sourcetable(con);
    kick_not_connected (con, "Transfer sourcetable");
    return;
  }

  if (!strncasecmp((req->path)+1,"?filter",7)) {
    if(con->udpbuffers && !info.sourcetable_via_udp)
    {
      kick_not_connected (con, "No sourcetable via UDP allowed");
      return;
    }
    send_sourcetable_filtered(con, (req->path)+1+7,0);
    kick_not_connected (con, "Transfer filtered sourcetable");
    return;
  }

  if (!strncasecmp((req->path)+1,"?match",6)) {
    if(con->udpbuffers && !info.sourcetable_via_udp)
    {
      kick_not_connected (con, "No sourcetable via UDP allowed");
      return;
    }
    send_sourcetable_filtered(con, (req->path)+1+6,1);
    kick_not_connected (con, "Transfer matched sourcetable");
    return;
  }


  if (!strncasecmp((req->path)+1,"?auth",5) ||!strncasecmp((req->path)+1,"?strict",7)) {
    ntrip_write_message(con, HTTP_NOT_IMPLEMENTED,
      get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "auth and strict not implemented for sourcetable");
    return;
  }

  if (!authenticate_user_request (con, req, client_e)) {
    ntrip_write_message(con, HTTP_GET_NOT_AUTHORIZED, get_formatted_time(HEADER_TIME, time), req->path, "text/html");
    kick_not_connected (con, "Not authorized");
    return;
  }

  if (strncasecmp(get_user_agent(con), "ntrip", 5) != 0) {
    if ((ntripcaster_strncmp(req->path, "/home", 5) == 0)) {
      http_display_home_page (con);
      kick_not_connected (con, "Home page displayed");
      return;
    } else if ((ntripcaster_strncmp(req->path, "/favicon.ico", 12) == 0)) {
      http_get_favicon (con);
      kick_not_connected (con, "Favicon delivered");
      return;
    } else if ((ntripcaster_strncmp(req->path, "/robots.txt", 11) == 0)) {
      http_get_robots (con);
      kick_not_connected (con, "Robots.txt delivered");
      return;
    } else if ((ntripcaster_strncmp(req->path, "/admin", 6) == 0)) {
/*      char secfile[BUFSIZE];

      if (get_ntripcaster_file (info.client_mountfile, conf_file_e, R_OK, secfile) == NULL) {
        ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
        kick_not_connected (con, "Access denied (no mountfile found)");
        return;
      }
*/

      if (http_admin_command (con, req)) {
        xa_debug (2, "DEBUG: kicking %s, executed admin command", con_host (con));
        kick_not_connected (con, "Executed admin command");
      } else
        kick_not_connected (con, "Failed to execute admin command");
      return;
    }

    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time), req->path, "text/html");
    kick_not_connected (con, "No NTRIP client");
    return;
  }

  xa_debug (1, "Looking for mount [%s:%d%s]", req->host, req->port, req->path);

  thread_mutex_lock (&info.double_mutex);
  thread_mutex_lock (&info.source_mutex);

  source = find_mount_with_req (req, &wasalias);

  if (wasalias && !authenticate_user_request (con, wasalias->real, client_e)) {
    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);

    ntrip_write_message(con, HTTP_GET_NOT_AUTHORIZED, get_formatted_time(HEADER_TIME, time), req->path, "text/html");
    kick_not_connected (con, "Not authorized");
    return;
  }

  xa_debug(2, "DEBUG: http_client_login() id %d Group %s", con->id, con->group ? con->group : "<none>");
  if (source == NULL)  {

    thread_mutex_unlock (&info.source_mutex);
    thread_mutex_unlock (&info.double_mutex);

    if (con->com_protocol == ntrip2_0_e)
      ntrip_write_message(con, HTTP_GET_STREAM_WRONG_MOUNT, get_formatted_time(HEADER_TIME, time));
    else
      send_sourcetable(con);
    //xa_debug (1, "DEBUG: http_client_login(): Try kicking with no existing mountpoint");
    kick_not_connected (con, "No existing mountpoint");
    //xa_debug (1, "DEBUG: http_client_login(): end");
    return;
  } else {
    if ((info.num_clients >= info.max_clients) || (source->food.source->num_clients >= info.max_clients_per_source)) {
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      if (info.num_clients >= info.max_clients)
        xa_debug (2, "DEBUG: inc > imc: %lu %lu", info.num_clients, info.max_clients);
      else if (source->food.source->num_clients >= info.max_clients_per_source)
        xa_debug (2, "DEBUG: snc > smc: %lu %lu", source->food.source->num_clients, info.max_clients_per_source);
      else
        xa_debug (1, "ERROR: Erroneous number of clients, what the hell is going on?");

      ntrip_write_message(con, HTTP_SERVICE_UNAVAILABLE, get_formatted_time(HEADER_TIME, time));
      kick_not_connected (con, "Server Full (too many listeners)");
      return;
    } else if (!check_ip_restrictions(con)) {
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);

      ntrip_write_message(con, HTTP_SERVICE_UNAVAILABLE, get_formatted_time(HEADER_TIME, time));
      kick_not_connected (con, "Server Full (too many accesses from IP)");
      return;
    }

    if (!add_group_connection(con)) {
      thread_mutex_unlock (&info.source_mutex);
      thread_mutex_unlock (&info.double_mutex);
      ntrip_write_message(con, HTTP_SERVICE_UNAVAILABLE, get_formatted_time(HEADER_TIME, time));
      kick_not_connected (con, "No more connections allowed for group");
      return;
    }

    put_client(con);
    con->food.client->type = http_client_e;
    con->food.client->source = source->food.source;
    var = get_con_variable(con, "Referer");
    if (var && strncmp(var, "RELAY", 5) == 0) con->food.client->type = pulling_client_e;

    thread_mutex_lock(&info.client_mutex);
    avl_insert(info.clients, con);
    thread_mutex_unlock(&info.client_mutex);

//    source->food.source->stats.client_connections++;


    greet_client(con, source->food.source);
    util_increase_total_clients();
    pool_add (con);

    if(strcmp(source->food.source->audiocast.mount, req->path)) {
      write_log(LOG_DEFAULT, "Accepted http client %d from [%s] on mountpoint [%s] alias [%s]. %d clients connected", con->id, con_host (con), source->food.source->audiocast.mount, req->path, info.num_clients);
    }
    else {
      write_log(LOG_DEFAULT, "Accepted http client %d from [%s] on mountpoint [%s]. %d clients connected", con->id, con_host (con), source->food.source->audiocast.mount, info.num_clients);
    }
  }

  thread_mutex_unlock (&info.source_mutex);
  thread_mutex_unlock (&info.double_mutex);
}

client_t *create_client() {
  client_t *client = (client_t *)nmalloc(sizeof(client_t));
  client->type = unknown_client_e;
  return client;
}

void put_client(connection_t *con) {
  client_t *cli = create_client();
  con->food.client = cli;
  cli->errors = 0;
  cli->type = unknown_client_e;
  cli->write_bytes = 0;
  cli->virgin = -1;
  cli->source = NULL;
  cli->cid = -1;
  cli->offset = 0;
  cli->alive = CLIENT_ALIVE;
  con->type = client_e;
}

void util_increase_total_clients () {
  internal_lock_mutex (&info.misc_mutex);
  info.num_clients++;
  info.hourly_stats.client_connections++;
  internal_unlock_mutex (&info.misc_mutex);
}

void
util_decrease_total_clients () {
  internal_lock_mutex (&info.misc_mutex);
  info.num_clients--;
  internal_unlock_mutex (&info.misc_mutex);
}

void
del_client(connection_t *client, source_t *source)
{
  if (!client || !source) {
    write_log(LOG_DEFAULT, "WARNING: del_client() called with NULL pointer");
    return;
  }

  if (source && client->food.client && (client->food.client->virgin != 1) && (client->food.client->virgin != -1)) {
    if (source->num_clients == 0)
      write_log (LOG_DEFAULT, "WARNING: Bloody going below limits on client count!");
    else
      source->num_clients--;
  }
  util_decrease_total_clients ();
}


void
greet_client(connection_t *con, source_t *source)
{
#ifdef _WIN32
  int bufsize = 16384; /* Is that a buffer in your pocket, or are you just happy to see me? :) */
#endif
  char time[50];

  if (!con) {
    write_log(LOG_DEFAULT, "WARNING: greet_client called with NULL pointer");
    return;
  }

  ntrip_write_message(con, con->udpbuffers ? UDP_GET_STREAM_OK : HTTP_GET_STREAM_OK, get_formatted_time(HEADER_TIME, time), "gnss/data", (con->trans_encoding == chunked_e)?"chunked":"not chunked", con->udpbuffers ? con->udpbuffers->ssrc : 0);

  if (con->sock > 0 && sock_set_blocking(con->sock, SOCK_BLOCKNOT) < 0)
    write_log(LOG_DEFAULT, "WARNING: sock_set_blocking in greet_client failed");

  if(con->udpbuffers)
  {
    con->rtp->datagram->pt = 96;
    con->rtp->datagram->ssrc = htonl(con->udpbuffers->ssrc); /* copy changed ssrc */
  }

#ifdef _WIN32
  if(con->sock > 0)
    setsockopt(con->sock, SOL_SOCKET, SO_SNDBUF, (char *)&bufsize, sizeof(int));
#endif
  con->food.client->virgin = 1;
}

void
describe_client (const com_request_t *req, const connection_t *clicon)
{
  const client_t *client;

  if (!req || !clicon)
  {
    xa_debug (1, "WARNING: describe_client(): called with NULL pointers");
    return;
  }

  if (clicon->type != client_e)
  {
    xa_debug (1, "WARNING: describe_client(): called with invalid type");
    return;
  }

  describe_connection (req, clicon);

  client = clicon->food.client;

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_START, "Misc client info:");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Transfer error balance: %d", client_errors (client));
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Transfer chunk id and offset: %d : %d", client->cid, client->offset);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Bytes transfered: %lu", client->write_bytes);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Virgin: %s", client->virgin ? "yes" : "no");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Client type: %s", client_type (clicon));
  if (client->source && client->source->audiocast.mount)
    admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_MISC, "Mountpoint: %s", client->source->audiocast.mount);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_CLIENT_END, "End of client info");
}

const char * client_types[5] = { "unknown client", "http client", "rtsp client", "puller", "rtsp client listener" };

const char *
client_type (const connection_t *clicon) {
  return client_types[clicon->food.client->type+1];
}

int
client_errors (const client_t *client)
{
  if (!client || !client->source)
    return 0;

  return (CHUNKLEN - (client->cid - client->source->cid)) % CHUNKLEN;
}
