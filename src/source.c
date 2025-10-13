/* source.c
 * - Source functions
 *
 * Copyright (c) 2023
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
#include <fcntl.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#else
#include <io.h>
#include <winsock.h>
#define write _write
#define read _read
#define close _close
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "rtsp.h"
#include "rtp.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "source.h"
#include "sourcetable.h"
#include "sock.h"
#include "log.h"
#include "connection.h"
#include "avl_functions.h"
#include "main.h"
#include "timer.h"
#include "alias.h"
#include "relay.h"
#include "client.h"
#include "commands.h"
#include "restrict.h"
#include "memory.h"
#include "admin.h"
#include "http.h"
#include "pool.h"
#include "logtime.h"
#include "vars.h"
#include "authenticate/basic.h"
#ifdef HAVE_TLS
#include "tls.h"
#endif /* HAVE_TLS */

/* in microseconds */
#define READ_RETRY_DELAY 400
#define READ_TIMEOUT 16000

extern server_info_t info;
const int source_read_sleep = READ_RETRY_DELAY * 1000; // in microseconds
const int source_read_tries = READ_TIMEOUT / READ_RETRY_DELAY;

void add_global_stats(source_t *source)
{
  statistics_t *stats = 0;
  statisticsentry_t *s;

  avl_traverser trav = {0};

  thread_mutex_lock (&info.sourcesstats_mutex);
  while ((s = avl_traverse (info.sourcesstats, &trav)))
  {
    if ((ntripcaster_strcmp (s->mount, source->audiocast.mount) == 0))
    {
      stats = &s->stats;
      break;
    }
  }
  if(!stats) /* add */
  {
    s = (statisticsentry_t *)nmalloc(sizeof(statisticsentry_t));
    memset(s, 0, sizeof(statisticsentry_t));
    s->mount = nstrdup(source->audiocast.mount);
    stats = &s->stats;
    avl_insert(info.sourcesstats, s);
    zero_stats (stats);
  }
  thread_mutex_unlock (&info.sourcesstats_mutex);

  stats->source_connections++;
  source->globalstats = stats;
}

void http_source_login(connection_t *con, ntrip_request_t *req) {
  source_t *source;
  int num_sources;
  char time[50];

  xa_debug (2, "DEBUG: HTTP Encoder logging in with path [%s]", req->path);

#ifdef HAVE_LIBWRAP
  if (con->sock > 0 && !sock_check_libwrap (con->sock, source_e)) {
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Access denied (tcp wrappers)");
    return;
  }
#endif
  if (!allowed (con, source_e)) {
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "Access denied (internal acl list, source connection)");
    return;
  }

  if (strncasecmp(get_source_agent(con), "ntrip", 5) != 0) { // rtsp
    ntrip_write_message(con, HTTP_FORBIDDEN, get_formatted_time(HEADER_TIME, time));
    kick_not_connected (con, "No NTRIP source");
    return;
  }

  if (authenticate_source_request(con, req) != 1) {
    ntrip_write_message(con, HTTP_SOURCE_NOT_AUTHORIZED, get_formatted_time(HEADER_TIME, time),req->path, "text/html");
    kick_not_connected (con, "Unauthorized source");
    return;
  }

  if (is_empty_request(req)) {
    ntrip_write_message(con, HTTP_BAD_REQUEST, get_formatted_time(HEADER_TIME, time));
    kick_not_connected(con, "Empty source request");
    return;
  }

  put_source(con);

  source = con->food.source;
  source->type = http_source_e;
  source->audiocast.mount = my_strdup(req->path);
  add_global_stats(source);

  thread_mutex_lock(&info.source_mutex);

  if (mount_exists (source->audiocast.mount)) {
    thread_mutex_unlock(&info.source_mutex);
    ntrip_write_message(con, HTTP_SOURCE_MOUNT_CONFLICT, get_formatted_time(HEADER_TIME, time));
    kick_connection (con, "Source with existing Mountpoint");
    return;
  }

  if ((info.num_sources + 1) > info.max_sources) {
    thread_mutex_unlock(&info.source_mutex);
    ntrip_write_message(con, HTTP_SERVICE_UNAVAILABLE, get_formatted_time(HEADER_TIME, time));
    kick_connection (con, "Server Full (too many streams)");
    return;
  }

  ntrip_write_message(con, con->udpbuffers ? UDP_SOURCE_OK : HTTP_SOURCE_OK, get_formatted_time(HEADER_TIME, time), (con->trans_encoding==not_chunked_e)?"not chunked":"chunked", con->udpbuffers ? con->udpbuffers->ssrc : 0);

  if(con->udpbuffers)
  {
    con->rtp->datagram->pt = 96;
    con->rtp->datagram->ssrc = htonl(con->udpbuffers->ssrc); /* copy changed ssrc */
  }

  add_source();
  source->connected = SOURCE_CONNECTED;
  avl_insert(info.sources, con);

  num_sources = info.num_sources; // store it, so we can unlock before write_log() call
  thread_mutex_unlock(&info.source_mutex);

  write_log (LOG_DEFAULT, "Accepted encoder on mountpoint %s from %s. %d sources connected", source->audiocast.mount, con_host(con), num_sources);

  thread_rename("Source Thread");

  source_func(con);
}

int authenticate_source_request(connection_t *con, ntrip_request_t *req) {
  const char *var;

  if (con->com_protocol == ntrip1_0_e) {
    var = get_con_variable(con, "Authorization");
    if (var == NULL) return 0;

    xa_debug(1, "DEBUG: authenticate_source_request(): NTRIP1.0: checking pass %s", var);

xa_debug(2, "DEBUG: Enc %s", info.encoder_pass);
    if (strncmp(info.encoder_pass, var, BUFSIZE) == 0)
      return 1;
    return authenticate_user_request_ntrip1upload(con, req, var);
  } else {
    xa_debug(1, "DEBUG: authenticate_source_request(): NTRIP2.0: checking source user");

    return authenticate_user_request(con, req, source_e);
  }

  return 0;
}

void *source_rtsp_function(void *conarg) {
  thread_init();
  source_func(conarg);
  thread_exit(0);
  return NULL;
}

/*
   A lot can be said about termination of this thread.
   Either it gets killed by another thread, using the kick_connection (thiscon,..),
   which should set the connected value to SOURCE_KILLED, and let this thread
   exit on it's on with the close_connection at the end.
   Or it kills itself, when the encoder dies, and then it should call kick_connection (thiscon,..),
   on itself, setting the value of connected to SOURCE_KILLED, and exit through close_connection () */
void *
source_func(void *conarg)
{
  source_t *source;
  avl_traverser trav = {0};
  connection_t *clicon, *con = (connection_t *)conarg;
  mythread_t *mt;
  int i;

  source = con->food.source;
  con->food.source->thread = thread_self();

  mt = thread_get_mythread ();

  if(con->sock > 0)
    sock_set_blocking(con->sock, SOCK_BLOCKNOT);

  sourcetable_add_source(source);

  while (thread_alive (mt) && ((source->connected == SOURCE_CONNECTED) || (source->connected == SOURCE_PAUSED)))
  {
    source_get_new_clients (source);

    add_chunk(con);

    for (i = 0; i < 10; i++) {

      if (source->connected != SOURCE_CONNECTED)
        break;

      zero_trav (&trav);

      while ((clicon = avl_traverse(source->clients, &trav)) != NULL) {

        if (source->connected == SOURCE_KILLED || source->connected == SOURCE_PAUSED)
          break;

        source_write_to_client (source, clicon);

      }

      if (mt->ping == 1)
        mt->ping = 0;
    }
    kick_dead_clients (source); //-> client_mutex, authentication_mutex (in close_connection) locked inside.
  }
  sourcetable_remove_source(source);

  thread_mutex_lock (&info.double_mutex);
  thread_mutex_lock (&info.source_mutex);

  source_get_new_clients (source); // to clean the pool before source dies.

  close_connection (con); //-> client_mutex (in kick_dead_clients), authentication_mutex locked inside.

  thread_mutex_unlock (&info.source_mutex);
  thread_mutex_unlock (&info.double_mutex);

  return NULL;
}

source_t *
create_source()
{
  source_t *source = (source_t *)nmalloc(sizeof(source_t));
  memset(source,0,sizeof(source_t));
  source->type = unknown_source_e;
  return source;
}

void
put_source(connection_t *con)
{
  register int i;
  source_t *source = create_source();

  con->food.source = source;
  zero_stats (&source->stats);
  source->globalstats = NULL;
  source->connected = SOURCE_UNUSED;
  source->type = unknown_source_e;
  source->audiocast.name = NULL;
  source->audiocast.mount = NULL;
  source->cid = 0;
  source->clients = avl_create (compare_connection, &info);
  source->num_clients = 0;
  source->priority = 0;

  for (i = 0; i < CHUNKLEN; i++)
  {
    source->chunk[i].clients_left = 0;
    source->chunk[i].len = 0;
  }

  con->type = source_e;
}

void
add_source ()
{
  info.num_sources++;
  info.hourly_stats.source_connections++;
}

void
del_source ()
{
  info.num_sources--;
}

connection_t *
find_mount(char *mount) {
//  char tempbuf[256] = "";
  connection_t *con;
  alias_t *alias = NULL;
  int true = 0;
  avl_traverser trav = {0};

  if (!mount) {
    write_log (LOG_DEFAULT, "WARNING: find_mount called with NULL mount!");
    return NULL;
  }

  // First look for aliases
  alias = get_alias_mount (mount);

  if (alias)
  {
    xa_debug (1, "DEBUG: Found alias [%s] -> [%s:%d%s]", mount,
         alias->real->host, alias->real->port, alias->real->path);
    return find_mount_with_req (alias->real, &alias);
  }

  while ((con = avl_traverse(info.sources, &trav)) != NULL) {
    true = 0;

    xa_debug(2, "DEBUG: Looking on mount [%s]", con->food.source->audiocast.mount);

    if (con->food.source->audiocast.mount[0] == '/') {
      if (ntripcaster_strcmp(con->food.source->audiocast.mount, mount) == 0) true = 1;
    } else {
      if (ntripcaster_strcmp(con->food.source->audiocast.mount, mount+1) == 0) true = 1;
    }

    if (true) {
      xa_debug(1, "DEBUG: Found local mount for [%s]", mount);
      return con;
    }
  }
  return NULL;
}

/* Must have source and double mutex to call this */
connection_t *
find_mount_with_req (ntrip_request_t *req, alias_t **wasalias)
{
  char tempbuf[256] = "";

  connection_t *con;
  alias_t *alias = NULL;
  ntrip_request_t search;

  avl_traverser trav = {0};

  if (!req || !req->path[0] || !req->host[0])
  {
    write_log (LOG_DEFAULT, "WARNING: find_mount_with_req called with NULL request!");
    return NULL;
  }

  xa_debug (1, "DEBUG: Looking for [%s] on host [%s] on port %d", req->path, req->host, req->port);
  xa_debug (1, "DEBUG: Searching local aliases");

  // First look for aliases
  alias = get_alias_whole (req);

  if (alias && hostname_local (req->host) &&
      hostname_local (alias->real->host) && (req->port == alias->real->port))
  {
    *wasalias = alias;
    xa_debug (1, "DEBUG: Found alias [%s:%d%s] -> [%s:%d%s]", req->host, req->port, req->path,
         alias->real->host, alias->real->port, alias->real->path);
    return find_mount_with_req (alias->real, wasalias);
  }

  xa_debug (1, "DEBUG: Search local mount points path %s host %s port %d",
  req->path, req->host, req->port);

  int pathlen = strlen(req->path);

  while ((con = avl_traverse(info.sources, &trav)) != NULL)
  {
    int true = 0;
    int mlen = strlen(con->food.source->audiocast.mount);

    zero_request(&search);

    /* shortcut to prevent complicated next steps, mount must end with path */
    if(mlen < pathlen || strcmp(con->food.source->audiocast.mount+mlen-pathlen,
    req->path))
      continue;

    generate_request(con->food.source->audiocast.mount, &search);

    strncpy(tempbuf, con->food.source->audiocast.mount, 252);

    if (search.path[0] && (ntripcaster_strcasecmp(search.host, req->host) == 0)
    && (search.port == req->port) && ((ntripcaster_strcmp(search.path,
    req->path) == 0) || (ntripcaster_strcmp(tempbuf, req->path) == 0))) {
      true = 1;
    } else if (con->food.source->audiocast.mount[0] == '/') { /* regular mount */
      if ((hostname_local(req->host) && ((ntripcaster_strcmp(
      con->food.source->audiocast.mount, req->path) == 0)
      || (ntripcaster_strcmp(tempbuf, req->path) == 0)))) {
        true = 1;
      }
    } else {
      if ((hostname_local(req->host) && ((ntripcaster_strcmp(
      con->food.source->audiocast.mount, req->path + 1) == 0)
      || (ntripcaster_strcmp(tempbuf, req->path + 1) == 0)))) {
        true = 1;
      }
    }

    if (true) {
      if (con->food.source->connected == SOURCE_CONNECTED) {
        xa_debug(1, "DEBUG: Found local mount for [%s]", req->path);
        return con;
      } else
        return NULL;
    }
  }

  xa_debug (1, "DEBUG: End search local mount points");

  return NULL;
}

void
add_chunk (connection_t *con)
{
  int read_bytes = 0;
  int len = -1;
  int tries = 0;
  int maxread = (int)(0.5 * SOURCE_READSIZE);

  if (con->food.source->connected == SOURCE_KILLED) return;

  if (con->food.source->chunk[con->food.source->cid].clients_left > 0)
    kick_trailing_clients(con->food.source); // rtsp

  do
  {
    errno = 0;
#ifdef _WIN32
    if(con->sock > 0)
      sock_set_blocking(con->sock, SOCK_BLOCK);
#endif

    switch (con->data_protocol)
    {
      case tcp_e:
      {
        if (con->trans_encoding == chunked_e)
        {
          if (con->http_chunk->left <= 0)
          {
#ifdef HAVE_TLS
            if(con->tls_socket)
              len = tls_read_line(con->tls_socket, con->http_chunk->buf + con->http_chunk->off, 20 - con->http_chunk->off);
            else
#endif
            len = sock_read_line(con->sock, con->http_chunk->buf + con->http_chunk->off, 20 - con->http_chunk->off);

            xa_debug (5, "DEBUG: add_chunk: hex [%s], read=%d (%d) of max=%d on mountpoint [%s] tries=%d", con->http_chunk->buf,
            len, strlen(con->http_chunk->buf), 20 - con->http_chunk->off, con->food.source->audiocast.mount, tries);

            if (len > (20 - con->http_chunk->off))
            { // a whole line could be read.
              if(con->http_chunk->finish)
              {
                xa_debug (4, "DEBUG: add_chunk: read trailing [%s] on mountpoint [%s]", con->http_chunk->buf, con->food.source->audiocast.mount);
                con->http_chunk->finish = 0;
                continue;
              }
              else
              {
                con->http_chunk->left = strtol(con->http_chunk->buf, (char **)NULL, 16);
                con->http_chunk->off = 0;
                con->http_chunk->finish = 1;
                xa_debug (4, "DEBUG: add_chunk: http chunk left=%d on mountpoint [%s]", con->http_chunk->left, con->food.source->audiocast.mount);
              }
            }
            else if (len > 0)
            {
              con->http_chunk->off += len;
              len = -1;
              break;
            }
            else
              break;
          }

          if (con->http_chunk->left < SOURCE_READSIZE)
          {
#ifdef HAVE_TLS
            if(con->tls_socket)
              len = tls_recv(con->tls_socket, con->food.source->chunk[con->food.source->cid].data + read_bytes, con->http_chunk->left - read_bytes);
            else
#endif
            len = recv(con->sock, con->food.source->chunk[con->food.source->cid].data + read_bytes, con->http_chunk->left - read_bytes, 0);
            maxread = con->http_chunk->left;
          }
          else
          {
#ifdef HAVE_TLS
            if(con->tls_socket)
              len = tls_recv(con->tls_socket, con->food.source->chunk[con->food.source->cid].data + read_bytes, SOURCE_READSIZE - read_bytes);
            else
#endif
            len = recv(con->sock, con->food.source->chunk[con->food.source->cid].data + read_bytes, SOURCE_READSIZE - read_bytes, 0);
          }
          break;
        }
        else
        {
#ifdef HAVE_TLS
          if(con->tls_socket)
            len = tls_recv(con->tls_socket, con->food.source->chunk[con->food.source->cid].data + read_bytes, SOURCE_READSIZE - read_bytes);
          else
#endif
          len = recv(con->sock, con->food.source->chunk[con->food.source->cid].data + read_bytes, SOURCE_READSIZE - read_bytes, 0);
          xa_debug (5, "DEBUG: Source receive no-chunk %d bytes (want %d have %d) on mountpoint [%s]", len, SOURCE_READSIZE - read_bytes, read_bytes, con->food.source->audiocast.mount);
          break;
        }
      }
      case rtp_e:
      {
        len = rtp_recieve_datagram_buffered(con);
        if (len > 0)
        {
          source_fill_chunks(con->food.source, con->rtp->datagram->data, len);
          maxread = 0;
        }
        break;
      }
      case udp_e:
      {
        time_t ct = time(0);
        thread_mutex_lock(&con->udpbuffers->buffer_mutex);

        if (con->udpbuffers && ct-con->udpbuffers->lastsend > 20)
        {
          sock_write_string_con(con, "");
          con->udpbuffers->lastsend = ct;
        }
        if(con->udpbuffers->len)
        {
          len = SOURCE_READSIZE - read_bytes;
          if(len > con->udpbuffers->len)
            len = con->udpbuffers->len;
          memcpy(con->food.source->chunk[con->food.source->cid].data + read_bytes,
          con->udpbuffers->buffer, len);
          con->udpbuffers->len -= len;
          if(con->udpbuffers->len)
          {
            memmove(con->udpbuffers->buffer,
            con->udpbuffers->buffer+len,con->udpbuffers->len);
          }
        }
        thread_mutex_unlock(&con->udpbuffers->buffer_mutex);
        break;
      }
      default:
      {
        len = recv(con->sock, con->food.source->chunk[con->food.source->cid].data + read_bytes, SOURCE_READSIZE - read_bytes, 0);
      }
    }

    xa_debug (5, "DEBUG: Source received %d bytes on mountpoint [%s] in try %d, total %d, errno: %d (%s)", len, con->food.source->audiocast.mount, tries, read_bytes, errno, strerror(errno));

#ifdef _WIN32
    if(con->sock > 0)
      sock_set_blocking(con->sock, SOCK_BLOCKNOT);
#endif

    if (con->food.source->connected == SOURCE_KILLED)
      return;

    if (len > 0)
    {
      stat_add_read(&con->food.source->stats, len);
      stat_add_read(con->food.source->globalstats, len);

      internal_lock_mutex (&info.misc_mutex);
      info.hourly_stats.read_bytes += len;
      internal_unlock_mutex (&info.misc_mutex);

      read_bytes += len;

      if (read_bytes > SOURCE_READSIZE) { // rtsp
        read_bytes = read_bytes % SOURCE_READSIZE;
        if (read_bytes == 0) read_bytes = SOURCE_READSIZE;
      }
      len = 0;
    }
    else if (len == 0 || (read_bytes && tries))
    {
      break;
    }
    else
    {
      my_sleep(read_bytes ? source_read_sleep/10 : source_read_sleep);
    }

    tries++;
  } while ((read_bytes < maxread) && (tries < source_read_tries));

  if (read_bytes == 0)
  {
    write_log(LOG_DEFAULT, "Didn't receive data from source after %d milliseconds, assuming it died...", tries * READ_RETRY_DELAY);

    thread_mutex_lock (&info.double_mutex);
    kick_connection(con, "Source died");
    thread_mutex_unlock (&info.double_mutex);
    return;
  }

#ifndef NTRIP_NUMBER
  xa_debug (4, "DEBUG: add_chunk: Chunk %d was [%d] bytes on mountpoint [%s]", con->food.source->cid, read_bytes, con->food.source->audiocast.mount );
#endif

  con->food.source->chunk[con->food.source->cid].len = read_bytes;
  con->food.source->chunk[con->food.source->cid].clients_left = con->food.source->num_clients;
  con->food.source->cid = (con->food.source->cid + 1) % CHUNKLEN;

  if (con->trans_encoding == chunked_e)
  {
    con->http_chunk->left = con->http_chunk->left - read_bytes;
    xa_debug (4, "DEBUG: add_chunk: http_chunk left now=%d ", con->http_chunk->left );
  }
}

void
write_chunk(source_t *source, connection_t *clicon)
{
  int i = 0;
  long int write_bytes = 0, len = 0;
  char *buff;

  if (clicon->food.client->alive == CLIENT_DEAD) return; // rtsp

  /* Try to write 2 times */
  for (i = 0; i < 2; i++)
  {
    if (source->cid == clicon->food.client->cid) return;

    /* This is how much we should be writing to the client */
    len = source->chunk[clicon->food.client->cid].len - clicon->food.client->offset;

    xa_debug (5, "DEBUG: write_chunk(): Try: %d, writing chunk %d to client %d on mountpoint [%s], len(%d) - offset(%d) == %d", i, clicon->food.client->cid, clicon->id, source->audiocast.mount, source->chunk[clicon->food.client->cid].len, clicon->food.client->offset, len);

    if (len < 0 || source->chunk[clicon->food.client->cid].len == 0)
    {
#ifndef NTRIP_NUMBER
      xa_debug (5, "DEBUG: write_chunk: Empty chunk [%d] [%d]", source->chunk[clicon->food.client->cid].len,
          clicon->food.client->offset );
#endif
      source->chunk[clicon->food.client->cid].clients_left--;
      clicon->food.client->cid = (clicon->food.client->cid + 1) % CHUNKLEN;
      clicon->food.client->offset = 0;
      continue; /* Perhaps for some reason the source read a zero sized chunk but the next one is ok.. */
    }

    buff = &source->chunk[clicon->food.client->cid].data[clicon->food.client->offset];

    switch (clicon->data_protocol)
    {
      case tcp_e:
      {
        if (clicon->trans_encoding == chunked_e)
        {
          if (clicon->http_chunk->left <= 0)
          {
            snprintf(clicon->http_chunk->buf, 20, "%X\r\n", source->chunk[clicon->food.client->cid].len);

            write_bytes = sock_write_bytes_con(clicon, clicon->http_chunk->buf, strlen(clicon->http_chunk->buf)); // rtsp

            if (write_bytes > 0)
              clicon->http_chunk->left = source->chunk[clicon->food.client->cid].len;
            else break;
          }
          write_bytes = sock_write_bytes_con(clicon, buff, len);
          break;
        }
        else
        {
          write_bytes = sock_write_bytes_con(clicon, buff, len);
          break;
        }
      }
      case rtp_e:
      {
        rtp_prepare_send(clicon->rtp);

        memcpy(clicon->rtp->datagram->data, buff, len);

        write_bytes = sock_write_bytes_con(clicon, (void *)clicon->rtp->datagram, 12+len) - 12;

        break;
      }
      default:
      {
        write_bytes = sock_write_bytes_con(clicon, buff, len);
      }
    }

#ifndef NTRIP_NUMBER
    xa_debug (4, "DEBUG: client %d in write_chunk() on mountpoint [%s]. %d of %d bytes written, client on chunk %d (+%d), source on chunk %d", clicon->id, source->audiocast.mount, write_bytes, source->chunk[clicon->food.client->cid].len - clicon->food.client->offset, clicon->food.client->cid, clicon->food.client->offset, source->cid);
#endif

    if (clicon->udpbuffers && time(0)-clicon->udpbuffers->lastactive > 60)
    {
      kick_connection(clicon, "UDP connection timeout");
      break;
    }
    else if (write_bytes < 0)
    {
#ifndef NTRIP_NUMBER
      xa_debug (5, "DEBUG: client: [%2d] errors: [%3d]", clicon->id, client_errors (clicon->food.client));
#endif
      kick_connection(clicon, "Broken connection");
      break;
    }
    else if (write_bytes > 0)
    {
      clicon->food.client->write_bytes += write_bytes;
      stat_add_write (&source->stats, write_bytes);
      stat_add_write (source->globalstats, write_bytes);

      internal_lock_mutex (&info.misc_mutex);
      info.hourly_stats.write_bytes += write_bytes;
      internal_unlock_mutex (&info.misc_mutex);

      if (write_bytes + clicon->food.client->offset >= source->chunk[clicon->food.client->cid].len)
      {
        source->chunk[clicon->food.client->cid].clients_left--;
        clicon->food.client->cid = (clicon->food.client->cid + 1) % CHUNKLEN;
        clicon->food.client->offset = 0;

        if (clicon->trans_encoding == chunked_e)
        {
          sock_write_bytes_con(clicon, "\r\n", 2);
          clicon->http_chunk->left = 0;
        }
      }
      else
      {
        clicon->food.client->offset += write_bytes;
#ifndef NTRIP_NUMBER
        xa_debug (5, "DEBUG: client %d only read %d of %d bytes", clicon->id, write_bytes, source->chunk[clicon->food.client->cid].len - clicon->food.client->offset);
#endif
      }
    }
  }

  clicon->food.client->errors = client_errors(clicon->food.client);

  xa_debug (4, "DEBUG: client %d tried %d times, now has %d errors %d chunks behind source", clicon->id, i, clicon->food.client->errors, source->cid < clicon->food.client->cid ? source->cid+CHUNKLEN - clicon->food.client->cid : source->cid - clicon->food.client->cid);
}

void kick_trailing_clients(source_t *source) {
#ifndef NTRIP_NUMBER
  xa_debug (2, "DEBUG: Kicking trailing clients [%d] on id %d", source->chunk[source->cid].clients_left, source->cid);
#endif
  kick_clients_on_cid (source);
}

void kick_clients_on_cid(source_t *source) {
  avl_traverser trav = {0};
  connection_t *clicon;

  unsigned int max = avl_count (source->clients) * avl_count (source->clients) + 2;

  xa_debug (3, "Clearing cid %d", source->cid);

  zero_trav (&trav);

#if !defined(NTRIP_FIGURE) || !defined(NTRIP_NUMBER)
  xa_debug (5, "DEBUG: In function kick_clients_on_cid. Source has %d clients", max);
#endif
  while (max >= 0) {
    clicon = avl_traverse (source->clients, &trav);
    if (!clicon) break;

    if (clicon->food.client->errors >= (CHUNKLEN - 1) && clicon->food.client->alive != CLIENT_DEAD) {
      kick_connection (clicon, "Too many errors (client not receiving data fast enough)");
    }
    max--;
  }
  source->chunk[source->cid].clients_left = 0;
#if !defined(NTRIP_FIGURE) || !defined(NTRIP_NUMBER)
  xa_debug (5, "DEBUG: leaving function kick_clients_on_cid");
#endif
}

/*
 * Can't be removing clients inside the loop which handles all the
 * write_chunk()s, instead we kick all the dead ones for each chunk.
 */
void
kick_dead_clients(source_t *source)
{
  avl_traverser trav = {0};
  connection_t *clicon = NULL;

  int max = avl_count (source->clients) * avl_count (source->clients) + 2;

#if !defined(NTRIP_FIGURE) || !defined(NTRIP_NUMBER)
  xa_debug (5, "DEBUG: In function kick_dead_clients. Will run %d laps", max);
#endif
  while (max >= 0) {
    clicon = avl_traverse (source->clients, &trav);
    if (!clicon) break;

    if (clicon->food.client->alive == CLIENT_DEAD) {
      close_connection (clicon);
      zero_trav (&trav);
    }

    max--;
  }

#if !defined(NTRIP_FIGURE) || !defined(NTRIP_NUMBER)
  xa_debug (5, "DEBUG: leaving function kick_dead_clients, %d laps left", max);
#endif
}

connection_t *
get_source_with_mount (const char *mount)
{
  avl_traverser trav = {0};
  connection_t *travcon;
  char* alias;

  thread_mutex_lock (&info.source_mutex);

  while ((travcon = avl_traverse (info.sources, &trav)))
  {
    if (! (alias = strchr (travcon->food.source->audiocast.mount, '/')))
      alias = travcon->food.source->audiocast.mount;
    if ((ntripcaster_strcmp (alias, mount) == 0) ||
        ((mount[0] != '/') && (ntripcaster_strcmp (&mount[0], &alias[1]) == 0)))
    {
      thread_mutex_unlock (&info.source_mutex);
      return travcon;
    }
  }

  thread_mutex_unlock (&info.source_mutex);
  return NULL;
}

connection_t *
get_source_from_host (connection_t *con)
{
  avl_traverser trav = {0};
  connection_t *travcon;

  thread_mutex_lock (&info.source_mutex);

  while ((travcon = avl_traverse (info.sources, &trav)))
  {
    if (hosteq (con, travcon))
    {
      thread_mutex_unlock (&info.source_mutex);
      return travcon;
    }
  }

  thread_mutex_unlock (&info.source_mutex);

  return NULL;
}

void
describe_source (const com_request_t *req, const connection_t *sourcecon)
{
  const source_t *source;
  char buf[BUFSIZE];

  if (!req || !sourcecon)
  {
    xa_debug (1, "WARNING: describe_source(): called with NULL pointers");
    return;
  }

  if (sourcecon->type != source_e)
  {
    xa_debug (1, "WARNING: describe_source(): called with invalid type");
    return;
  }

  describe_connection (req, sourcecon);

  source = sourcecon->food.source;

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_START, "Misc source info:");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Connected: %s", source->connected ? "yes" : "no");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Source Type: %s", source_type(sourcecon));
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Number of clients: %lu", source->num_clients);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Stream priority: %d", source->priority);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Source mount: %s", source->audiocast.mount ? source->audiocast.mount : "(null)");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "KBytes read: %lu", source->stats.read_kilos);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "KBytes written: %lu", source->stats.write_kilos);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Client connections: %lu", source->stats.client_connections);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Client connect time: %s", nntripcaster_time_minutes (source->stats.client_connect_time, buf));
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Average client connect time: %s", connect_average (source->stats.client_connect_time, source->stats.client_connections, buf));
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_MISC, "Average client transfer: %lu", transfer_average (source->stats.write_kilos, source->stats.client_connections));

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_SOURCE_END, "End of source info");
}

const char source_types[5][20] = { "unknown source", "http source", "rtsp client source", "relay", "nontrip source" }; // rtsp. ajd

const char *source_type(const connection_t *con) {
  return source_types[con->food.source->type+1];
}

/* What we want to do here is give the client the best possible
 * chunk in the source to start from. Where he suffers the least
 * from both his own slow network connection, and discrepanices
 * in the source feed. I used to set this to the chunk where the
 * source was, minus one, but I think it's better to find a place
 * in the middle somewhere. I leave this for later testing though.
 */
int
start_chunk (source_t *source)
{
  return source->cid > 0 ? source->cid - 1 : CHUNKLEN - 1;
}

void
source_write_to_client (source_t *source, connection_t *clicon)
{
  client_t *client;

  if (!clicon || !source) {
    xa_debug (1, "WARNING: source_write_to_client() called with NULL pointers");
    return;
  }

  client = clicon->food.client;

  if (client->alive == CLIENT_DEAD) return;

  if (client->virgin == -1) return; // rtsp

  if (client->alive == CLIENT_PAUSED) { // rtsp
    source->chunk[clicon->food.client->cid].clients_left--;
    clicon->food.client->cid = (clicon->food.client->cid + 1) % CHUNKLEN;
    clicon->food.client->offset = 0;
    return;
  }

  if (client->virgin == 1) {
    client->cid = start_chunk (source);
    client->offset = 0; // rtsp. ajd
    client->virgin = 0;
    thread_mutex_lock(&info.source_mutex);
    source->num_clients++;
    thread_mutex_unlock(&info.source_mutex);
  }

  if (client->alive == CLIENT_UNPAUSED) {
    client->cid = start_chunk (source);
    client->offset = 0;
    client->virgin = 0;

    if (clicon->trans_encoding == chunked_e) { // rtsp
      clicon->http_chunk->left = 0;
      clicon->http_chunk->off = 0;
    }

    client->alive = CLIENT_ALIVE;
  }

  write_chunk (source, clicon);
}

void
source_get_new_clients (source_t *source)
{
  connection_t *clicon;

  while ((clicon = pool_get_my_clients (source)))
  {
    xa_debug (1, "DEBUG: source_get_new_clients(): Accepted client %d", clicon->id);
    avl_insert (source->clients, clicon);

    source->stats.client_connections++;
    source->globalstats->client_connections++;
  }
}

int
source_get_id (char *argument)
{
  connection_t *con;

  if (!argument || !argument[0])
    return -1;

  if (isdigit((int)argument[0]) && strchr (argument, '.') == NULL && strchr (argument, '/') == NULL)
    return atoi (argument);

  con = find_source_with_mount (argument);

  if (con)
    return con->id;

  return -1;
}

void add_nontrip_source(char *line) {
  char mount[BUFSIZE];
  char tmp[BUFSIZE];
  nontripsource_t *nsource, *old;

  if (splitc(mount, line, ':') == NULL) return;
  if (atoi(line) < 1024) return;

  nsource = (nontripsource_t *) nmalloc (sizeof (nontripsource_t));

  if (mount[0] != '/') {
    if(snprintf(tmp, BUFSIZE, "/%s", mount) >= BUFSIZE)
      tmp[BUFSIZE-1] = 0;
    nsource->mount = my_strdup(tmp);
  } else {
    nsource->mount = my_strdup(mount);
  }
  nsource->port = atoi(line);

  thread_mutex_lock (&info.misc_mutex);

  old = avl_replace(info.nontripsources, nsource);

  thread_mutex_unlock (&info.misc_mutex);

  if (old != NULL) {
    nfree(old->mount);
    nfree(old);
  }
}

int source_fill_chunks(source_t *source, const char *buf, int len) {
  int p = 0;

  while (len > SOURCE_READSIZE) {
    if (source->chunk[source->cid].clients_left > 0) kick_trailing_clients(source);
    memcpy(source->chunk[source->cid].data, buf + p, SOURCE_READSIZE);
    source->chunk[source->cid].len = SOURCE_READSIZE;
    source->chunk[source->cid].clients_left = source->num_clients;
    source->cid = (source->cid + 1) % CHUNKLEN;
    len -= SOURCE_READSIZE;
    p += SOURCE_READSIZE;
  }

  memcpy(source->chunk[source->cid].data, buf + p, len);

  return len;
}
