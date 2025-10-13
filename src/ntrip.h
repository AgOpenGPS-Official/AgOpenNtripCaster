/* ntrip.h
 * - Ntrip protocol related function headers and definitions
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Informatik Centrum Dortmund http://www.icd.de
 *
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

#ifndef __NTRIP_H
#define __NTRIP_H

// protocols
//#define HTTP 0
//#define RTSP 1
//#define NTRIP1_0 0
//#define NTRIP2_0 1

// reply messages
#define HTTP_GET_STREAM_OK 1
#define HTTP_GET_SOURCETABLE_OK 2
#define HTTP_GET_STREAM_WRONG_MOUNT 3
#define HTTP_GET_NOT_AUTHORIZED 4
#define HTTP_SOURCE_OK 10
#define HTTP_SOURCE_MOUNT_CONFLICT 11
#define HTTP_SOURCE_NOT_AUTHORIZED 12

#define HTTP_BAD_REQUEST 13
#define HTTP_NOT_IMPLEMENTED 14
#define HTTP_SERVICE_UNAVAILABLE 15
#define HTTP_NOT_ACCEPTABLE 16
#define HTTP_FORBIDDEN 17

#define RTSP_OPTIONS_OK 19
#define RTSP_DESCRIBE_OK 20
#define RTSP_SETUP_OK 21
#define RTSP_SETUP_WRONG_MOUNT 22
//#define RTSP_SETUP_NOT_AUTHORIZED 23
#define RTSP_SETUP_MOUNT_CONFLICT 24
#define RTSP_PLAY_OK 25
#define RTSP_PLAY_WRONG_MOUNT 26
#define RTSP_POST_OK 27
#define RTSP_POST_MOUNT_CONFLICT 28
#define RTSP_PAUSE_OK 29
#define RTSP_TEARDOWN_OK 30
#define RTSP_GET_PARAMETER_OK 31

#define RTSP_BAD_REQUEST 40
#define RTSP_NOT_AUTHORIZED 41
#define RTSP_INTERNAL_SERVER_ERROR 42
#define RTSP_AGGREGATE_NOT_ALLOWED 43
#define RTSP_UNSUPPORTED_TRANSPORT 44
#define RTSP_SESSION_NOT_FOUND 45
#define RTSP_METHOD_NOT_VALID 46
#define RTSP_SERVICE_UNAVAILABLE 47
#define RTSP_NOT_IMPLEMENTED 48

#define UDP_GET_STREAM_OK 50
#define UDP_SOURCE_OK 51
/*
typedef struct ntrip_method_St
{
  char *method;
  protocol_t protocol;
  ntripcaster_function *login_func;
  ntripcaster_int_function *execute_func;
} ntrip_method_t;

typedef struct ntrip_request_St {
  ntrip_method_t *method;
  char path[BUFSIZE];
  char host[BUFSIZE];
  int port;
  int cseq;
  long int sessid;
} ntrip_request_t;
*/

typedef struct ntrip_header_element_St
{
  int index;
  char *name;
  char *value;
} ntrip_header_element_t;

typedef struct ntrip_message_St
{
  int type;
  int protocol;
  char *message;
  int code;
  int header_element[10]; // the indices in the ntrip_header_element_t array. ajd
} ntrip_message_t;

ntrip_method_t *get_ntrip_method(char *name, int protocol);
char *get_ntrip_method_string(char *buf);
void ntrip_init();
ntrip_header_element_t *get_header_element(int index);
ntrip_message_t *get_ntrip_message(int type, avl_tree *tree);
void add_header_string(int header_element[], char *buf);
void ntrip_write_message(connection_t *con, int type, ...);
int ntrip_read_header(connection_t *con, char *header, ntrip_request_t *req);
//int ntrip_read_old_source_header(connection_t *con, char *header, ntrip_request_t *req);
http_chunk_t *ntrip_create_http_chunk();
void ntrip_zero_http_chunk(http_chunk_t *hc);

#endif

