/* item.c
 * - Functions for item lists
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

#include "avl.h"
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
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
#include "item.h"

extern server_info_t info;

/* Top level interface to the new output interface for admins.
 * Call with request struct that contains the admin socket and some other stuff,
 * type - The command specific type of message (commands.h listing stuff),
 * itype - The context dependant specifier (header, list_start, list_end, list_item.. etc),
 * num - Number of items
 * items...
*/

void
item_write_formatted_line (com_request_t *req, int type, item_type_t itype, int num, ...)
{
  va_list ap;
  int first = 1;
  char valuefirst[BUFSIZE];

  if (!req || num < 1) {
    write_log (LOG_DEFAULT, "ERROR: Invalid arguments for admin_write_formatted_line()");
    return;
  }

  va_start (ap, num);

  item_write_type (req, type, itype, token_header, NULL);

  while (num > 0) {
    item_t *item = va_arg (ap, item_t *);
    item_write_item (req, itype, item);

    if (first) {
      if (strchr(item->format, 's') && item->value != NULL) // was (item->format[1] == 's' && item->value != NULL). ajd

        snprintf(valuefirst, BUFSIZE,  "%s", (char *)item->value);

      first = 0;
    }

    item_dispose (item);
    num -= 1;
  }

  item_write_type (req, type, itype, token_footer, valuefirst);

  va_end (ap);
}

void
item_write_type (com_request_t *req, int type, item_type_t itype, item_token_t token, const char *firstvalue)
{
  if (!req) {
    write_log (LOG_DEFAULT, "ERROR: item_write_type() called with invalid arguments");
    return;
  }

  switch (token) {
    case token_header:
      if (type == -1 || (admin_scheme (req) == default_scheme_e))
        return; /* No header */
      else if (admin_scheme (req) == html_scheme_e) {
        switch (itype) {
          case list_start:
            admin_write_raw (req, "%s", "<h2>");
            break;
          case list_caption:
            admin_write_raw (req, "%s", "<tr>");
            break;
          case list_end:
            admin_write_raw (req, "%s", "</table><h2>");
            break;
          case header:
            admin_write_raw (req, "%s", "<h1>");
            break;
          case list_item:
          case list_set_item:
            admin_write_raw (req, "%s", "<tr>");
            break;

          default:
            break;
        }
      } else if (admin_scheme (req) == tagged_scheme_e) {
        if (req->wid == -1)
          admin_write_raw (req, "M%d ", type);
        else
          admin_write_raw (req, "M%d W%d ", type, req->wid);
      }
      break;

    case token_footer:
      if (admin_scheme (req) == html_scheme_e) {
        switch (itype) {
          case list_start:
            admin_write_raw (req, "%s", "</h2><table class=\"listtable\">\r\n");
            break;
          case list_end:
            admin_write_raw (req, "%s", "</h2>\r\n");
            break;
          case header:
            admin_write_raw (req, "%s", "</h1>\r\n");
            break;
          case list_caption:
            admin_write_raw (req, "%s", "</tr>\r\n");
            break;
          case list_item:
            admin_write_raw (req, "%s", "</tr>\r\n");
            break;
          case list_set_item: // was admin_write_raw (req, "<td><a href=\"/admin?mode=change&argument=%s\">Change</a></td><td><a href=\"/admin?mode=help#%s\">Help</a></td></tr>\r\n", firstvalue, firstvalue); . ajd

/*          admin_write_raw (req, "<td><a href=\"/admin?mode=change&argument=%s\">Change</a></td></tr>\r\n", firstvalue);*/

            admin_write_raw (req, "%s", "</tr>\r\n");
            break;

          default:
            admin_write_raw (req, "%s", "<br>\r\n");
        }
      } else {
        if (itype != list_caption)
          admin_write_raw (req, "%s", "\r\n");
      }
      break;
  }
}

void
item_write_item (com_request_t *req, item_type_t itype, item_t *item)
{
  if (!req || !item || !item->name) {
    write_log (LOG_DEFAULT, "WARNING: item_write_item() called with invalid arguments");
    return;
  }


  switch (admin_scheme (req)) {
    case tagged_scheme_e:
    case default_scheme_e:
      item_write_item_default (req, itype, item);
      break;
    case html_scheme_e:
      item_write_item_html (req, itype, item);
      break;
  }
}

void
item_write_item_default (com_request_t *req, item_type_t itype, item_t *item)
{
  char buf[BUFSIZE];

  buf[0] = '\0';

  switch (itype) {
    case list_start:
    case list_end:
    case header:
            if (item->value == NULL){
        admin_write_raw (req, "%s", item->name);}
      else {
        snprintf(buf, BUFSIZE, "%s", item->name);
        item_write_value (req, item->format, buf, item->value);
      }
      break;
    case list_item:
    case list_set_item:
      if (item->name && item->name[0])
        snprintf(buf, BUFSIZE, "[%s: %s] ", item->name, item->format);
      else
        snprintf(buf, BUFSIZE, "%s", item->format);
      item_write_value (req, item->format, buf, item->value);
      break;
    default:
      break;
  }
}

void
item_write_item_html (com_request_t *req, item_type_t itype, item_t *item)
{
  char buf[BUFSIZE];

  buf[0] = '\0';

  switch (itype) {
    case header:
      if (item->value == NULL){
        admin_write_raw (req, "%s", item->name);}
      else {
        snprintf(buf, BUFSIZE, "%s", item->name);
        item_write_value (req, item->format, buf, item->value);
      }
      break;
    case list_start:
      if (item->value == NULL)
        admin_write_raw (req, "%s", item->name);
      else {
        snprintf(buf, BUFSIZE, "%s", item->name);
        item_write_value (req, item->format, buf, item->value);
      }
      break;
    case list_end:
      if (item->value == NULL)
        admin_write_raw (req, "%s", item->name);
      else {
        snprintf(buf, BUFSIZE, "%s", item->name);
        item_write_value (req, item->format, buf, item->value);
      }
      break;
    case list_item:
    case list_set_item:
      snprintf(buf, BUFSIZE, "<td>%s</td>", item->format);
      item_write_value (req, item->format, buf, item->value);
      break;
    case list_caption:
      admin_write_raw (req, "<td>%s</td>", item->name);
      break;
    default:
      if (item->value == NULL)
        admin_write_raw (req, "%s", item->name);
      else {
        snprintf(buf, BUFSIZE, "%s", item->name);
        item_write_value (req, item->format, buf, item->value);
      }
      break;
  }
}

item_t *
item_create (const char *name, const char *format, const void *value)
{
  item_t *new = (item_t *) nmalloc (sizeof (item_t));
  new->name = nstrdup (name);
  new->format = nstrdup (format);
  new->value = value;
  return new;
}

void
item_dispose (item_t *item)
{
  nfree (item->name);
  nfree (item->format);
  nfree (item);
}

void
item_write_value (const com_request_t *req, const char *format, const char *buf, const void *value)
{
  if (buf == NULL) {
    write_log (LOG_DEFAULT, "ERROR: item_write_value() called with NULL buf");
    return;
  }

  if (value == NULL || format == NULL) {
    admin_write_raw (req, "%s", buf ? buf : "(null)");
    return;
  }


  if (format[1] == 'd') {
    admin_write_raw (req, buf, *(int *)value);}
  else if (format[1] == 'f') {
    admin_write_raw (req, buf, *(double *)value);}
  else {
    admin_write_raw (req, buf, value);}
}




