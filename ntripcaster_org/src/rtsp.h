/* rtsp.h
 * - RTSP function headers
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

#ifndef __NTRIPCASTER_RTSP_H
#define __NTRIPCASTER_RTSP_H

#define INIT_STATE 0
#define READY_STATE 1
#define PLAY_STATE 2
#define POST_STATE 3

#define MAX_SESSIONS 1000

typedef struct rtsp_transport_St {
  char *transport_protocol;
  char *profile;
  char *lower_transport;
  int xcast;
  char *destination;
  int interleaved;
  int append;
  int ttl;
  int layers;
  int port;
  int client_port;
  int server_port;
  char *ssrc;
  char *mode;
} rtsp_transport_t;

void rtsp_client_login(connection_t *con, ntrip_request_t *req);
//int rtsp_read_header(connection_t *con, char *header, request_t *req);
//void rtsp_build_request(char *line, request_t *req);
void *rtsp_client_func(connection_t *con, ntrip_request_t *req);
rtsp_session_t *create_rtsp_session();
void clean_session_tree();
int add_session_no_mutex(rtsp_session_t *session);
rtsp_session_t *find_session(long int id);
//rtsp_session_t *find_session_no_mutex(long int id);
rtsp_session_t *get_new_session(void);
int delete_session(rtsp_session_t *session);
int delete_session_no_mutex(rtsp_session_t *session);
int delete_session_by_id(long int id);
void free_session(rtsp_session_t *session);
int setup_session(rtsp_session_t *session, connection_t *con, ntrip_request_t *req);
connection_t *rtsp_create_client_connection(rtsp_session_t *session);
connection_t *rtsp_create_source_connection(rtsp_session_t *session);
void rtsp_remove_connection_from_session(connection_t *con, long int id);
//void rtsp_build_session_description(rtsp_session_t *session, char *buf);
int rtsp_parse_transport(connection_t *con, rtsp_session_t *session);

int rtsp_options(connection_t *con, ntrip_request_t *req);
int rtsp_describe(connection_t *con, ntrip_request_t *req);
int rtsp_setup(connection_t *con, ntrip_request_t *req);
int rtsp_play(connection_t *con, ntrip_request_t *req);
int rtsp_post(connection_t *con, ntrip_request_t *req);
int rtsp_pause(connection_t *con, ntrip_request_t *req);
int rtsp_get_parameter(connection_t *con, ntrip_request_t *req);
int rtsp_teardown(connection_t *con, ntrip_request_t *req);

#endif
