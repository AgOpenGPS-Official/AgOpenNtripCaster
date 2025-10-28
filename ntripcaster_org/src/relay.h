/* relay.h
 * - Relay function headers
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

#ifndef __NTRIPCASTER_RELAY_H
#define __NTRIPCASTER_RELAY_H

int relay_add_pull_to_list (char *arg);
int relay_add_push_to_list (char *arg);
int relay_insert (relay_t *relay);
void relay_dispose (relay_t *relay);
relay_t *relay_create ();
relay_t *relay_copy (relay_t *old);
void relay_connect_all_relays ();
int relay_connected_or_pending (relay_t *rel);

void *relay_connect_list_item (void *arg);
//connection_t *relay_connect_push (relay_t *relay, int *err);
int relay_pull (com_request_t *comreq, char *arg);
//connection_t *relay_pull_stream (ntrip_request_t *req, int *err);
int relay_connect_pull (relay_t *rel);
void relay_source_login(connection_t *con, relay_t *rel);
int login_as_client_on_server (connection_t *con, relay_t *rel);
int login_as_nontrip_client_on_server (connection_t *con, relay_t *rel);
connection_t *relay_setup_connection (ntrip_request_t *req);

int relay_remove_with_con (connection_t *con);
int relay_remove_with_req (ntrip_request_t *req);
relay_t *relay_find_with_req (ntrip_request_t *req, const char * mount);
relay_t *relay_find_with_con (connection_t *con);

#endif
