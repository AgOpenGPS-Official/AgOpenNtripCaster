/* admin.h
 * - Admin Function Headers
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

#ifndef __NTRIPCASTER_ADMIN_H
#define __NTRIPCASTER_ADMIN_H

admin_t *create_admin();
void admin_login(connection_t *con, ntrip_request_t *req);
int authenticate_admin_request(connection_t *con);
void *handle_admin(void *vcon);
void handle_remote_admin(connection_t *con);
void put_admin(connection_t *con);
void add_admin ();
void del_admin ();
void add_ntripcaster_console ();
void describe_admin (const com_request_t *req, const connection_t *admcon);
void write_admin_prompt (const connection_t *admcon);
const char *admin2string (admin_t *adm, char *buf);
void initialize_readline ();
void uninitialize_readline ();
void put_http_admin (connection_t *con);
scheme_t admin_scheme (com_request_t *req);
int admin_write_raw (const com_request_t *req, const char *fmt, ...);
int admin_write (const com_request_t *req, const int message_type, const char *fmt, ...);
int admin_write_line (const com_request_t *req, const int message_type, const char *fmt, ...);
int admin_write_string (const com_request_t *req, const int message_type, const char *buff);
int http_write_string (const com_request_t *req, const int message_type, const char *buff);
#endif

