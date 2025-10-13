/* sock.h
 * - General Socket Function Headers
 *
 * Copyright (c) 2023
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

#ifndef __NTRIPCASTER_SOCK_H
#define __NTRIPCASTER_SOCK_H

#include "ntripcastertypes.h"

typedef struct ntripcaster_socket_St
{
  SOCKET sock;
  int domain;
  int type;
  int protocol;
  int blocking;
  int keepalive;
  int linger;
  int busy;
} ntripcaster_socket_t;

#ifndef _WIN32
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <sys/socket.h>
#endif

enum blockmode {SOCK_BLOCK=0, SOCK_BLOCKNOT=1};

#define SOCK_READ_LINE_TIMEOUT 5
#define SOCK_READ_LINES_TIMEOUT 2

#define SOCK_READ_TIMEOUT 5

#ifdef _WIN32
int inet_aton(const char *s, struct in_addr *a);
#endif

/* sock connect macro */
#define sock_connect(h, p) sock_connect_wto(h, p, 0)

/* Misc socket functions */
int sock_set_keepalive(SOCKET sockfd, const int keepalive);
int sock_set_no_delay(SOCKET sockfd);
int sock_set_send_buffer(SOCKET sockfd, int size);
int sock_set_no_linger (SOCKET sockfd);
int sock_valid (const SOCKET sockfd);
int sock_set_blocking(SOCKET sockfd, enum blockmode block);
int sock_close(SOCKET sockfd);
SOCKET sock_socket (int domain, int type, int protocol);
SOCKET sock_accept (SOCKET s, struct sockaddr *addr, socklen_t *addrlen);
SOCKET sock_create_udp_socket ();
char *sock_get_local_ipaddress ();
void sock_close_all_sockets ();
void sock_dump_fd (SOCKET s, int ffd, int size);

/* Connection related socket functions */
SOCKET sock_get_server_socket(const int port, int udp);
SOCKET sock_connect_wto(const char *hostname, const int port, const int timeout);

/* Socket write functions */
int sock_write_bytes(SOCKET sockfd, const char *buff, int len);
int sock_read_line_nb (SOCKET sock, char *buff, const int len);
int sock_write_bytes_or_kick (SOCKET sockfd, connection_t *clicon, const char *buff, const int len);
int sock_write(SOCKET sockfd, const char *fmt, ...);
int sock_write_line (SOCKET sockfd, const char *fmt, ...);
int sock_write_string (SOCKET sockfd, const char *buff);
int sock_write_bytes_con(connection_t *con, const char *buff, int len);
int sock_write_con(connection_t *con, const char *fmt, ...);
int sock_write_line_con (connection_t *con, const char *fmt, ...);
int sock_write_string_con (connection_t *con, const char *buff);

/* Socket read functions */
int sock_read_line (SOCKET sockfd, char *string, const int len);
int sock_read_line_with_timeout(SOCKET sockfd, char *buff, const int len);
int sock_read_lines_with_timeout(SOCKET sockfd, char *buff, const int len);

int readable_timeo (int fd, int sec);

SOCKET sock_get_bound_tcp_socket(int port); // nontrip.

/* Libwrap functions */
int sock_check_libwrap(const SOCKET sock, const contype_t contype);
const char *sock_get_libwrap_type (const contype_t contype);

/* rtsp. */
void sock_write_file(FILE *ifp, int fd);
SOCKET sock_get_udp_socket(const char *host, int port);
int get_socket_port(const SOCKET sockfd);

#ifdef DEBUG_SOCKETS
ntripcaster_socket_t *sock_find (SOCKET s);
#endif
#endif







