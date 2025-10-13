/* restrict.c
 * - Functions to play with the acl lists
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
#ifndef __USE_BSD
#define __USE_BSD
#endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
#include "restrict.h"
#include "sourcetable.h"
#include "match.h"
#include "memory.h"
#include "commands.h"

extern server_info_t info;

void
list_rule (com_request_t *req, restrict_t *res)
{
  admin_write_line (req, ADMIN_SHOW_RESTRICT_ENTRY, "%d\t%s\t[%s] (%d)", res->id, res->type ? "allow": "deny",  res->mask, res->num_short_connections);
}

int
list_restrict (com_request_t *req, avl_tree *tree, acltype_t type)
{
  avl_traverser trav = {0};
  restrict_t *res;
  int listed = 0;

  thread_mutex_lock (&info.acl_mutex);

  while ((res = avl_traverse (tree, &trav)))
  {
    if ((type == all) || (res->type == type))
    {
      list_rule (req, res);
      listed++;
    }
  }

  thread_mutex_unlock (&info.acl_mutex);

  return listed;
}

void
list_acl_control (com_request_t *req, avl_tree *acl, acltype_t type)
{
  int listed = 0;

  switch (type) {
    case deny:
      admin_write (req, ADMIN_SHOW_RESTRICT_START_DENY, "Listing deny rules");
      break;
    case allow:
      admin_write (req, ADMIN_SHOW_RESTRICT_START_ALLOW, "Listing allow rules");
      break;
    case all:
    default:
      admin_write (req, ADMIN_SHOW_RESTRICT_START_ALL, "Listing all control rules (%d)", avl_count(acl));
  }

  listed = list_restrict (req, acl, type);

  admin_write_line (req, ADMIN_SHOW_RESTRICT_END, "End of restriction listing (%d listed)", listed);
}

void
list_all_acls (com_request_t *req)
{
  admin_write_line (req, ADMIN_SHOW_RESTRICT_START_CON_ALL, "Rules affecting all connections:");
  list_acl_control (req, info.all_acl, all);

  admin_write_line (req, ADMIN_SHOW_RESTRICT_START_CON_ADMIN, "Rules affecting admin connections:");
  list_acl_control (req, info.admin_acl, all);

  admin_write_line (req, ADMIN_SHOW_RESTRICT_START_CON_SOURCE, "Rules affecting source connections:");
  list_acl_control (req, info.source_acl, all);

  admin_write_line (req, ADMIN_SHOW_RESTRICT_START_CON_CLIENT, "Rules affecting client connections:");
  list_acl_control (req, info.client_acl, all);
}

restrict_t *
create_restrict ()
{
  restrict_t *res = (restrict_t *) nmalloc (sizeof (restrict_t));
  return res;
}

restrict_t *
add_restrict (avl_tree *tree, char *mask, acltype_t type)
{
  restrict_t *res, *out;

  if (!mask || !mask[0])
    return NULL;

  res = create_restrict ();
  res->mask = nstrdup (mask);
  res->id = avl_count (tree);
  res->type = type;
  res->num_short_connections = -1; // added. ajd

  thread_mutex_lock (&info.acl_mutex);

  out = avl_replace (tree, res);

  thread_mutex_unlock (&info.acl_mutex);

  if (out)
  {
    nfree (out->mask);
    nfree (out);
  }



  return res;
}

int
del_restrict (avl_tree *tree, char *name, acltype_t type)
{
  avl_traverser trav = {0};

  restrict_t *res = NULL, *out;

  if (is_pattern (name))
  {
    thread_mutex_lock (&info.acl_mutex);

    while ((res = avl_traverse (tree, &trav)))
    {
      if ((ntripcaster_strcasecmp (res->mask, name) == 0) && (type == res->type))
        break;
    }
  } else if (is_number (name)) {
    restrict_t search;

    search.id = atoi (name);

    thread_mutex_lock (&info.acl_mutex);

    res = avl_find (tree, &search);
  }

  if (res)
  {
    out = avl_delete (tree, res);
    if (out)
    {
      nfree (out->mask);
      nfree (out);
      thread_mutex_unlock (&info.acl_mutex);
      return 1;
    }
  }
  thread_mutex_unlock (&info.acl_mutex);
  return 0;
}

/* 0 for "Not allowed, 1 for "allowed", -1 for not decided */
int
allowed_no_policy (connection_t *con, contype_t contype)
{
  int result;

  thread_mutex_lock (&info.acl_mutex);

  /* First check to see if we match an acl for our specific
   * connection type */
  result = restrict_list (con, get_acl_list (contype));

  if (result != -1)
  {
    thread_mutex_unlock (&info.acl_mutex);
    return result ? 1 : 0;
  }

  /* Next check if it matches against the generic all class
   * of acls */
  result = restrict_list (con, get_acl_list (-1));

  if (result != -1)
  {
    thread_mutex_unlock (&info.acl_mutex);
    return result ? 1 : 0;
  }

  thread_mutex_unlock (&info.acl_mutex);

  /* We don't match any ACLs, let someone else decide */
  return -1;
}

/* 0 for "Not allowed, 1 for "allowed", -1 for not decided */
int
allowed (connection_t *con, contype_t contype)
{
  int result;

  thread_mutex_lock (&info.acl_mutex);


//write_log(LOG_DEFAULT, "allowed:1");

  /* First check to see if we match an acl for our specific
   * connection type */
  result = restrict_list (con, get_acl_list (contype));

  if (result != -1)
  {
//write_log(LOG_DEFAULT, "allowed:2");
    thread_mutex_unlock (&info.acl_mutex);
    return result ? 1 : 0;
  }

//write_log(LOG_DEFAULT, "allowed:3");
  /* Next check if it matches against the generic all class
   * of acls */
  result = restrict_list (con, get_acl_list (-1));

//write_log(LOG_DEFAULT, "allowed:4");
  if (result != -1)
  {
    thread_mutex_unlock (&info.acl_mutex);
    return result ? 1 : 0;
  }

  thread_mutex_unlock (&info.acl_mutex);

//write_log(LOG_DEFAULT, "allowed:5");
  /* We don't match any ACLs, so push through the default */
  return info.policy;
}

avl_tree *
get_acl_list (contype_t contype)
{
  switch (contype)
  {
    case client_e:
      return info.client_acl;
      break;
    case source_e:
      return info.source_acl;
      break;
    case admin_e:
      return info.admin_acl;
      break;
    default:
      return info.all_acl;
      break;
  }
}

int
restrict_list (connection_t *con, avl_tree *list)
{
  avl_traverser trav = {0};
  restrict_t *res;

  int out = -1;

//write_log(LOG_DEFAULT, "restrict_list:1");

  /* First, we want to find him */
  while ((res = avl_traverse (list, &trav)))
  {

//write_log (LOG_DEFAULT, "Restrict:compared masks: %s %s\r\n", res->mask, con->host);

    if (wild_match ((unsigned char *)res->mask, (unsigned char *)con->host)
        || (con->hostname && wild_match ((unsigned char *)res->mask, (unsigned char *)con->hostname)))
    {
//write_log (LOG_DEFAULT, "Existing Restrict found: mask %s type %d num_con %d", res->mask, res->type, res->num_short_connections);
      con->res = res;
//      if ((res->num_short_connections > -1) && (res->regular_rule != 1)) return -1;
      out = res->type;
      if (out == 1) return 1;
    }
  }

//write_log(LOG_DEFAULT, "restrict_list:2");

  return out;
}

void
free_acl_list (avl_tree *list)
{
  restrict_t *res, *out;

  if (!list) {
    write_log (LOG_DEFAULT, "WARNING: NULL list passed to free_acl_list()");
    return;
  }

  while ((res = avl_get_any_node (list)))
  {
    if (!res) {
      write_log (LOG_DEFAULT, "WARNING: NULL item passed from avl_get_any_node()");
      continue;
    }

    out = avl_delete (list, res);
    if (out) {
      nfree (out->mask);
      nfree (out);
    } else {
      write_log (LOG_DEFAULT, "WARNING: avl_delete() of acl item failed!");
    }
  }
}

void
free_acl_lists ()
{
  if (!info.client_acl || !info.source_acl || !info.admin_acl || !info.all_acl)
  {
    write_log (LOG_DEFAULT, "WARNING: free_acl_lists(): NULL acl tree pointers, this is weird! (info at %p)", &info);
  }
  else
  {
    thread_mutex_lock (&info.acl_mutex);

    free_acl_list (info.client_acl);
    free_acl_list (info.source_acl);
    free_acl_list (info.admin_acl);
    free_acl_list (info.all_acl);

    thread_mutex_unlock (&info.acl_mutex);
  }
}



