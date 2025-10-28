/* avl_functions.c
 * - Functions to compare avl nodes and stuff
 *
 * Copyright (c) 2022
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
#ifndef __USE_BSD
#define __USE_BSD
#endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "connection.h"
#include "log.h"
#include "avl_functions.h"
#include "memory.h"
#include "authenticate/basic.h"
#include "item.h"
#include "sock.h"
#include "ntrip.h"
#include "sourcetable.h"

int compare_groups (const void *first, const void *second, void *param)
{
  group_t *v1 = (group_t *) first, *v2 = (group_t *) second;

  if (!first || !second || !v1->name || !v2->name)
  {
    xa_debug (2, "WARNING: compare_groups called with NULL pointers!");
    return 0;
  }

  return (ntripcaster_strcmp (v1->name, v2->name));
}

int compare_users (const void *first, const void *second, void *param)
{
  ntripcaster_user_t *v1 = (ntripcaster_user_t *) first, *v2 = (ntripcaster_user_t *) second;

  if (!first || !second || !v1->name || !v2->name)
  {
    xa_debug (2, "WARNING: compare_users called with NULL pointers!");
    return 0;
  }

  return (ntripcaster_strcmp (v1->name, v2->name));
}

int compare_mounts (const void *first, const void *second, void *param)
{
  mount_t *v1 = (mount_t *) first, *v2 = (mount_t *) second;

  if (!first || !second || !v1->name || !v2->name)
  {
    xa_debug (2, "WARNING: compare_mounts called with NULL pointers!");
    return 0;
  }

  return (ntripcaster_strcmp (v1->name, v2->name));
}

int compare_vars (const void *first, const void *second, void *param)
{
  varpair_t *v1 = (varpair_t *) first, *v2 = (varpair_t *) second;

  if (!first || !second || !v1->name || !v2->name)
  {
    xa_debug (2, "WARNING: compare_vars called with NULL pointers!");
    return 0;
  }

  return (ntripcaster_strcmp (v1->name, v2->name));
}

int compare_header_vars (const void *first, const void *second, void *param)
{
  varpair_t *v1 = (varpair_t *) first, *v2 = (varpair_t *) second;

  if (!first || !second || !v1->name || !v2->name)
  {
    xa_debug (2, "WARNING: compare_header_vars called with NULL pointers!");
    return 0;
  }

  return (ntripcaster_strcasecmp (v1->name, v2->name));
}

#ifdef DEBUG_MEMORY
int compare_mem (const void *firstp, const void *secondp, void *param)
{
  meminfo_t *first = (meminfo_t *) firstp, *second = (meminfo_t *) secondp;

  if (!first || !second)
  {
    fprintf (stderr, "WARNING, compare_mem called with NULL pointers!");
    return 0;
  }

        if ((unsigned long int) (first->ptr) > (unsigned long int) (second->ptr))
    return 1;
  else if ((unsigned long int) (first->ptr) < (unsigned long int) (second->ptr))
    return -1;
  return 0;
}
#endif

int compare_relays (const void *first, const void *second, void *param)
{
  relay_t *r1 = (relay_t *) first, *r2 = (relay_t *) second;
  char cfirst[BUFSIZE], csecond[BUFSIZE];

  if (!r1 || !r2) {
    write_log (LOG_DEFAULT, "WARNING: compare_relays() called with NULL pointers!");
  }

  if ((r1->localmount == NULL) || (r2->localmount == NULL)) {
    if(snprintf (cfirst, BUFSIZE, "%s:%d%s", r1->req.host, r1->req.port, r1->req.path) >= BUFSIZE)
      cfirst[BUFSIZE-1] = 0;
    if(snprintf (csecond, BUFSIZE, "%s:%d%s", r2->req.host, r2->req.port, r2->req.path) >= BUFSIZE)
      csecond[BUFSIZE-1] = 0;
  } else {
    if(snprintf (cfirst, BUFSIZE, "%s:%d%s%s", r1->req.host, r1->req.port, r1->req.path, r1->localmount) >= BUFSIZE)
      cfirst[BUFSIZE-1] = 0;
    if(snprintf (csecond, BUFSIZE, "%s:%d%s%s", r2->req.host, r2->req.port, r2->req.path, r2->localmount) >= BUFSIZE)
      csecond[BUFSIZE-1] = 0;
  }

  return ntripcaster_strcmp (cfirst, csecond);
}

int compare_sourcetable_entrys (const void *first, const void *second, void *param)
{
  sourcetable_entry_t *ste1 = (sourcetable_entry_t *) first,
                      *ste2 = (sourcetable_entry_t *) second;

  if (!first || !second) {
    write_log (LOG_DEFAULT, "WARNING: compare_sourcetable_entrys() called with NULL pointers!");
    return 0;
  }
  if(ste1->type != ste2->type)
    return ste1->type-ste2->type;

  return ntripcaster_strcmp (ste1->id, ste2->id);
}

int compare_sessions (const void *first, const void *second, void *param)
{
  rtsp_session_t *s1, *s2;
  s1 = (rtsp_session_t *)first;
  s2 = (rtsp_session_t *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_sessions called with NULL pointers");
    return 0;
  }

  if (s1->id > s2->id) return 1;
  if (s1->id < s2->id) return -1;
  return 0;
}

int compare_header_elements (const void *first, const void *second, void *param)
{
  ntrip_header_element_t *h1 = (ntrip_header_element_t *) first, *h2 = (ntrip_header_element_t *) second;

  if (!first || !second)
  {
    xa_debug (2, "WARNING: compare_header_elements called with NULL pointers!");
    return 0;
  }

  if (h1->index > h2->index) return 1;
  if (h1->index < h2->index) return -1;
  return 0;
}

int compare_messages (const void *first, const void *second, void *param)
{
  ntrip_message_t *m1 = (ntrip_message_t *) first, *m2 = (ntrip_message_t *) second;

  if (!first || !second)
  {
    xa_debug (2, "WARNING: compare_messages called with NULL pointers!");
    return 0;
  }

  if (m1->type > m2->type) return 1;
  if (m1->type < m2->type) return -1;
  return 0;
}

int compare_nontrip_sources (const void *first, const void *second, void *param)
{
  nontripsource_t *n1 = (nontripsource_t *)first;
  nontripsource_t *n2 = (nontripsource_t *)second;
  int res;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_nontrip_sources called with null pointers");
    return 0;
  }

  xa_debug (4, "DEBUG: Comparing nontrip sources [%s] with [%s]", n1->mount, n2->mount);

  res = ntripcaster_strcasecmp (n1->mount, n2->mount);

  if ((res == 0) || (n1->port == n2->port))
    return 0;
  else
    return res;
}

int compare_strings (const void *first, const void *second, void *param)
{
  char *a1 = (char *)first, *a2 = (char *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_strings called with null pointers");
    return 0;
  }

  xa_debug (4, "DEBUG: Comparing [%s] with [%s]", a1, a2);
  return (ntripcaster_strcasecmp (a1, a2));
}

int compare_aliases (const void *first, const void *second, void *param)
{
  alias_t *a1 = (alias_t *) first, *a2 = (alias_t *) second;
  char full[BUFSIZE], full2[BUFSIZE];

  if (!a1 || !a2 || !a1->name || !a2->name)
  {
    write_log (LOG_DEFAULT, "WARNING: NULL pointers in comparison");
    return -1;
  }

  if(snprintf (full, BUFSIZE, "%s:%d%s", a1->name->host, a1->name->port, a1->name->path) >= BUFSIZE)
    full[BUFSIZE-1] = 0;
  if(snprintf (full2, BUFSIZE, "%s:%d%s", a2->name->host, a2->name->port, a2->name->path) >= BUFSIZE)
    full2[BUFSIZE-1] = 0;

  return ntripcaster_strcmp (full, full2);
}

int compare_restricts (const void *first, const void *second, void *param)
{
  restrict_t *t1 = (restrict_t *)first, *t2 = (restrict_t *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_restricts called with NULL pointers");
    return 0;
  }

  return ntripcaster_strncmp(t1->mask, t2->mask, BUFSIZE);
}

int compare_statisticsentry (const void *first, const void *second, void *param)
{
  statisticsentry_t *t1 = (statisticsentry_t *)first, *t2 = (statisticsentry_t *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_statisticsentry called with NULL pointers");
    return 0;
  }

  return ntripcaster_strcmp(t1->mount, t2->mount);
}

int compare_threads (const void *first, const void *second, void *param)
{
  mythread_t *t1, *t2;
  t1 = (mythread_t *)first;
  t2 = (mythread_t *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_threads called with NULL pointers");
    return 0;
  }

  if (t1->id > t2->id)
    return 1;
  if (t1->id < t2->id)
    return -1;
  return 0;
}

int compare_mutexes (const void *first, const void *second, void *param)
{
  mutex_t *t1, *t2;
  t1 = (mutex_t *)first;
  t2 = (mutex_t *)second;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING: compare_mutex called with NULL pointers");
    return 0;
  }

  if (t1->mutexid > t2->mutexid)
    return 1;
  if (t1->mutexid < t2->mutexid)
    return -1;
  return 0;
}

int compare_connection(const void *first, const void *second, void *param)
{
  connection_t *a1, *a2;

  if (!first || !second)
  {
    write_log (LOG_DEFAULT, "WARNING!!! - Null pointer connection!");
    return -1;
  }

  a1 = (connection_t *)first;
  a2 = (connection_t *)second;

  if (a1->type != a2->type)
  {
    write_log (LOG_DEFAULT, "WARNING!!!! - Comparing different type connections");
    return -1;
  }

  if (a1->id > a2->id)
    return 1;
  else if (a1->id < a2->id)
    return -1;
  else
    return 0;
}

void zero_trav(avl_traverser *trav)
{
  if (!trav)
  {
    write_log (LOG_DEFAULT, "WARNING: zero_trav called with NULL trav");
    return;
  }
  trav->init = 0;
  trav->nstack = 0;
  trav->p = NULL;
}

int compare_sockets (const void *first, const void *second, void *param)
{
  ntripcaster_socket_t *is1 = (ntripcaster_socket_t *) first, *is2 = (ntripcaster_socket_t *) second;

  if (is1 == NULL || is2 == NULL) {
    fprintf (stderr, "WARNING: compare_sockets called with NULL values");
    return -1;
  } else if (is1->sock < 0 || is2->sock < 0) {
    fprintf (stderr, "WARNING: compare_sockets called with negative socket number");
    return -1;
  }

  if (is1->sock > is2->sock)
    return 1;
  else if (is1->sock < is2->sock)
    return -1;
  return 0;
}

void * avl_get_any_node (avl_tree *tree)
{
  avl_traverser trav = {0};
  if (!tree)
  {
    write_log (LOG_DEFAULT, "WARNING: avl_get_any_node called with NULL tree");
    return NULL;
  }

  if (avl_count (tree) <= 0)
    return NULL;

  return (avl_traverse (tree, &trav));
}
