/* admin.c
 * - Admin Functions
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
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#endif

#ifdef HAVE_LIBREADLINE
#include <readline.h>
#ifdef HAVE_HISTORY_H
#include <history.h>
#endif
#endif

#include "avl.h"
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "admin.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "log.h"
#include "commands.h"
#include "sock.h"
#include "connection.h"
#include "logtime.h"
#include "restrict.h"
#include "memory.h"
#include "source.h"
#include "http.h"
#include "vars.h"

extern server_info_t info;

connection_t *local_admin_console = NULL;

/*
 * Brand new admin, check the password and give him the prompt
 * in a new thread if it's valid
 * Called from handle_connection, and the line should be something like
 * ADMIN <password>
*/
void admin_login(connection_t *con, ntrip_request_t *req) {

  xa_debug (2, "DEBUG: admin_login: called with connection %d", con->id);

#ifdef HAVE_LIBWRAP
  if (!sock_check_libwrap (con->sock, admin_e)) {
    write_http_code_page (con, 403, "Forbidden");
    kick_not_connected(con, "Access denied (libwrap (admin connection))");
    thread_exit(0);
  }
#endif
  if (!allowed(con, admin_e)) {
    write_http_code_page (con, 403, "Forbidden");
    kick_not_connected(con, "Access denied (internal acl list (admin connection))");
    thread_exit(0);
  }

  if (!authenticate_admin_request(con)) {
    sock_write_line (con->sock, "ERROR - Bad Password");
    kick_not_connected (con, "Invalid admin password"); // was kick_connection. ajd
    thread_exit(0);
  }

  if (info.num_admins >= info.max_admins) {
    sock_write_line (con->sock, "ERROR - Too many connected admins");
    kick_not_connected (con, "Too many connected admins"); // was kick_connection. ajd
    thread_exit(0);
  }

  write_log(LOG_DEFAULT, "Accepted admin pass from connection %d. %d admins connected", con->id, info.num_admins + 1);

  sock_write_line(con->sock, "OK");

  thread_rename("Remote Admin Thread");

  put_admin (con);

  thread_mutex_lock(&info.admin_mutex);
  avl_insert(info.admins, con);
  thread_mutex_unlock(&info.admin_mutex);
  handle_remote_admin (con);

}

int authenticate_admin_request(connection_t *con) {
  const char *var;

  var = get_con_variable(con, "Authorization");
  if (var == NULL) return 0;

  xa_debug(1, "DEBUG: authenticate_admin_request(): checking pass %s", var);

  if (password_match(info.remote_admin_pass, var) == 1)
    return 1;
  else
    return 0;
}

/* This is called, as a new thread, to handle local admins */
void *
handle_admin (void *vcon)
{
  char line[BUFSIZE];
  connection_t *con = (connection_t *) vcon;
  mythread_t *mt;
  line[0] = '\0';
  con->food.admin->thread = thread_self ();

  thread_init ();

  mt = thread_get_mythread ();

  fd_write (con->sock, "-> ");

  sock_set_blocking (con->sock, SOCK_BLOCKNOT);
  sock_set_no_linger (con->sock);

  while (con->food.admin->alive && thread_alive (mt)) {
    /* con->sock will be STDOUT, so special case reading to use STDIN */
    if (fd_read_line_nb(fileno(stdin), line, BUFSIZE))
      handle_admin_command (con, line, ntripcaster_strlen (line));
    else
      fd_write_line (con->sock, "You can run, but you can't hide!");
  }
/* added locking. ajd */
thread_mutex_lock(&info.admin_mutex);
  close_connection (con);
thread_mutex_unlock(&info.admin_mutex);

  thread_exit(0);
  return NULL;
}

#ifdef HAVE_LIBREADLINE

char **
ntripcaster_completion (char *text, int start, int end)
{
  char **matches;
  matches = (char **) NULL;
  if (start == 0)
    matches = completion_matches (text, commands_generator);
  else if (ntripcaster_strncmp (rl_line_buffer, "set ", 4) == 0)
    matches = completion_matches (text, settings_generator);
  return (matches);
}

int
sleep_for_a_while ()
{
  my_sleep (30000);
  return 0;
}

void
initialize_readline ()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  xa_debug (2, "DEBUG: Initializing readline");
  rl_readline_name = "NtripCaster";
  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = (CPPFunction *)ntripcaster_completion;
  rl_event_hook = sleep_for_a_while;
  rl_initialize ();


#ifdef HAVE_HISTORY_H
  {
    char *file = NULL;
    /* Initialize history */
    using_history ();
    file = get_log_file ("NtripCaster.history");
    if (file)
    {
      read_history (file);
      nfree (file);
    }
  }
#endif
  xa_debug (2, "DEBUG: Done initializing readline");
}

void
uninitialize_readline ()
{
#ifdef HAVE_HISTORY_H
  char *file = get_log_file ("NtripCaster.history");
  if (file)
  {
    write_history (file);
    nfree (file);
  }
#endif
  rl_reset_terminal ("vt100");
}

void *
handle_admin_with_readline (void *vcon)
{
  char *line = NULL;
  connection_t *con = (connection_t *) vcon;
  mythread_t *mt;

  con->food.admin->thread = thread_self ();
  thread_init ();

  mt = (mythread_t *) thread_get_mythread ();

  do
  {
    if (line) {
      free (line);
      line = NULL;
    }

    if (is_server_running())
      line = readline (info.prompt);

    if (line)
    {
#ifdef HAVE_HISTORY_H
      add_history (strdup (line));
#endif
      handle_admin_command (con, line, ntripcaster_strlen (line));
    }
  } while (con->food.admin->alive && thread_alive (mt));

/* This will probably never happen since readline() is a bitch */
  write_log(LOG_DEFAULT, "Uninitializing readline...");
  uninitialize_readline();

/* added locking. ajd */
thread_mutex_lock(&info.admin_mutex);
  close_connection (con);
thread_mutex_unlock(&info.admin_mutex);

  thread_exit(0);

  return NULL;
}

#endif

/* This is called, as a new thread, to handle remote admins */
void
handle_remote_admin(connection_t *con)
{
  char line[BUFSIZE] = "";
  mythread_t *mt;
  con->food.admin->thread = thread_self ();

  thread_init ();

  mt = thread_get_mythread ();

  sock_write (con->sock, "-> ");

  sock_set_blocking (con->sock, SOCK_BLOCKNOT);

  while (con->food.admin->alive && thread_alive (mt)) {
    if (sock_read_line_nb (con->sock, line, BUFSIZE))
      handle_admin_command (con, line, ntripcaster_strlen (line));
    else
      break;
  }

  if (con->food.admin->alive)
    kick_connection (con, "Admin signed off");

/* added locking. ajd */
thread_mutex_lock(&info.admin_mutex);
  close_connection (con);
thread_mutex_unlock(&info.admin_mutex);

}

admin_t *create_admin()
{
  admin_t *admin = (admin_t *)nmalloc(sizeof(admin_t));
  return admin;
}

void put_admin(connection_t *con)
{
  admin_t *adm = create_admin();
  con->food.admin = adm;
  con->type = admin_e;
  adm->commands = 0;
  adm->oper = 0;
  adm->tailing = 0;
  adm->status = 1;
  adm->alive = 1;
  adm->scheme = default_scheme_e;
  adm->debuglevel = 0;
  add_admin ();
}

void
put_http_admin (connection_t *con)
{
  admin_t *adm = create_admin();
  con->food.admin = adm;
  con->type = admin_e;
  adm->commands = 0;
  adm->oper = 0;
  adm->tailing = 0;
  adm->status = 1;
  adm->alive = 1;
  adm->scheme = html_scheme_e;
  adm->debuglevel = 0;
}

void add_admin()
{
  internal_lock_mutex (&info.misc_mutex);
  info.num_admins++;
  internal_unlock_mutex  (&info.misc_mutex);
}

void del_admin()
{
  internal_lock_mutex (&info.misc_mutex);
  info.num_admins--;
  internal_unlock_mutex (&info.misc_mutex);
}

void
add_ntripcaster_console()
{
  connection_t *con;
  admin_t *adm;

  write_log (LOG_DEFAULT, "Using stdin as NtripCaster operator console");
  con = create_connection();
  con->sock = fileno(stdin); /* STDIN is special cased where appropriate */
  con->host = nstrdup("NtripCaster console");
  con->hostname = NULL;
  con->connect_time = get_time();
  put_admin(con);
  adm = con->food.admin;
  adm->oper = 1;

  if (info.console_mode == CONSOLE_ADMIN_TAIL) {
    write_log(LOG_DEFAULT, "Tailing file to NtripCaster operator console");
    adm->tailing = 1;
  }

  con->id = new_id();

  /* Initialize readline for the ntripcaster console, rox :) */
#ifdef HAVE_LIBREADLINE
  initialize_readline ();
#endif

        /* No threads here, no need to lock */
  avl_insert(info.admins, con);

  write_log (LOG_DEFAULT, "Starting Admin Console Thread...");

#ifdef HAVE_LIBREADLINE
  thread_create ("Admin Console Thread", handle_admin_with_readline, (void *)con);
#else
  thread_create ("Admin Console Thread", handle_admin, (void *)con);
#endif
  local_admin_console = con;
}

void
describe_admin (const com_request_t *req, const connection_t *admcon)
{
  const admin_t *admin;

  if (!req || !admcon)
  {
    xa_debug (1, "WARNING: describe_admin(): called with NULL pointers");
    return;
  }

  if (admcon->type != admin_e)
  {
    xa_debug (1, "WARNING: describe_admin(): called with invalid type");
    return;
  }

  describe_connection (req, admcon);

  admin = admcon->food.admin;

  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_START,"Misc admin info:");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_MISC, "Display regular status information: %s",
        admin->status ? "yes" : "no");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_MISC, "NtripCaster operator: %s", admin->oper ? "yes" : "no");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_MISC, "Tailing logfile: %s", admin->tailing ? "yes" : "no");
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_MISC, "Commands executed: %d", admin->commands);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_MISC, "Debuglevel: %d", admin->debuglevel);
  admin_write_line (req, ADMIN_SHOW_DESCRIBE_ADMIN_END, "End of admin info");
}

const char *
admin2string (admin_t *adm, char *buf)
{
  if (!adm || !buf)
    return nullcheck_string (NULL);

  buf[0] = '\0';

  if (adm->oper)
    strcat (buf, "O");
  if (adm->tailing)
    strcat (buf, "T");
  if (adm->status)
    strcat (buf, "S");
  return buf;
}

void
write_admin_prompt (const connection_t *con)
{
#ifdef HAVE_LIBREADLINE
  if (con->host && con->food.admin->alive && ntripcaster_strcmp (con->host, "NtripCaster console") != 0)
    sock_write (con->sock, "-> ");
#else
  sock_write (con->sock, "-> ");
#endif
}

scheme_t
admin_scheme (com_request_t *req)
{
  return req->con->food.admin->scheme;
}

int
admin_write_raw (const com_request_t *req, const char *fmt, ...)
{
  char buf[BUFSIZE];
  va_list ap;

  if (!req || !fmt)
    return 0;

  va_start (ap, fmt);

  vsnprintf(buf, BUFSIZE, fmt, ap);

  va_end (ap);

  return sock_write (req->con->sock, "%s", buf);
}


/* The following two functions are used by admin functions to output properly */
int
admin_write(const com_request_t * req, const int message_type,
      const char *fmt, ...)
{
  char buff[BUFSIZE];
  va_list ap;

  if (!req || !fmt || !req->con || req->con->type != admin_e)
    return 0;

  va_start(ap, fmt);
  vsnprintf(buff, BUFSIZE, fmt, ap);
  va_end(ap);

  return admin_write_string(req, message_type, buff);
}

int
admin_write_string(const com_request_t * req, const int message_type,
       const char *buff)
{
  if (message_type == -1
      || (req->con->food.admin->scheme == default_scheme_e)) {
    if (req->wid == -1)
      return sock_write(req->con->sock, "%s", buff);
    else
      return sock_write(req->con->sock, "W%d %s", req->wid,
            buff);
  } else if (req->con->food.admin->scheme == html_scheme_e) {
    return http_write_string(req, message_type, buff);
  } else {
    if (req->wid == -1)
      return sock_write(req->con->sock, "M%d %s",
            message_type, buff);
    else
      return sock_write(req->con->sock, "M%d W%d %s",
            message_type, req->wid, buff);
  }
}

int
admin_write_line(const com_request_t * req, const int message_type,
     const char *fmt, ...)
{
  char buff[BUFSIZE];
  char buff2[BUFSIZE + 8];
  va_list ap;

  if (!req || !fmt || !req->con || req->con->type != admin_e)
    return 0;

  va_start(ap, fmt);
  vsnprintf(buff, BUFSIZE, fmt, ap);
  buff2[0] = '\0';
  if (req->con->food.admin->scheme == html_scheme_e)
    catsnprintf(buff2, BUFSIZE + 8, "%s<br>\r\n", buff);
  else
    catsnprintf(buff2, BUFSIZE + 8, "%s\r\n", buff);
  return admin_write_string(req, message_type, buff2);
}

void
admin_die ()
{
}

