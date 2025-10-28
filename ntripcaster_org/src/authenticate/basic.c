/*
 * basic.c
 * - Basic Authentication module
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
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

#include <sys/stat.h>

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "connection.h"
#include "log.h"
#include "logtime.h"
#include "sock.h"
#include "avl_functions.h"
#include "restrict.h"
#include "sourcetable.h"
#include "match.h"
#include "memory.h"
#include "basic.h"
#include "user.h"
#include "group.h"
#include "mount.h"
#include "vars.h"

extern server_info_t info;
mutex_t authentication_mutex = {MUTEX_STATE_UNINIT};
mounttree_t *client_mounttree = NULL;
mounttree_t *source_mounttree = NULL;
usertree_t *usertree = NULL;
grouptree_t *grouptree = NULL;
time_t lastrehash = 0;

void rehash_authentication_scheme()
{
  int rehash_it = 0;
  struct stat st;
  char file[BUFSIZE];

  if (get_ntripcaster_file(info.userfile, conf_file_e, R_OK, file) != NULL)
    if (stat(file, &st) == 0) {
      if (st.st_mtime > lastrehash)
        rehash_it = 1;
    }
  if (!rehash_it && (get_ntripcaster_file(info.groupfile, conf_file_e, R_OK, file) != NULL))
    if (stat(file, &st) == 0) {
      if (st.st_mtime > lastrehash)
        rehash_it = 1;
    }
  if (!rehash_it && (get_ntripcaster_file(info.client_mountfile, conf_file_e, R_OK, file) != NULL))
    if (stat(file, &st) == 0) {
      if (st.st_mtime > lastrehash)
        rehash_it = 1;
    }
  if (!rehash_it && (get_ntripcaster_file(info.source_mountfile, conf_file_e, R_OK, file) != NULL))
    if (stat(file, &st) == 0) {
      if (st.st_mtime > lastrehash)
        rehash_it = 1;
    }

  if (rehash_it) parse_authentication_scheme();
}

void init_authentication_scheme()
{
  thread_create_mutex(&authentication_mutex);

  parse_authentication_scheme();
}

/*
 * Clean and setup authentication scheme.
 * Run every time any authentication file changes
 * Assert Class: 1
 */
void parse_authentication_scheme() {

  thread_mutex_lock(&authentication_mutex);

  /*
   * Make a clean slate
   */
  destroy_authentication_scheme();

  /*
   * Parse user file and flip it into memory
   */
  parse_user_authentication_file();

  /*
   * Dito with group file, with pointers to every user
   */
  parse_group_authentication_file();

  /*
   * Dito with mount file, with pointers to every group
   */
  parse_mount_authentication_file(info.client_mountfile, client_mounttree);
  parse_mount_authentication_file(info.source_mountfile, source_mounttree);

  thread_mutex_unlock(&authentication_mutex);

  lastrehash = get_time();
}

void destroy_authentication_scheme() {
  cleanup_authentication_scheme();

  client_mounttree = create_mount_tree();
  source_mounttree = create_mount_tree();
  grouptree = create_group_tree();
  usertree = create_user_tree();
}

void cleanup_authentication_scheme() {
  free_mount_tree(client_mounttree);
  free_mount_tree(source_mounttree);
  free_group_tree(grouptree);
  free_user_tree(usertree);
}

int authenticate_user_request(connection_t *con, ntrip_request_t *req, contype_t contype) {
  avl_traverser trav = {0};
  ntripcaster_user_t *checkuser;
  mounttree_t *mt;
  mount_t *mount;
  group_t *group;
  int ret = 0;

  checkuser = con_get_user(con);

  thread_mutex_lock(&authentication_mutex);

  if (contype == source_e)
    mt = source_mounttree;
  else
    mt = client_mounttree;

  mount = need_authentication_with_mutex(req, mt);

  xa_debug(2, "DEBUG: authenticate_user_request() mount %s user %s", mount ? mount->name : "<none>",
  checkuser ? checkuser->name : "<none>");
  if (mount == NULL) {
    group = find_group_from_tree(grouptree, "monitor");
    if ((group != NULL) && (checkuser != NULL) && (is_member_of(checkuser->name, group))) {
      if (con->group == NULL) con->group = nstrdup(group->name);
      con->ghost = 1;
    }
    if(strcmp(req->path, "all"))
      ret = 1;
  } else if ((checkuser != NULL) && (user_authenticate(checkuser->name, checkuser->pass))) {
    while ((group = avl_traverse(mount->grouptree, &trav))) {
      if (is_member_of(checkuser->name, group)) {
        xa_debug(2, "DEBUG: authenticate_user_request() group %s user %s", group ? group->name : "<none>",
        checkuser ? checkuser->name : "<none>");
        if (con->group == NULL) con->group = nstrdup(group->name);
        if (strncmp(group->name, "monitor", 7) == 0) con->ghost = 1;
        ret = 1;
        break;
      }
    }
  } else {
    xa_debug(1, "DEBUG: User authentication failed!!!");
  }

  thread_mutex_unlock(&authentication_mutex);

  if (checkuser != NULL) {
    nfree(checkuser->name);
    nfree(checkuser->pass);
    nfree(checkuser);
  }

  xa_debug(2, "DEBUG: authenticate_user_request() mount %s ret %d path %s",
  mount ? mount->name : "<none>", ret, req->path);

  if(strncmp(req->path, "/admin", 6) && strncmp(req->path, "/oper", 5)
  && strncmp(req->path, "/home", 5) && strncmp(req->path, "/favicon.ico", 12)
  && strncmp(req->path, "/robots.txt", 11)
  && strcmp(req->path, "all") && strcmp(req->path, "default")) {
    ntrip_request_t r;
    if(!mount) {
      generate_request("default", &r);
      memmove(r.path, r.path+1, strlen(r.path+1)+1);
      ret = authenticate_user_request(con, &r, contype);
    }
    if(!ret) {
      generate_request("all", &r);
      memmove(r.path, r.path+1, strlen(r.path+1)+1);
      ret = authenticate_user_request(con, &r, contype);
    }
  }

  return ret;
}

int authenticate_user_request_ntrip1upload(connection_t *con,
ntrip_request_t *req, const char *pwd) {
  mount_t *mount;
  int ret = 0;

  thread_mutex_lock(&authentication_mutex);

  mount = need_authentication_with_mutex(req, source_mounttree);

  xa_debug(2, "DEBUG: authenticate_user_request_ntrip1upload() mount %s",
  mount ? mount->name : "<none>");
  if (mount) {
    avl_traverser trav = {0};
    group_t *group;
    while (!ret && (group = avl_traverse(mount->grouptree, &trav))) {
      avl_traverser utrav = {0};
      ntripcaster_user_t *user;
      while (!ret && (user = avl_traverse(group->usertree, &utrav))) {
        if(user_authenticate(user->name, pwd)) {
          xa_debug(2, "DEBUG: authenticate_user_request() group %s user %s",
          group->name, user->name);
          if (con->group == NULL) con->group = nstrdup(group->name);
          if (strncmp(group->name, "monitor", 7) == 0) con->ghost = 1;
          ret = 1;
        }
      }
    }
  }

  thread_mutex_unlock(&authentication_mutex);

  xa_debug(2, "DEBUG: authenticate_user_request_ntrip1upload() mount %s ret "
  "%d path %s", mount ? mount->name : "<none>", ret, req->path);

  return ret;
}

mount_t *need_authentication(ntrip_request_t * req, mounttree_t *mt) {
  mount_t *mount;
  thread_mutex_lock(&authentication_mutex);
  mount = need_authentication_with_mutex(req, mt);
  thread_mutex_unlock(&authentication_mutex);
  return mount;
}

mount_t *need_authentication_with_mutex(ntrip_request_t * req, mounttree_t *mt) {
  mount_t *mount;
  mount_t search;

  xa_debug(3, "DEBUG: Checking need for authentication on mount %s", req->path);

  search.name = req->path;

  mount = avl_find(mt, &search);

  return mount;
}

int
check_ip_restrictions(connection_t *con) {
  int max_ip = info.max_ip_connections, numip = 0, numgroupip = 0;
  int group_max_ip = info.max_ip_connections;
  connection_t *clicon;
  avl_traverser clitrav = {0};
  group_t *group;
  ntripcaster_user_t *conuser;
  avl_traverser grouptrav = {0};

  if(max_ip <= 0)
  {
    max_ip = group_max_ip = DEFAULT_MAX_IP_CONNECTIONS;
  }

  if((conuser = con_get_user(con)))
  {
    thread_mutex_lock(&authentication_mutex);
    while (info.max_ip_connections >= 0 &&
    (group = avl_traverse (grouptree, &grouptrav))) {
      if (group->max_num_con == -1)
      {
        avl_traverser usertrav = {0};
        ntripcaster_user_t *user;
        while ((user = avl_traverse (group->usertree, &usertrav))) {
          if (!strcmp(user->name, conuser->name)) {
            xa_debug(1, "DEBUG: IP connections user %s is in group %s max %d",
            user->name, group->name, group->max_num_con);
            if(group->max_num_ip != -1) {
              if(group->max_num_ip < group_max_ip) /* take the smallest value */
                group_max_ip = group->max_num_ip;
            } else {
              max_ip = -1;
              break;
            }
          }
        }
      }
    }
    thread_mutex_unlock(&authentication_mutex);

    xa_debug(1, "DEBUG: IP connections user %s max %d%s", conuser->name,
    max_ip, max_ip < 0 ? " accepted" : "");
    if(max_ip < 0)
    {
      nfree(conuser->name);
      nfree(conuser->pass);
      nfree(conuser);
      return 1;
    }
  }

  thread_mutex_lock (&info.client_mutex);
  while ((clicon = avl_traverse (info.clients, &clitrav))) {
    if (clicon->sin && clicon->sin->sin_addr.s_addr == con->sin->sin_addr.s_addr) {
      if(conuser)
      {
        ntripcaster_user_t *u = con_get_user(clicon);
        if(u)
        {
          if(!strcmp(u->name, conuser->name))
            ++numgroupip;
          nfree(u->name);
          nfree(u->pass);
          nfree(u);
        }
      }
      ++numip;
    }
  }
  thread_mutex_unlock (&info.client_mutex);

  xa_debug(1, "DEBUG: IP connections user %s max %d num %d res %s group max %d num %d res %s",
  conuser ? conuser->name : "-", max_ip, numip, numip < max_ip ? "accepted" : "not accepted",
  group_max_ip, numgroupip, numgroupip < group_max_ip ? "accepted" : "not accepted");

  if(conuser)
  {
    nfree(conuser->name);
    nfree(conuser->pass);
    nfree(conuser);
  }

  return numip < max_ip && numgroupip < group_max_ip ? 1 : 0;
}

int
add_group_connection(connection_t *con) {
  int ret = 1;
  group_t *congroup;

  xa_debug(2, "DEBUG: add_group_connection() id %d group %s",
  con->id, !con->group ? "<none>" : con->group);
  if (con->group != NULL) {
    int maxgroup = -1;
    thread_mutex_lock(&authentication_mutex);
    congroup = find_group_from_tree(grouptree, con->group);
    if (congroup != NULL) {
      maxgroup = congroup->max_num_con;
      xa_debug(2, "DEBUG: add_group_connection() check group %s, max connections %d",
      congroup->name, congroup->max_num_con);
    } else {
      xa_debug(2, "DEBUG: add_group_connection() id %d did not find group %s",
      con->id, con->group);
    }
    thread_mutex_unlock(&authentication_mutex);

    if(maxgroup >= 0) {
      connection_t *clicon;
      avl_traverser clitrav = {0};
      int numgroup = 0;

      thread_mutex_lock (&info.client_mutex);
      while((clicon = avl_traverse (info.clients, &clitrav))) {
        if(clicon->groupactive && clicon->group && !strcmp(clicon->group, con->group))
          ++numgroup;
      }

      if (numgroup >= maxgroup) {
        xa_debug(2, "DEBUG: add_group_connection() id %d no remaining connections for group %s (%d of %d used)",
        con->id, congroup->name, numgroup, maxgroup);
        ret = 0;
      } else{
        con->groupactive = 1;
        ++numgroup;
        xa_debug(2, "DEBUG: add_group_connection() id %d remaining connections for group %s is %d (%d of %d used)",
        con->id, congroup->name, maxgroup-numgroup, numgroup, maxgroup);
      }
      thread_mutex_unlock (&info.client_mutex);
    }
  }
  return ret;
}

void
remove_group_connection(connection_t *con) {
  con->groupactive = 0;
}
