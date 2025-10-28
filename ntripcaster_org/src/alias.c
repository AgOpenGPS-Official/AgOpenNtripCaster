/* alias.c
 * - Functions to play with the aliases
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
#include "win32config.h"
#else
#include "config.h"
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
#include "sock.h"
#include "admin.h"
#include "avl_functions.h"
#include "alias.h"
#include "memory.h"
#include "commands.h"

extern server_info_t info;

/* Aliases looks like this:
   <name> <realname>
   Where, if name is an adress like http://apan.com:8000/apan, then it is used for virtual hosts
   And if realname is an adress like http://apan.com:8000/apan, this it is used for on demand relaying. */

void
list_aliases (com_request_t *req)
{
  avl_traverser trav = {0};
  alias_t *alias;

  if (!req)
  {
    write_log (LOG_DEFAULT, "WARNING: list_aliases called with NULL pointer");
    return;
  }

  admin_write_line (req, ADMIN_SHOW_ALIAS_START, "Listing aliases");

  thread_mutex_lock (&info.alias_mutex);

  while ((alias = avl_traverse (info.aliases, &trav)))
    admin_write_line (req, ADMIN_SHOW_ALIAS_ENTRY, "\t[%s:%d%s]\t[%s:%d%s]", alias->name->host, alias->name->port, alias->name->path,
          alias->real->host, alias->real->port, alias->real->path);

  thread_mutex_unlock (&info.alias_mutex);

  admin_write_line (req, ADMIN_SHOW_ALIAS_END, "End of alias listing (%d listed)", avl_count (info.aliases));
}

alias_t *
create_alias () {
  alias_t *res = (alias_t *) nmalloc (sizeof (alias_t));
  res->real = res->name = NULL;
 // added. ajd
  res->userID[0] = '\0';
  res->localmount[0] = '\0';
  res->pending = 0;

  return res;
}

ntrip_request_t *
create_request () {
  ntrip_request_t *req = (ntrip_request_t *) nmalloc (sizeof (ntrip_request_t));
  req->port = -1;
  req->host[0] = '\0';
  req->path[0] = '\0';
  return req;
}

alias_t *
add_alias (ntrip_request_t *name, ntrip_request_t *real, char *userID, char *localmount) {
  alias_t *res, *out;

  if (!name || !real)
    return NULL;

  xa_debug (1, "DEBUG: adding alias [%s:%d%s] for [%s:%d%s]", name->host, name->port, name->path,
       real->host, real->port, real->path);

  res = create_alias ();
  res->name = create_request ();
  res->real = create_request ();

  strcpy (res->name->host, name->host);
  strcpy (res->name->path, name->path);
  res->name->port = name->port;

  strcpy (res->real->host, real->host);
  strcpy (res->real->path, real->path);
  res->real->port = real->port;
// added. ajd
  if (userID != NULL) strcpy (res->userID, userID);
  if (localmount != NULL) strcpy (res->localmount, localmount);

  thread_mutex_lock (&info.alias_mutex);

  out = avl_replace (info.aliases, res);

  if (out)
  {
    nfree (out->name);
    out->name = NULL;
    nfree (out->real);
    out->real = NULL;
    nfree (out);
    out = NULL;
  }

  thread_mutex_unlock (&info.alias_mutex);

  return res;
}

ntrip_request_t *
get_alias (ntrip_request_t *req)
{
  avl_traverser trav = {0};
  alias_t *res = NULL, search;

  if (!req)
  {
    write_log (LOG_DEFAULT, "ERROR: get_alias called with NULL source");
    return NULL;
  }

  thread_mutex_lock (&info.alias_mutex);

  while ((res = avl_traverse (info.aliases, &trav))) {
    if (hostname_local (res->name->host) && hostname_local (req->host)) {
      if (ntripcaster_strcmp (res->name->path, req->path) == 0)
          break;
    }
  }

  if (!res) {
    search.name = req;

    res = avl_find (info.aliases, &search);
  }

  thread_mutex_unlock (&info.alias_mutex);

  return res ? res->real : NULL;
}

alias_t *
get_alias_whole (ntrip_request_t *req)
{
  avl_traverser trav = {0};
  alias_t *res = NULL, search;

  if (!req)
  {
    write_log (LOG_DEFAULT, "ERROR: get_alias_whole called with NULL source");
    return NULL;
  }

  thread_mutex_lock (&info.alias_mutex);

  while ((res = avl_traverse (info.aliases, &trav))) {
    if (hostname_local (res->name->host) && hostname_local (req->host)) {
      if (ntripcaster_strcmp (res->name->path, req->path) == 0)
        break;
    }
  }

  if (!res) {
    search.name = req;

    res = avl_find (info.aliases, &search);
  }

  thread_mutex_unlock (&info.alias_mutex);

  return res ? res : NULL;
}

alias_t *
get_alias_mount (const char *mount)
{
  avl_traverser trav = {0};
  alias_t *res = NULL;

  if (!mount)
  {
    write_log (LOG_DEFAULT, "ERROR: get_alias_mount called with NULL source");
    return NULL;
  }

  thread_mutex_lock (&info.alias_mutex);

  while ((res = avl_traverse (info.aliases, &trav))) {
    if (ntripcaster_strcmp (res->name->path, mount) == 0)
      break;
  }

  thread_mutex_unlock (&info.alias_mutex);

  return res ? res : NULL;
}

int
del_alias (char *name)
{
  avl_traverser trav = {0};
  alias_t *res;
  char full[BUFSIZE];

  thread_mutex_lock (&info.alias_mutex);

  while ((res = avl_traverse (info.aliases, &trav)))
  {
    if(snprintf(full, BUFSIZE, "%s:%d%s", res->name->host, res->name->port, res->name->path) >= BUFSIZE)
      full[BUFSIZE-1] = 0;
    if (ntripcaster_strcmp (full, name) == 0)
    {
      remove_alias (res);
      thread_mutex_unlock (&info.alias_mutex);
      return 1;
    }
  }

  zero_trav (&trav);

  while ((res = avl_traverse (info.aliases, &trav)))
  {
    if (ntripcaster_strcmp (res->name->path, name) == 0)
    {
      remove_alias (res);
      thread_mutex_unlock (&info.alias_mutex);
      return 1;
    }
  }

  thread_mutex_unlock (&info.alias_mutex);

  return 0;
}

void
remove_alias (alias_t *al)
{
  alias_t *out = avl_delete (info.aliases, al);

  if (out)
  {
    nfree (out->real);
    nfree (out->name);
    nfree (out);
  }
}

void
free_aliases ()
{
  alias_t *alias, *out;

  if (!info.aliases) {
    write_log (LOG_DEFAULT, "WARNING: info.aliases is NULL, weird!");
    return;
  }

  thread_mutex_lock (&info.alias_mutex);

  while ((alias = avl_get_any_node (info.aliases)))
  {
    if (!alias) {
      write_log (LOG_DEFAULT, "WARNING: avl_get_any_node() returned NULL alias");
      continue;
    }

    out = avl_delete (info.aliases, alias);

    if (out)
    {
      nfree (out->real);
      nfree (out->name);
      nfree (out);
    }
  }

  thread_mutex_unlock (&info.alias_mutex);
}

/* added. ajd*/
ntrip_request_t *
get_alias_with_mount(char *mount) {

  avl_traverser trav = {0};
  alias_t *res = NULL;
  int found = 0;

  if (!mount)
  {
    write_log (LOG_DEFAULT, "ERROR: get_alias_with_mount called with NULL pointer");
    return NULL;
  }

  thread_mutex_lock (&info.alias_mutex);

  while ((res = avl_traverse (info.aliases, &trav))) {
      if (ntripcaster_strcmp (res->name->path, mount) == 0) {
          found = 1;
          break;
      }
  }

  thread_mutex_unlock (&info.alias_mutex);

  if (found > 0)
    return res->real;
  else
    return NULL;

}


