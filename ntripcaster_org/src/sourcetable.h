/* sourcetable.h
 * - sourcetable function headers
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

#ifndef __SOURCETABLE_H
#define __SOURCETABLE_H

typedef enum { cas_e = 1, net_e = 2, str_e = 3, all_e = 4, unknown_e = -1 } sourcetable_entry_type_t;

typedef struct sourcetable_entry_St {
  sourcetable_entry_type_t type;
  void *fields;
  char *id;
  char *line;
  int linelen;
  int show;
  int serial;
} sourcetable_entry_t;

typedef struct sourcetable_field_St {
  type_t type;
  char *name;
  void *data;
} sourcetable_field_t;

void send_sourcetable (connection_t *con);
void send_sourcetable_filtered(connection_t *con, char *filter, int matchonly);
void read_sourcetable(void);
void cleanup_sourcetable(void);
void sourcetable_add_source(source_t *source);
void sourcetable_remove_source(source_t *source);
//void rehash_sourcetable();
//void free_sourcetable_tree(avl_tree *tree, sourcetable_entry_type_t type);
void sourcetable_set_show_status(void);
int sourcetable_calculate_show_size(void);

sourcetable_entry_type_t get_sourcetable_entry_type(const char *s);
sourcetable_entry_t *create_sourcetable_entry();
void free_sourcetable_entry(sourcetable_entry_t *ste);
char *create_entry_id(sourcetable_entry_t *ste);
void *create_entry_fields(sourcetable_entry_type_t type, const char *s);
void free_entry_fields(sourcetable_entry_type_t type, void *fields);
void set_entry_field(sourcetable_field_t *field, sourcetable_field_t *template, const char *s);
sourcetable_field_t *create_entry_field(type_t type, char *name, char *data);
void dispose_entry_field(sourcetable_field_t *field);
int compare_entry_fields(sourcetable_field_t *f1, sourcetable_field_t *f2);

int get_integer_value_by_index(void *fields, int index);
double get_real_value_by_index(void *fields, int index);
char *get_string_value_by_index(void *fields, int index);
int get_integer_value(sourcetable_field_t *field);
double get_real_value(sourcetable_field_t *field);
char *get_string_value(sourcetable_field_t *field);

int get_field_array_length(sourcetable_field_t *array);

#endif
