/*
 * user.c
 * - User authentication file stuff
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
#include "win32config.h"
#else
#include "config.h"
#endif
#endif

#include "definitions.h"
#include <stdio.h>
#include "definitions.h"

#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

#ifdef HAVE_LIBLDAP
#include "ldapAuthenticate.h"
#endif /* HAVE_LIBLDAP */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
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
#include "vars.h"
#include "memory.h"
#include "basic.h"
#include "commands.h"
#include "admin.h"
#include "user.h"
#include "http.h"

extern server_info_t info;

extern mutex_t authentication_mutex;
//extern mounttree_t *client_mounttree;
//extern mounttree_t *source_mounttree;
extern usertree_t *usertree;
extern grouptree_t *grouptree;

void parse_user_authentication_file()
{
  int fd;
  ntripcaster_user_t *user;
  char line[BUFSIZE];
  char userfile[BUFSIZE];

  if ((get_ntripcaster_file(info.userfile, conf_file_e, R_OK, userfile) == NULL) || ((fd = open_for_reading(userfile)) == -1)) {
//    if (userfile)
//      nfree(userfile);
    xa_debug(1, "WARNING: Could not open user authentication file");
    return;
  }
  while (fd_read_line(fd, line, BUFSIZE)) {
    if (line[0] == '#' || line[0] == ' ')
      continue;

    user = create_user_from_line(line);

    if (user)
      add_authentication_user(user);
  }

  if (line[BUFSIZE-1] == '\0') {
    write_log(LOG_DEFAULT, "READ ERROR: too long line in user authentication file (exceeding BUFSIZE)");
  }

//  if (userfile)
//    nfree(userfile);
  fd_close(fd);
}

int runtime_add_user(char *name, char *password)
{
  char line[BUFSIZE];
  char userfile[BUFSIZE];
  int fd;

  if (!name || !password || !name[0] || !password[0])
    return ICE_ERROR_INVALID_SYNTAX;
#ifdef _WIN32
  sprintf(line, "%s:%s\r\n", name, password);
#else
  sprintf(line, "%s:%s\n", name, password);
#endif

  thread_mutex_lock(&authentication_mutex);

  if (find_user_from_tree(usertree, name)) {
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_DUPLICATE;
  }

  if ((get_ntripcaster_file(info.userfile, conf_file_e, R_OK, userfile) == NULL) || ((fd = open_for_append(userfile)) == -1)) {
//    if (userfile)
//      nfree(userfile);
    xa_debug(1, "WARNING: Could not open user authentication file for writing");
    thread_mutex_unlock(&authentication_mutex);
    return ICE_ERROR_FILE;
  }
  fd_write_line(fd, "%s", line);
  fd_close(fd);
//  if (userfile)
//    nfree(userfile);
  thread_mutex_unlock(&authentication_mutex);
  return 1;
}

ntripcaster_user_t *
create_user_from_line(char *line)
{
  ntripcaster_user_t *user;
  char name[BUFSIZE], pass[BUFSIZE];

  if (!line) {
    xa_debug(1, "WARNING: create_user_from_line() called with NULL pointer");
    return NULL;
  }
  if (!splitc(name, line, ':')) {
    xa_debug(1, "ERROR: Syntax error in user file, with line [%s]", line);
    return NULL;
  }
  if (!splitc(pass, line, ':'))
    strcpy(pass, line);

  user = create_user();
  user->name = nstrdup(clean_string(name));
  user->pass = nstrdup(clean_string(pass));

  return user;
}

ntripcaster_user_t *
create_user()
{
  ntripcaster_user_t *user = (ntripcaster_user_t *) nmalloc(sizeof (ntripcaster_user_t));

  user->name = NULL;
  user->pass = NULL;
  return user;
}

usertree_t *
 create_user_tree()
{
  usertree_t *ut = avl_create(compare_users, &info);

  return ut;
}

static void freeuser(ntripcaster_user_t *user, void *param)
{
  nfree(user->name);
  nfree(user->pass);
  nfree(user);
}

void add_authentication_user(ntripcaster_user_t * user)
{
  ntripcaster_user_t *out;

  if (!user || !usertree || !user->name || !user->pass) {
    xa_debug(1, "ERROR: add_authentication_user() called with NULL pointers");
    return;
  }
  out = avl_replace(usertree, user);

  if (out) {
    write_log(LOG_DEFAULT, "WARNING: Duplicate user record %s, using latter", user->name);
    freeuser(out, 0);
  }
  xa_debug(1, "DEBUG: add_authentication_user(): Inserted user [%s:%s]", user->name, user->pass);
}

void free_user_tree(usertree_t * ut)
{
  if (ut)
    avl_destroy(ut, (avl_node_func)freeuser);
}

int user_authenticate(char *cuser, const char *password)
{
  const ntripcaster_user_t *user;
  ntripcaster_user_t search;

  search.name = cuser;

  if (!cuser || !password) {
    xa_debug(1, "WARNING: user_authenticate() called with NULL pointer");
    return 0;
  }
#ifdef HAVE_LIBLDAP
  if(info.ldap_server[0])
  {
    return ldap_authenticate(cuser,password);
  }
#endif /* HAVE_LIBLDAP */

  user = avl_find(usertree, &search);

  if (!user) return 0;

  return password_match(user->pass, password);
}

ntripcaster_user_t * find_user_from_tree(usertree_t * ut, char *name) {
  ntripcaster_user_t search;
/*
  search.name = strchr(name, name[0]);  // sigh...
*/
  if (!ut || !name) {
    xa_debug(1, "WARNING: find_user_from_tree() called with NULL pointers");
    return NULL;
  }

  search.name = name;

  return avl_find(ut, &search);
}

ntripcaster_user_t *con_get_user(connection_t * con) {
  ntripcaster_user_t *outuser = NULL;
  const char *cauth;
  char *decoded, *ptr;
  char cryptype[BUFSIZE];
  char user[BUFSIZE];
  char auth[BUFSIZE];
  char pass[BUFSIZE];

  if (con == NULL) {
    xa_debug(1, "WARNING: con_get_user() called with NULL pointer");
    return NULL;
  }

  cauth = get_con_variable(con, "Authorization");

  if (cauth == NULL) return NULL;

  strcpy(auth, cauth);

  if (splitc(cryptype, auth, ' ') == NULL) {
    xa_debug(1, "DEBUG: con_get_user() uncrypted: [%s]", auth);
    if (splitc(user, auth, ':') == NULL) {
      strcpy(user, auth);
      pass[0] = '\0';
    } else {
      strcpy(pass, auth);
    }
  } else {
    if (strncasecmp(cryptype, "basic", 5) == 0) {
      xa_debug(1, "DEBUG: con_get_user() decoding: [%s]", auth);
      ptr = decoded = util_base64_decode(auth);
      if (decoded != NULL) {
        xa_debug(1, "DEBUG: con_get_user() decoded: [%s]", decoded);
        if (splitc(user, decoded, ':') == NULL) {
          strcpy(user, decoded);
          pass[0] = '\0';
        } else {
          strcpy(pass, decoded);
        }
        free(ptr);
      } else return NULL;
    } else {
      xa_debug(1, "WARNING: con_get_user(): unsupported cryptype");
      return NULL;
    }
  }

  outuser = (ntripcaster_user_t *)nmalloc(sizeof(ntripcaster_user_t));
  outuser->name = strdup(user);
  outuser->pass = strdup(pass);

  return outuser;
}

void con_display_users(com_request_t * req)
{
  ntripcaster_user_t *user;
  avl_traverser trav =
  {0};
  int listed = 0;

  admin_write_line(req, ADMIN_SHOW_AUTH_USER_START, "Listing users in the authentication module");

  thread_mutex_lock(&authentication_mutex);

  while ((user = avl_traverse(usertree, &trav))) {
    admin_write_line(req, ADMIN_SHOW_AUTH_USER_ENTRY, "User: [%s]", user->name);
    listed++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_line(req, ADMIN_SHOW_AUTH_USER_END, "End of user listing (%d listed)", listed);
}

/*void
html_display_users(com_request_t *req) {

  char buf[BUFSIZE];
  ntripcaster_user_t *user;
  int num = 0;
  avl_traverser trav = {0};

  admin_write_string(req, ADMIN_SHOW_AUTH_USER_START, "<table border=\"0\" cellpadding=\"5\" cellspacing=\"0\">\r\n<tr><td><h2>User</h2></td><td><i>password</i></td></tr>\r\n");

  thread_mutex_lock(&authentication_mutex);

  while ((user = avl_traverse(usertree, &trav))) {

    snprintf(buf, sizeof(buf), "<tr><td><input type=\"checkbox\" name=\"uu%d\" value=\"%s\">%s</td><td>%s</td></tr>\r\n",
         num, user->name, user->name, user->pass);

    admin_write_string(req, ADMIN_SHOW_AUTH_USER_ENTRY, buf);

    num++;
  }

  thread_mutex_unlock(&authentication_mutex);

  admin_write_string(req, ADMIN_SHOW_AUTH_MOUNT_END, "</table>\r\n");


}*/
