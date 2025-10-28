/* http.h
 * - HTTP function declarations
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

#ifndef __NTRIPCASTER_HTTP_H
#define __NTRIPCASTER_HTTP_H

typedef struct
{
  char *name;                     /* User printable name of the function. */
  ntripcaster_int_function *func; /* Function to call to do the job. */
  int wrap;
  char *argument;
} http_command_t;

typedef struct
{
  int message_type;
  char *html;
} html_wrapper_t;

/* html links for admin web interface. */
typedef struct
{
  char *path;
  char *link;
  char *space;
} http_link_t;

int http_admin_command (connection_t *con, ntrip_request_t *req);
void display_admin_page (connection_t *con, ntrip_request_t *req);
void http_display_home_page (connection_t *con);
void http_get_favicon (connection_t *con);
void http_get_robots (connection_t *con);
void write_http_header (sock_t sockfd, int error, const char *msg);
http_parsable_t *find_http_element (char *name, http_parsable_t *el);
int print_http_variable (vartree_t *request_vars, const char *name, connection_t *clicon, int fd);
char *url_encode(const char *string, char **result_p);
char *url_decode (const char *string);
const char *parse_template_file (connection_t *clicon, connection_t *sourcecon, const char *ptr, int fd, vartree_t *variables);
int write_template_parsed_html_page (connection_t *clicon, connection_t *sourcecon, const char *template_file, int fd, vartree_t *variables);
const char *http_loop_sources (char *ident, connection_t *clicon, const char *ptr, int fd, vartree_t *variables);
const char *http_loop_admins (char *ident, connection_t *clicon, const char *ptr, int fd, vartree_t *variables);
const char *http_loop_listeners (char *ident, connection_t *clicon, const char *ptr, int fd, vartree_t *variables);
char *ntripcaster_uptime ();
char *ntripcaster_starttime ();
const void *get_http_variable (vartree_t *request_vars, const char *name);
void write_http_code_page (connection_t *con, int code, const char *msg);
void write_401 (connection_t *con, char *realm);
void write_didnt_find_html_page (connection_t *con, char *file);
void display_generic_admin_page (connection_t *con);
int http_write_string (const com_request_t *req, const int message_type, const char *buff);
const http_command_t *find_http_command (const char *name, const http_command_t *el);
int http_display (com_request_t *req);
void write_no_such_command (connection_t *con, const char *name);
int http_change (com_request_t *req);
int http_write_links(const com_request_t * req);
#endif





