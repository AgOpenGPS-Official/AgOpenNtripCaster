/* match.h
 * - wildcard matching function headers
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

#ifndef __NTRIPCASTER_MATCH_H
#define __NTRIPCASTER_MATCH_H

#define MATCH (match+saved+sofar)
#define NOMATCH 0
#define TOKEN_VALUE_LENGTH 100
#define CONSTANT -1

/*typedef enum { and_e = 1, or_e = 2, not_e = 3, equal_e = 4, not_equal_e = 5, less_e = 6, greater_e = 7, less_equal_e = 8, greater_equal_e = 9, approx_e = 10, constant_e = -1 } expression_type_t;*/

typedef struct token_St {
        int type;
        char *value;
} token_t;

typedef struct expression_St {
        int type;
        void *left;
        void *right;
} expression_t;

int wild_match(register unsigned const char *m, register unsigned const char *n);
int match_sourcetable_entry(list_t *expression_list, sourcetable_entry_t *entry);
int match_sourcetable_field(expression_t *root, sourcetable_field_t *field);
list_t *get_filter_expression_list(char *filter, int matchonly);
expression_t *parse_expression(char *expr);
void dispose_parse_tree(expression_t *root);
list_t *get_token_list(char *expr);
token_t *get_next_token(char **pp);
expression_t *create_expression(int type, void *left, void *right);
token_t *create_token(int type, char* value);
void dispose_token(token_t *t);

//void test_expression_match();

#endif
