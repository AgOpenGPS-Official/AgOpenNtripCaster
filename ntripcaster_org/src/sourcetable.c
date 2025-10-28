/* sourcetable.c
 * - sourcetable functions
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

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
# ifndef __USE_BSD
#  define __USE_BSD
# endif
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#if defined (_WIN32)
#include <windows.h>
#define strncasecmp strnicmp
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
#include "connection.h"
#include "sock.h"
#include "ntrip.h"
#include "sourcetable.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "log.h"
#include "source.h"
#include "memory.h"
#include "logtime.h"
#include "alias.h"
#include "match.h"

extern server_info_t info;

sourcetable_field_t stream_entry_fields[] = {
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { real_e, "", NULL },
  { real_e, "", NULL },
  { integer_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { unknown_type_e, NULL, NULL }
};

sourcetable_field_t caster_entry_fields[] = {
  { string_e, "", NULL },
  { string_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { real_e, "", NULL },
  { real_e, "", NULL },
  { string_e, "", NULL },
  { integer_e, "", NULL },
  { string_e, "", NULL },
  { unknown_type_e, NULL , NULL }
};

sourcetable_field_t network_entry_fields[] = {
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { string_e, "", NULL },
  { unknown_type_e, NULL, NULL }
};

void send_sourcetable (connection_t *con) {
  avl_traverser trav = {0};
  sourcetable_entry_t *se;
  string_buffer_t *sb;
  char time[50];
  const char *datatype = "text/plain";

  thread_mutex_lock(&info.sourcetable_mutex);

  sb = string_buffer_create(info.sourcetable.length+(info.sourcetable.lines*2)+1);

  {
    int found=0;
    int minfound=-1;
    int oldmin;

    while ((se = avl_traverse (info.sourcetable.tree, &trav))) if (se->show == 1) {
      if (!found) minfound = se->serial;
      found=1;
      minfound = ( minfound <= se->serial) ? minfound : se->serial;
    }

    while(found) {
      while ((se = avl_traverse (info.sourcetable.tree, &trav)))
      if (se->show == 1 &&  se->serial==minfound) {

        write_line_to_buffer(sb, se->line);
        found=0;
      }

      oldmin=minfound;
      minfound = 0x7fffff;

      while ((se = avl_traverse (info.sourcetable.tree, &trav)))
      if (se->show == 1 && se->serial > oldmin && se->serial < minfound) {
        found=1;
        minfound = se->serial;
      }
    }
  }

  thread_mutex_unlock(&info.sourcetable_mutex);

  if (con->com_protocol == ntrip2_0_e && !strncasecmp(get_user_agent(con), "ntrip", 5))
    datatype = "gnss/sourcetable";
  ntrip_write_message(con, HTTP_GET_SOURCETABLE_OK, get_formatted_time(HEADER_TIME, time), datatype, sb->pos+16);

  if(con->udpbuffers)
    con->rtp->datagram->pt = 96;

  sock_write_string_con(con, sb->buf);

  dispose_string_buffer(sb);

  sock_write_line_con (con, "ENDSOURCETABLE");
  if(con->udpbuffers)
  {
    con->rtp->datagram->pt = 98;
    sock_write_string_con(con, "");
  }
}

void send_sourcetable_filtered(connection_t *con, char *filter, int matchonly) {
  avl_traverser trav = {0};
  sourcetable_entry_t *se;
  string_buffer_t *sb;
  list_t *l;
  char time[50];

  l = get_filter_expression_list(filter,matchonly);

  thread_mutex_lock(&info.sourcetable_mutex);

  sb = string_buffer_create(info.sourcetable.length+(info.sourcetable.lines*2)+1);

  while ((se = avl_traverse (info.sourcetable.tree, &trav))) {
    if (match_sourcetable_entry(l, se) == 1)
      write_line_to_buffer(sb, se->line);
  }

  if (con->com_protocol == ntrip2_0_e)
    ntrip_write_message(con, HTTP_GET_SOURCETABLE_OK, get_formatted_time(HEADER_TIME, time),"gnss/sourcetable",sb->pos+16);
  else
    ntrip_write_message(con, HTTP_GET_SOURCETABLE_OK, get_formatted_time(HEADER_TIME, time),"text/plain",sb->pos+16);

  if(con->udpbuffers)
    con->rtp->datagram->pt = 96;

  sock_write_string_con(con, sb->buf);

  dispose_string_buffer(sb);

  thread_mutex_unlock(&info.sourcetable_mutex);

  sock_write_line_con (con, "ENDSOURCETABLE");
  if(con->udpbuffers)
  {
    con->rtp->datagram->pt = 98;
    sock_write_string_con(con, "");
  }

  list_dispose_with_data(l, dispose_parse_tree );
}

static void freesourcetableentry(sourcetable_entry_t *st, void *param)
{
  free_sourcetable_entry(st);
  nfree(st);
}

void read_sourcetable(void) {
        static int serial = 0;
  int st;
  char pathandfile[BUFSIZE], line[BUFSIZE];
  sourcetable_entry_t *newste = NULL, *oldste = NULL;

  get_ntripcaster_file (info.sourcetablefile, conf_file_e, R_OK, pathandfile);
  st = open_for_reading(pathandfile);

  if (st > -1) {
    thread_mutex_lock(&info.sourcetable_mutex);

    while (fd_read_line (st, line, BUFSIZE)) {
      newste = create_sourcetable_entry();
      newste->line = nstrdup(line);
      newste->linelen = strlen(line);

      newste->type = get_sourcetable_entry_type(line);
      newste->serial = serial++;
      if (newste->type == str_e)
        newste->show = 0;
      else
        newste->show = 1;

      xa_debug (2, "DEBUG: read_sourcetable: creating fields for [%s]...", line);

      newste->fields = create_entry_fields(newste->type, line);

      xa_debug (2, "DEBUG: read_sourcetable: creating id... ");

      newste->id = create_entry_id(newste);

      xa_debug (2, "DEBUG: read_sourcetable: id: [%s]", newste->id);

      oldste = avl_replace(info.sourcetable.tree, newste);

      if (oldste != NULL) {
        newste->show = oldste->show;
        info.sourcetable.length -= oldste->linelen;
        info.sourcetable.lines--;

        freesourcetableentry(oldste, 0);
      }

      info.sourcetable.length += newste->linelen;
      info.sourcetable.lines++;
    }

    thread_mutex_unlock(&info.sourcetable_mutex);

    fd_close(st);
  } else write_log(LOG_DEFAULT, "WARNING: Could not open %s !", info.sourcetablefile);
}

void sourcetable_add_source(source_t *source) {
  sourcetable_entry_t search, *found = NULL;

  memset(&search, 0, sizeof(search));
  search.id = source->audiocast.mount;
  search.type = str_e;

  thread_mutex_lock(&info.sourcetable_mutex);

  found = avl_find(info.sourcetable.tree, &search);
  if (found != NULL) found->show = 1;

  thread_mutex_unlock(&info.sourcetable_mutex);
}

void sourcetable_remove_source(source_t *source) {
  sourcetable_entry_t search, *found = NULL;

  memset(&search, 0, sizeof(search));
  search.id = source->audiocast.mount;
  search.type = str_e;

  thread_mutex_lock(&info.sourcetable_mutex);

  found = avl_find(info.sourcetable.tree, &search);
  if (found != NULL) found->show = 0;

  thread_mutex_unlock(&info.sourcetable_mutex);
}

void cleanup_sourcetable(void)
{
  thread_mutex_lock(&info.sourcetable_mutex);
  if (info.sourcetable.tree)
    avl_destroy(info.sourcetable.tree, (avl_node_func)freesourcetableentry);
  thread_mutex_unlock(&info.sourcetable_mutex);
}

/* must have sourcetable_mutex AND source_mutex. */
void sourcetable_set_show_status(void) {
  sourcetable_entry_t search, *found = NULL;
  avl_traverser trav = {0};
  connection_t *scon;
  memset(&search, 0, sizeof(search));
  search.type = str_e;

  while ((scon = avl_traverse (info.sources, &trav))) {
    search.id = scon->food.source->audiocast.mount;

    found = avl_find(info.sourcetable.tree, &search);
    if (found != NULL) found->show = 1;
  }
}

/* must have sourcetable_mutex. */
int sourcetable_calculate_show_size(void) {
  avl_traverser trav = {0};
  sourcetable_entry_t *ste;
  int bytes = 0;

  while ((ste = avl_traverse (info.sourcetable.tree, &trav))) {
    if ((ste->type != str_e) || (ste->show == 1)) bytes += (ste->linelen+2);
  }

  return bytes;
}

sourcetable_entry_type_t get_sourcetable_entry_type(const char *s) {
  if (strncasecmp(s, "STR", 3) == 0)
    return str_e;
  else if (strncasecmp(s, "NET", 3) == 0)
    return net_e;
  else if (strncasecmp(s, "CAS", 3) == 0)
    return cas_e;

  return unknown_e;
}

sourcetable_entry_t *create_sourcetable_entry() {
  sourcetable_entry_t *ste;

  ste = (sourcetable_entry_t *)nmalloc (sizeof(sourcetable_entry_t));
  ste->type = unknown_e;
  ste->id = NULL;
  ste->line = NULL;
  ste->fields = NULL;
  ste->linelen = -1;
  ste->show = 0;

  return ste;
}

void free_sourcetable_entry(sourcetable_entry_t *ste) {
  if (ste->id != NULL)
  {
    nfree (ste->id);
  }
  if (ste->line != NULL)
  {
    nfree (ste->line);
  }
  if (ste->fields != NULL) {
    free_entry_fields(ste->type, ste->fields);
    nfree (ste->fields);
  }
}

char *create_entry_id(sourcetable_entry_t *ste) {
  char buf[BUFSIZE];

  if (ste->type == str_e) {
    snprintf(buf, BUFSIZE, "/%s", get_string_value_by_index(ste->fields, 1));
  } else if (ste->type == cas_e) {
    snprintf(buf, BUFSIZE, "%s%s%d", get_string_value_by_index(ste->fields, 0), get_string_value_by_index(ste->fields, 1), get_integer_value_by_index(ste->fields, 2));
  } else if (ste->type == net_e) {
    snprintf(buf, BUFSIZE, "%s%s", get_string_value_by_index(ste->fields, 0), get_string_value_by_index(ste->fields, 1));
  } else {
    strncpy(buf, ste->line, BUFSIZE);
    buf[BUFSIZE-1] = 0;
  }

  return nstrdup(buf);
}

void *create_entry_fields(sourcetable_entry_type_t type, const char *s) {
  int field_len;
  register int i;
  sourcetable_field_t *array;
  sourcetable_field_t *field;
  void *fields;
  char line[BUFSIZE], field_string[BUFSIZE];

  if (type == str_e)
    array = stream_entry_fields;
  else if (type == cas_e)
    array = caster_entry_fields;
  else if (type == net_e)
    array = network_entry_fields;
  else return NULL;

  field_len = get_field_array_length(array);

  xa_debug (2, "DEBUG: create_entry_fields: field_len: %d", field_len);

  fields = nmalloc ((field_len + 1) * sizeof(sourcetable_field_t));

  xa_debug (2, "DEBUG: create_entry_fields: allocated memory: %lu bytes",
  ((field_len+1) * sizeof(sourcetable_field_t)));

  field = fields;
  strncpy(line, s, BUFSIZE);
  line[BUFSIZE-1] = 0;

  for (i=0; i < field_len; i++) {
    field->type = unknown_type_e;
    field->name = NULL;
    field->data = NULL;

    xa_debug (2, "DEBUG: create_entry_fields: setting field at %p of type %d",
    field, array->type);

    if (splitc(field_string, line, ';') == NULL)
      set_entry_field(field, array, line);
    else
      set_entry_field(field, array, field_string);

    field++;
    array++;
  }

  /* to mark the end of the array. */
  field->type = unknown_type_e;
  field->name = NULL;
  field->data = NULL;

  return fields;
}

void free_entry_fields(sourcetable_entry_type_t type, void *fields) {
  sourcetable_field_t *field;

  field = (sourcetable_field_t *)fields;

  while (!((field->type == unknown_type_e) && (field->name == NULL))) {
    if (field->data != NULL)
    {
      nfree(field->data);
    }
    field++;
  }
}

void set_entry_field(sourcetable_field_t *field, sourcetable_field_t *template, const char *s) {
  field->type = template->type;
  field->name = template->name;

  if (template->type == integer_e) {
    if (field->data == NULL) field->data = nmalloc (sizeof(int));
    *(int *)field->data = atoi(s);
  } else if (template->type == real_e) {
    if (field->data == NULL) field->data = nmalloc (sizeof(double));
    *(double *)field->data = atof(s);
  } else if (template->type == string_e) {
    if (field->data != NULL)
    {
      nfree (field->data);
    }
    field->data = nstrdup(s);
  }
}

sourcetable_field_t *create_entry_field(type_t type, char *name, char *data) {
  sourcetable_field_t *new = nmalloc (sizeof(sourcetable_field_t));

  new->type = type;
  new->name = NULL;
  new->data = NULL;

  if (name != NULL) new->name = nstrdup(name);

  if (type == integer_e) {
    new->data = nmalloc (sizeof(int));
    *(int *)new->data = atoi(data);
  } else if (type == real_e) {
    new->data = nmalloc (sizeof(double));
    *(double *)new->data = atof(data);
  } else if (type == string_e) {
    new->data = nstrdup(data);
  }

  return new;
}

/* use this function only on fields created with create_entry_field(...). */
void dispose_entry_field(sourcetable_field_t *field) {
  if (field->name != NULL)
  {
    nfree(field->name);
  }
  if (field->data != NULL)
  {
    nfree(field->data);
  }
  nfree(field);
}

/* compares f1 to f2. */
int compare_entry_fields(sourcetable_field_t *f1, sourcetable_field_t *f2) {
  if (f1->type == f2->type) {
    switch (f1->type) {
      case integer_e:
        if (*(int *)f1->data < *(int *)f2->data) return -1;
        if (*(int *)f1->data > *(int *)f2->data) return 1;
        return 0;
      case real_e:
        if (*(double *)f1->data < *(double *)f2->data) return -1;
        if (*(double *)f1->data > *(double *)f2->data) return 1;
        return 0;
      case string_e:
        /* WATCH OUT: wild_match(...) expects the filter expression (which
         * eventually contains wildcards) as first argument. */
        if (wild_match((unsigned char *)f1->data, (unsigned char *)f2->data) > 0)
          return 0;
        return strncmp((char *)f1->data, (char *)f2->data, BUFSIZE);
      default:
        break;
    }
  }

  /* Last ditch to repair different types: User supplied an int, where double
   * was expected and vice versa. */

  if (f1->type == integer_e &&  f2->type == real_e) {
    if ( ((double)(*(int *)f1->data)) < *(double *)f2->data) return -1;
    if ( ((double)(*(int *)f1->data)) > *(double *)f2->data) return  1;
        return 0;
  }

  if (f2->type == integer_e &&  f1->type == real_e) {
    if ( ((double)(*(int *)f2->data)) > *(double *)f1->data) return  -1;
    if ( ((double)(*(int *)f2->data)) < *(double *)f1->data) return  1;
        return 0;
  }

  // Now, everything is lost
  return -2;
}

int get_integer_value_by_index(void *fields, int index) {
  sourcetable_field_t *field;

  field = ((sourcetable_field_t *)fields) + index;

  if ((field->type == integer_e) && (field->data != NULL))
    return *(int *)field->data;
  else
    return -1;
}

double get_real_value_by_index(void *fields, int index) {
  sourcetable_field_t *field;

  field = ((sourcetable_field_t *)fields) + index;

  if ((field->type == real_e) && (field->data != NULL))
    return *(double *)field->data;
  else
    return -1.0;
}

char *get_string_value_by_index(void *fields, int index) {
  sourcetable_field_t *field;

  field = ((sourcetable_field_t *)fields) + index;

  if ((field->type == string_e) && (field->data != NULL))
    return (char *)field->data;
  else
    return "";
}

int get_integer_value(sourcetable_field_t *field) {
  if ((field->type == integer_e) && (field->data != NULL))
    return *(int *)field->data;
  else
    return -1;
}

double get_real_value(sourcetable_field_t *field) {
  if ((field->type == real_e) && (field->data != NULL))
    return *(double *)field->data;
  else
    return -1.0;
}

char *get_string_value(sourcetable_field_t *field) {
  if ((field->type == string_e) && (field->data != NULL))
    return (char *)field->data;
  else
    return "";
}

int get_field_array_length(sourcetable_field_t *array) {
  int c=0;
  while (array->name != NULL) {
    array++;
    c++;
  }
  return c;
}
