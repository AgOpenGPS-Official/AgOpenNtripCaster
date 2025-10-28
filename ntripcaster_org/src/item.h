/* item.h
 * - Line item Function Headers
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

#ifndef __NTRIPCASTER_LINE_H
#define __NTRIPCASTER_LINE_H

typedef struct output_item_t {
  char *name;
  char *format;
  const void *value;
} item_t;

typedef enum {token_header = 0, token_footer = 1 } item_token_t;
typedef enum {list_start = 0, list_end = 1, list_item = 2, header = 3, list_caption = 4, plaintext = 5, list_set_item = 6} item_type_t;

void item_write_formatted_line (com_request_t *req, int type, item_type_t itype, int num, ...);
void item_write_type (com_request_t *req, int type, item_type_t itype, item_token_t token, const char *firstvalue);
void item_write_item (com_request_t *req, item_type_t itype, item_t *item);
void item_write_item_default (com_request_t *req, item_type_t itype, item_t *item);
void item_write_item_html (com_request_t *req, item_type_t itype, item_t *item);
item_t *item_create (const char *name, const char *format, const void *value);
void item_dispose (item_t *item);
void item_write_value (const com_request_t *req, const char *format, const char *buf, const void *value);

#endif


