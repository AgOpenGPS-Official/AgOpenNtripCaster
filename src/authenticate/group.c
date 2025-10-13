/*
 * group.c
 * - group authentication file stuff
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
#include "definitions.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
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

/* Can someone verify this? */
#ifndef W_OK
#define W_OK 2
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
#include "avl_functions.h"
#include "restrict.h"
#include "sourcetable.h"
#include "match.h"
#include "memory.h"
#include "commands.h"
#include "admin.h"
#include "basic.h"
#include "group.h"
#include "user.h"
#include "http.h"

extern server_info_t info;

extern mutex_t authentication_mutex;
//extern mounttree_t *client_mounttree;
//extern mounttree_t *source_mounttree;
extern usertree_t *usertree;
extern grouptree_t *grouptree;

void parse_group_authentication_file()
{
  int fd;
  char groupfile[BUFSIZE];
  group_t *group;
  char line[FILE_LINE_BUFSIZE];



  if ((get_ntripcaster_file(info.groupfile, conf_file_e, R_OK, groupfile) == NULL ) || ((fd = open_for_reading(groupfile)) == -1)) {
//    if (groupfile)
//      nfree(groupfile);
    xa_debug(1, "WARNING: Could not open group authentication file");
    return;
  }
  while (fd_read_line(fd, line, FILE_LINE_BUFSIZE)) {
    if (line[0] == '#' || line[0] == ' ')
      continue;

    group = create_group_from_line(line);

    if (group)
      add_authentication_group(group);
  }

  if (line[FILE_LINE_BUFSIZE-1] == '\0') {
    write_log(LOG_DEFAULT, "READ ERROR: too long line in group authentication file (exceeding FILE_LINE_BUFSIZE)");
  }

//  if (groupfile)
//    nfree(groupfile);
  fd_close(fd);
}

group_t *
 create_group_from_line(char *line)
{
  group_t *group;
  ntripcaster_user_t *user;
  char name[BUFSIZE], cuser[BUFSIZE];
  int go_on = 1;

  if (!line) {
    xa_debug(1, "WARNING: create_group_from_line() called with NULL pointer");
    return NULL;
  }
  if (!splitc(name, line, ':')) {
    xa_debug(1, "ERROR: Syntax error in group file, with line [%s]", line);
    return NULL;
  }
  group = create_group();

  group->name = nstrdup(clean_string(name));

  do {
    if (!splitc(cuser, line, ',')) {

/* added to extract number of allowed simultaneous connections. ajd */
      if (!splitc(cuser, line, ':')) {

        strcpy(cuser, line); // there were just these two lines before. ajd
        group->max_num_con = -1;
        group->max_num_ip = -1;
      } else {
        line = clean_string(line);
        int ip = 0;
        if((line[0] == 'i' || line[0] == 'I' )
        && (line[1] == 'p' || line[1] == 'P')) {
          ip = 1;
          line += 2;
        }
        if (is_number(clean_string(line))) {
          if(ip) {
            group->max_num_con = -1;
            group->max_num_ip = atoi(line);
          } else {
            group->max_num_con = atoi(line);
            group->max_num_ip = -1;
          }
        } else {
          group->max_num_con = -1;
          group->max_num_ip = -1;
        }
      }

      go_on = 0;
    }
    user = find_user_from_tree(usertree, clean_string(cuser));

    if (!user) {
      write_log(LOG_DEFAULT, "WARNING: Unrecognized user [%s] specified for group [%s]",
          cuser, name);
    } else {
      avl_insert(group->usertree, user);
    }
  } while (go_on);

  return group;
}

group_t *
 create_group()
{
  group_t *group = (group_t *) nmalloc(sizeof (group_t));

  group->usertree = create_user_tree();
  group->name = NULL;
  return group;
}

grouptree_t *
 create_group_tree()
{
  grouptree_t *gt = avl_create(compare_groups, &info);

  return gt;
}

static void freegroup(group_t *group, void *param)
{
  nfree(group->name);
  avl_destroy(group->usertree, NULL);
  nfree(group);
}

void add_authentication_group(group_t * group)
{
  group_t *out;

  if (!group || !grouptree || !group->name || !group->usertree) {
    xa_debug(1, "ERROR: add_authentication_group() called with NULL pointers");
    return;
  }
  out = avl_replace(grouptree, group);

  if (out) {
    write_log(LOG_DEFAULT, "WARNING: Duplicate group record %s, using latter", group->name);
    freegroup(out, 0);
  }
  xa_debug(1, "DEBUG: add_authentication_group(): Inserted group [%s]", group->name);
}

void free_group_tree(grouptree_t * gt)
{
  if (gt)
    avl_destroy(gt, (avl_node_func)freegroup);
}

int is_member_of(char *user, group_t * group)
{
  ntripcaster_user_t *up;
  ntripcaster_user_t search;

  if (!user || !group || !group->usertree) {
    xa_debug(1, "WARNING: is_member_of() called with NULL pointers");
    return 0;
  }
  search.name = user;

  up = avl_find(group->usertree, &search);

  return up ? 1 : 0;
}

group_t *find_group_from_tree(grouptree_t * gt, const char *name) {
  group_t search;

  if (!gt || !name) {
    xa_debug(1, "WARNING: find_group_from_tree() called with NULL pointers");
    return NULL;
  }

  search.name = strchr(name, name[0]); // moved here from above. ajd

  return avl_find(gt, &search);
}

void con_display_groups(com_request_t * req)
{
  avl_traverser trav =
  {0};
  avl_traverser usertrav =
  {0};
  group_t *group;
  ntripcaster_user_t *user;
  int listed = 0;

  admin_write_line(req, ADMIN_SHOW_AUTH_GROUP_START, "Listing groups in the authentication module:");

  thread_mutex_lock(&authentication_mutex);

  while ((group = avl_traverse(grouptree, &trav))) {
    zero_trav(&usertrav);

    admin_write(req, ADMIN_SHOW_AUTH_GROUP_ENTRY, "%s: ", group->name ? group->name : "(null)");

    while ((user = avl_traverse(group->usertree, &usertrav)))
      admin_write(req, -1, "%s ", user->name);

    admin_write_line(req, -1, " %d %d", group->max_num_con, group->max_num_ip);
    listed++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_line(req, ADMIN_SHOW_AUTH_GROUP_END, "End of group listing (%d listed)", listed);
}

/*void
html_display_groups(com_request_t *req) {

  char buf[BUFSIZE];
  group_t *group;
  ntripcaster_user_t *user;
  int num = 0;
  avl_traverser trav_group = {0};
  avl_traverser trav_user = {0};

  admin_write_string(req, ADMIN_SHOW_AUTH_GROUP_START, "<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\r\n<tr><td><h2>Group(max_num_con)</h2></td><td><i>users</i></td></tr>\r\n");

  thread_mutex_lock(&authentication_mutex);

  while ((group = avl_traverse(grouptree, &trav_group))) {

    snprintf(buf, sizeof(buf), "<tr><td><input type=\"checkbox\" name=\"gg%d\" value=\"%s\">%s (%d)</td><td>\r\n",
         num, group->name, group->name, group->max_num_con);
    admin_write_string(req, ADMIN_SHOW_AUTH_GROUP_ENTRY, buf);

    while ((user = avl_traverse(group->usertree, &trav_user))) {

      admin_write(req, ADMIN_SHOW_AUTH_GROUP_ENTRY, "%s, ", user->name);

    }
    admin_write_string(req, ADMIN_SHOW_AUTH_GROUP_ENTRY, "</td></tr>");

    zero_trav (&trav_user);
    num++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_string(req, ADMIN_SHOW_AUTH_GROUP_END, "</table>");

}*/


int runtime_add_group(const char *name)
{
  char line[BUFSIZE];
  char groupfile[BUFSIZE];
  int fd;

  if (!name || !name[0])
    return ICE_ERROR_INVALID_SYNTAX;
#ifdef _WIN32
  sprintf(line, "%s:\r\n", name);
#else
  sprintf(line, "%s:\n", name);
#endif

  thread_mutex_lock(&authentication_mutex);

  if (find_group_from_tree(grouptree, name)) {
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_DUPLICATE;
  }

  if ((get_ntripcaster_file(info.groupfile, conf_file_e, W_OK, groupfile) == NULL) || ((fd = open_for_append(groupfile)) == -1)) {
//    if (groupfile)
//      nfree(groupfile);
    xa_debug(1, "WARNING: Could not open group authentication file for writing");
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_FILE;
  }
  fd_write_line(fd, "%s", line);
  fd_close(fd);
//  if (groupfile)
//    nfree(groupfile);
  thread_mutex_unlock(&authentication_mutex);
  return 1;
}


int runtime_add_group_with_user(const char *name, char *users)
{
  char line[BUFSIZE];
  char groupfile[BUFSIZE];
  char *s;
  int fd;

  if (!name || !users || !name[0] || !users[0])
    return ICE_ERROR_INVALID_SYNTAX;

  while ((s = strchr(users, ' ')))
    *s = ',';

#ifdef _WIN32
  sprintf(line, "%s:%s\r\n", name, users);
#else
  sprintf(line, "%s:%s\n", name, users);
#endif

  thread_mutex_lock(&authentication_mutex);

  if (find_group_from_tree(grouptree, name)) {
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_DUPLICATE;
  }

  if ((get_ntripcaster_file(info.groupfile, conf_file_e, W_OK, groupfile) == NULL) || ((fd = open_for_append(groupfile)) == -1)) {
//    if (groupfile)
//      nfree(groupfile);
    xa_debug(1, "WARNING: Could not open group authentication file for writing");
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_FILE;
  }
  fd_write_line(fd, "%s", line);
  fd_close(fd);
//  if (groupfile)
//    nfree(groupfile);
  thread_mutex_unlock(&authentication_mutex);
  return 1;
}
