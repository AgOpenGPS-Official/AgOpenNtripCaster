/* vars.c
 * - Parsing variables and stuff
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
#include "win32config.h"
#else
#include "config.h"
#endif
#endif

#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef __USE_BSD
#define __USE_BSD
#endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "threads.h"
#include "avl.h"
#include "avl_functions.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "vars.h"
#include "log.h"
#include "logtime.h"
#include "ntripcaster_string.h"
#include "memory.h"
#include "http.h"

extern server_info_t info;

vartree_t *
create_header_vars ()
{
  avl_tree *t = avl_create (compare_header_vars, &info);
  return t;
}

void
extract_header_vars (char *line, vartree_t *vars)
{
  char *colonptr;
  char name[BUFSIZE];

  if (!line || !vars)
  {
    xa_debug (1, "ERROR: extract_header_vars() called with NULL pointers");
    return;
  }

  colonptr = strchr (line, ':');
  if (!colonptr && line[0])
  {
    xa_debug (1, "WARNING: Invalid header line [%s] without colon", line);
    return;
  }

  if (splitc (name, line, ':') == NULL)
  {
    if (line[0])
      xa_debug (1, "WARNING: Invalid header line [%s]", line);
    return;
  }

  add_varpair2 (vars, nstrdup (clean_string (name)), nstrdup (clean_string (line)));
}

varpair_t *
create_varpair ()
{
  varpair_t *vp = (varpair_t *) nmalloc (sizeof (varpair_t));
  return vp;
}

void
add_varpair2 (vartree_t *request_vars, char *name, char *value)
{
  varpair_t *vp, *out;

  if (!request_vars)
    {
      xa_debug (2, "add_varpair2() called with NULL tree");
      return;
    }
  else if (!name || !value)
    {
      xa_debug (2, "add_varpair2() called with NULL values");
      return;
    }

  vp = create_varpair ();
  vp->name = name;
  vp->value = value;

  xa_debug (3, "DEBUG: Adding varpair [%s] == [%s]", vp->name, vp->value);

  if (!vp->name || !vp->value)
    {
      xa_debug (1, "WARNING: Adding NULL variables to tree");
      return;
    }

  out = avl_replace (request_vars, vp);

  if (out)
    {
      nfree (out->name);
      nfree (out->value);
      nfree (out);
    }
}

void
add_varpair (vartree_t *request_vars, char *varpair)
{
  char name[BUFSIZE];

  if (!varpair)
    {
      xa_debug (2, "WARNING: add_varpair called with NULL input");
      return;
    }

  name[0] = '\0';

  if (splitc (name, varpair, '=') == NULL) /* No '=' -> invalid */
    {
      xa_debug (1, "WARNING: Invalid varpair [%s]", varpair);
      return;
    }

  add_varpair2 (request_vars, nstrdup (name), nstrdup (varpair));
}

void
extract_vars (vartree_t *request_vars, char *requeststring)
{
  char varpair[BUFSIZE];
  int go_on = 1;

  varpair[0] = '\0';

  if (!requeststring || !requeststring[0])
  {
    xa_debug (1, "WARNING: Empty request string");
    return;
  }

  if (strchr (requeststring, '?') == NULL)
  {
    xa_debug (1, "WARNING: extract_vars called without vars");
    return;
  }

  splitc (NULL, requeststring, '?'); /* get rid of whatever.cgi? */

  do
  {
    if (splitc (varpair, requeststring, '&') == NULL)
    {
      strcpy (varpair, requeststring);
      go_on = 0;
    }
    add_varpair (request_vars, varpair);
  } while (go_on);
}

const char *
get_con_variable (connection_t *con, const char *name)
{
  if (!con || !con->headervars) return NULL;

  return (get_variable (con->headervars, name));
}

const char *
get_variable (vartree_t *request_vars, const char *name)
{
  varpair_t search, *vp;

  if (!request_vars || !name)
  {
    xa_debug (2, "WARNING: get_variable called with NULL pointers");
    return NULL;
  }

  search.name = strchr (name, *name);

  vp = avl_find (request_vars, &search);
  if (!vp) return NULL;
  return vp->value;
}

void
free_con_variables (connection_t *con)
{
  if (!con)
    return;
  free_variables (con->headervars);
  con->headervars = NULL;
}

void
free_variables (vartree_t *request_vars)
{
  varpair_t *vp, *out;

  if (!request_vars)
  {
    xa_debug (2, "WARNING: free_variables called with NULL tree");
    return;
  }

  while ((vp = avl_get_any_node (request_vars)))
  {
    out = avl_delete (request_vars, vp);

    if (!out)
    {
      xa_debug (2, "DEBUG: Fishy stuff in free_variables.");
      continue;
    }

    nfree (out->name);
    nfree (out->value);
    nfree (out);
  }

  avl_destroy (request_vars, NULL);
}

/* returns a string with all variables whose name begins with 'type'
   (a 2 character string). */
char *
get_all_vars_of_type (const char *type, vartree_t *request_vars) {

  char buff[BUFSIZE];
  char all[BUFSIZE];
  avl_traverser trav = {0};
  varpair_t *varpair;
  int found = 0;

  strncpy(all, type, BUFSIZE);
  all[BUFSIZE-1] = 0;

  while ((varpair = avl_traverse(request_vars, &trav))) {

    if (strncmp(varpair->name, type, 2) == 0) {
        if(snprintf(buff, BUFSIZE, "%s,%s", all, varpair->value) >= BUFSIZE)
          buff[BUFSIZE-1] = 0;
        strncpy(all, buff, BUFSIZE);
        all[BUFSIZE-1] = 0;
        found = 1;
    }
  }

  if (found == 0)
    return NULL;
  else
    return nstrdup(all);
}

/* generated strings of form "<type>,<arg1>,<arg2>,...". */
void
typecast_arguments(vartree_t *request_vars, com_request_t *comreq) {

  char buff[BUFSIZE];
  avl_traverser trav = {0};
  varpair_t *varpair;
  int i;

  for (i = 0; i<10; i++) comreq->typearg[i] = (char *)NULL;

  while ((varpair = avl_traverse(request_vars, &trav))) {

    if ((strncmp(varpair->name, "ar", 2) == 0) || (strncmp(varpair->name, "mo", 2) == 0)) continue;

    i = 0;

    while ((i < 10) && (comreq->typearg[i] != NULL) && strncmp(varpair->name, comreq->typearg[i], 2) != 0) i++;

    if (i < 10) {

      decode_url_string(varpair->value);

      if (comreq->typearg[i] == NULL) {
        comreq->typearg[i] = (char *)nmalloc(BUFSIZE);
        snprintf(comreq->typearg[i], BUFSIZE, "%s %s", varpair->name, varpair->value);
      } else {
        snprintf(buff, BUFSIZE, "%s %s", comreq->typearg[i], varpair->value);
        strncpy(comreq->typearg[i], buff, BUFSIZE);
      }
    }
  }
}

void
nfree_typecasted_arguments(com_request_t *comreq) {

  int i;

  for (i = 0; i < COMREQUEST_NUMARGS; i++)
  {
    if(comreq->typearg[i] != NULL)
    {
      nfree(comreq->typearg[i]);
    }
  }
}
