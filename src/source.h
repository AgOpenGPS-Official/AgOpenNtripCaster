/* source.h
 * - Source Function Headers
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

#ifndef __NTRIPCASTER_SOURCE_H
#define __NTRIPCASTER_SOURCE_H

source_t *create_source();
void http_source_login(connection_t *con, ntrip_request_t *req);
int authenticate_source_request(connection_t *con, ntrip_request_t *req);
void *source_rtsp_function(void *conarg);
void kick_source(source_t *sor, char *why);
void add_global_stats(source_t *sor);
void *source_func(void *con);
void put_source(connection_t *con);
void add_source ();
void del_source ();
connection_t *find_mount(char *mount);
connection_t *find_mount_with_req (ntrip_request_t *req, alias_t **wasalias);
connection_t *get_default_mount();
void add_chunk (connection_t *sourcecon);
void write_chunk (source_t *source, connection_t *clicon);
void kick_trailing_clients(source_t *source);
void kick_clients_on_cid(source_t *source);
void kick_dead_clients (source_t *source);
//void move_clients_to_default_mount (connection_t *con);
//int originating_id (connection_t *sourcecon, char *dshost);
//int write_data (connection_t *clicon, source_t *source);
//int write_data_with_metadata (connection_t *clicon, source_t *source, int justnull);
//void move_client (connection_t *clicon, source_t *source, int meta_at);
int finish_meta_frame (connection_t *clicon);
connection_t *get_source_with_mount (const char *mount);
connection_t *get_source_from_host (connection_t *con);
void describe_source (const com_request_t *req, const connection_t *sourcecon);
//const char *sourceproto_to_string (protocol_t proto);
const char *source_type(const connection_t *con);
int start_chunk (source_t *source);
connection_t *get_twin_mount (source_t *scon);
connection_t *get_twin_mount_wl (source_t *scon);
void move_to_smaller_twin (source_t *source, connection_t *clicon);
void source_write_to_client (source_t *source, connection_t *clicon);
void source_get_new_clients (source_t *source);
int source_get_id (char *arg);
void add_nontrip_source(char *line); // nontrip. ajd
int source_fill_chunks(source_t *source, const char *buf, int len);
#endif

