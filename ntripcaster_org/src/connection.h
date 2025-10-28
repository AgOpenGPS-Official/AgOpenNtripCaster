/* connection.h
 * - Connection Function Headers
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

#ifndef __NTRIPCASTER_CONNECTION_H
#define __NTRIPCASTER_CONNECTION_H

void *handle_connection(void *data);
connection_t *get_connection(int *sock);
connection_t *create_connection();
void describe_connection (const com_request_t *req, const connection_t *describecon);
const char *get_user_agent (connection_t *con);
const char *get_source_agent (connection_t *con);
void build_con_line_with_opts (connection_t *con, char *line, int *opt, int maxlen);
void build_source_con_line_with_opts (connection_t *con, char *line, int *opt, int maxlen);

/* nontrip. ajd */
connection_t *get_nontrip_connection();
void *handle_nontrip_connection(void *arg);

#endif
