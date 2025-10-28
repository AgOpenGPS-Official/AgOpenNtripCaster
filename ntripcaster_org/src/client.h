/* client.h
 * - Client function headers
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

#ifndef __NTRIPCASTER_CLIENT_H
#define __NTRIPCASTER_CLIENT_H

void http_client_login(connection_t *con, ntrip_request_t *req);
void put_client(connection_t *con);
client_t *create_client();
void util_increase_total_clients ();
void util_decrease_total_clients ();
void del_client(connection_t *client, source_t *source);
int client_wants_content_length(connection_t *con);
int client_wants_icy_headers(connection_t *con);
int client_wants_metadata (connection_t *con);
int client_wants_udp_info (connection_t *con);
int client_errors (const client_t *client);
void greet_client(connection_t *con, source_t *source);
void describe_client (const com_request_t *req, const connection_t *clicon);
const char *client_type (const connection_t *clicon);
#endif

























