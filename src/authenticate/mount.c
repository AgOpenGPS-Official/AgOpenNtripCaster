/*
 * mount.c
 * - mount authentication file stuff
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
#include "basic.h"
#include "mount.h"
#include "group.h"
#include "admin.h"
#include "http.h"

extern server_info_t info;

extern mutex_t authentication_mutex;
extern mounttree_t *client_mounttree;
extern mounttree_t *source_mounttree;
extern usertree_t *usertree;
extern grouptree_t *grouptree;

void parse_mount_authentication_file(char *mountfilename, mounttree_t *mt) {
  int fd;
  mount_t *mount;
  char line[FILE_LINE_BUFSIZE];
  char file[BUFSIZE];

  if ((get_ntripcaster_file(mountfilename, conf_file_e, R_OK, file) == NULL) || ((fd = open_for_reading(file)) == -1)) {
    xa_debug(1, "WARNING: Could not open mount authentication file");
    return;
  }

  while (fd_read_line(fd, line, FILE_LINE_BUFSIZE)) {
    if (line[0] == '#' || line[0] == ' ') continue;

    mount = create_mount_from_line(line);

    if (mount) add_authentication_mount(mount, mt);
  }

  if (line[FILE_LINE_BUFSIZE-1] == '\0') {
    write_log(LOG_DEFAULT, "READ ERROR: too long line in mount authentication file (exceeding FILE_LINE_BUFSIZE)");
  }

  fd_close(fd);
}

mount_t *
 create_mount_from_line(char *line)
{
  mount_t *mount;
  group_t *group;
  char name[BUFSIZE], cgroup[BUFSIZE];
  int go_on = 1;

  if (!line) {
    xa_debug(1, "WARNING: create_mount_from_line() called with NULL pointer");
    return NULL;
  }
  if (!splitc(name, line, ':')) {
    xa_debug(1, "ERROR: Syntax error in mount file, with line [%s]", line);
    return NULL;
  }
  mount = create_mount();

  mount->name = nstrdup(clean_string(name));

  do {
    if (splitc(cgroup, line, ',') == NULL) {
      strcpy(cgroup, line);
      go_on = 0;
    }
    group = find_group_from_tree(grouptree, clean_string(cgroup));

    if (!group) {
      write_log(LOG_DEFAULT, "WARNING: Unrecognized group [%s] specified for mount [%s]",
          cgroup, name);
    } else {
      avl_insert(mount->grouptree, group);
    }
  } while (go_on);

  return mount;
}

mount_t *
 create_mount()
{
  mount_t *mount = (mount_t *) nmalloc(sizeof (mount_t));

  mount->grouptree = create_group_tree();
  mount->name = NULL;
  return mount;
}

mounttree_t *
 create_mount_tree()
{
  mounttree_t *gt = avl_create(compare_mounts, &info);

  return gt;
}

static void freemount(mount_t *mount, void *param)
{
  nfree(mount->name);
  /* only destroy, don't free contents, these are in grouptree */
  if(mount->grouptree)
    avl_destroy(mount->grouptree, NULL);
  nfree(mount);
}

void add_authentication_mount(mount_t * mount, mounttree_t *mt) {
  mount_t *out;

  if (!mount || !mt || !mount->name || !mount->grouptree) {
    xa_debug(1, "ERROR: add_authentication_mount() called with NULL pointers");
    return;
  }
  out = avl_replace(mt, mount);

  if (out) {
    write_log(LOG_DEFAULT, "WARNING: Duplicate mount record %s, using latter", mount->name);
    freemount(out, 0);
  }
  xa_debug(1, "DEBUG: add_authentication_mount(): Inserted mount [%s]", mount->name);
}

void free_mount_tree(mounttree_t * mt)
{
  if(mt)
    avl_destroy(mt, (avl_node_func)freemount);
}

grouptree_t *get_grouptree_for_mount(const char *mountname, mounttree_t *mt) {
  mount_t *mount;
  avl_traverser trav = {0};

  if ((!mountname) || (!mt)) return NULL;

  while ((mount = avl_traverse(mt, &trav))) {
    if (mount->name && (ntripcaster_strcmp(mountname, mount->name) == 0))
      return mount->grouptree;
  }

  return NULL;
}

void con_display_mounts(com_request_t * req, mounttree_t *mt) {
  avl_traverser trav = {0};
  avl_traverser grouptrav = {0};
  mount_t *mount;
  group_t *group;
  int listed = 0;

  admin_write_line(req, ADMIN_SHOW_AUTH_MOUNT_START, "Listing mount points in the authentication module:");

  thread_mutex_lock(&authentication_mutex);

  while ((mount = avl_traverse(mt, &trav))) {
    zero_trav(&grouptrav);

    admin_write(req, ADMIN_SHOW_AUTH_MOUNT_ENTRY, "%s: ", mount->name ? mount->name : "(null)");

    while ((group = avl_traverse(mount->grouptree, &grouptrav)))
      admin_write(req, -1, "%s ", group->name);
    admin_write_line(req, -1, "");
    listed++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_line(req, ADMIN_SHOW_AUTH_MOUNT_END, "End of mount point listing (%d listed)", listed);
}

/*void
html_display_mounts(com_request_t *req, mounttree_t *mt) {

  char buf[BUFSIZE];
  group_t *group;
  mount_t *mount;
  int num = 0;
  avl_traverser trav_group = {0};
  avl_traverser trav_mount = {0};

  admin_write_string(req, ADMIN_SHOW_AUTH_MOUNT_START, "<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\r\n<tr><td><h2>Mount</h2></td><td><i>groups</i></td></tr>\r\n");

  thread_mutex_lock(&authentication_mutex);

  while ((mount = avl_traverse(mt, &trav_mount))) {

    snprintf(buf, sizeof(buf), "<tr><td><input type=\"checkbox\" name=\"mm%d\" value=\"%s\">%s</td><td>\r\n",
         num, mount->name, mount->name);
    admin_write_string(req, ADMIN_SHOW_AUTH_MOUNT_ENTRY, buf);

    while ((group = avl_traverse(mount->grouptree, &trav_group))) {

      admin_write(req, ADMIN_SHOW_AUTH_MOUNT_ENTRY, "%s, \r\n", group->name);

    }
    admin_write_string(req, ADMIN_SHOW_AUTH_MOUNT_ENTRY, "</td></tr>");

    zero_trav (&trav_group);
    num++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_string(req, ADMIN_SHOW_AUTH_MOUNT_END, "</table>");

}*/

int runtime_add_mount_with_group(const char *name, char *groups, char *mountfilename, mounttree_t *mt) {
  char line[BUFSIZE];
  char file[BUFSIZE];
  char *s;
  int fd;

  if (!name || !groups || !name[0] || !groups[0]) return ICE_ERROR_INVALID_SYNTAX;

  while ((s = strchr(groups, ' '))) *s = ',';

#ifdef _WIN32
  sprintf(line, "%s:%s\r\n", name, groups);
#else
  sprintf(line, "%s:%s\n", name, groups);
#endif

  thread_mutex_lock(&authentication_mutex);

  if (get_grouptree_for_mount(name, mt)) {
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_DUPLICATE;
  }

  if ((get_ntripcaster_file(mountfilename, conf_file_e, R_OK, file) == NULL) || ((fd = open_for_append(file)) == -1)) {
    xa_debug(1, "WARNING: Could not open mount authentication file for writing");
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_FILE;
  }
  fd_write_line(fd, "%s", line);
  fd_close(fd);

  thread_mutex_unlock(&authentication_mutex);
  return 1;
}

int runtime_add_mount(const char *name, char *mountfilename, mounttree_t *mt) {
  char line[BUFSIZE];
  char file[BUFSIZE];
  int fd;

  if (!name || !name[0]) return ICE_ERROR_INVALID_SYNTAX;
#ifdef _WIN32
  sprintf(line, "%s:\r\n", name);
#else
  sprintf(line, "%s:\n", name);
#endif

  thread_mutex_lock(&authentication_mutex);

  if (get_grouptree_for_mount(name, mt)) {
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_DUPLICATE;
  }

  if ((get_ntripcaster_file(mountfilename, conf_file_e, R_OK, file) == NULL) || ((fd = open_for_append(file)) == -1)) {
    xa_debug(1, "WARNING: Could not open mount authentication file for writing");
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_FILE;
  }
  fd_write_line(fd, "%s", line);
  fd_close(fd);

  thread_mutex_unlock(&authentication_mutex);
  return 1;
}

mounttree_t *get_client_mounttree() {
  return client_mounttree;
}

mounttree_t *get_source_mounttree() {
  return source_mounttree;
}
