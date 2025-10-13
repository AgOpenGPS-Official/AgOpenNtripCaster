/* tls.h
 * - TLS Function Headers
 *
 * Copyright (c) 2018
 * German Federal Agency for Cartography and Geodesy (BKG)
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Alberding GmbH https://www.alberding.eu/
 *
 * The BKG disclaims any liability nor responsibility to any person or entity
 * with respect to any loss or damage caused, or alleged to be caused,
 * directly or indirectly by the use and application of the NTRIP technology.
 *
 * For latest information and updates, access:
 * https://igs.bkg.bund.de/ntrip/index
 *
 * BKG, Frankfurt, Germany
 * E-mail: euref-ip@bkg.bund.de
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

#ifndef __NTRIPCASTER_TLS_H
#define __NTRIPCASTER_TLS_H
#ifdef HAVE_TLS

#include "ntripcastertypes.h"

int tls_connect(connection_t *con, const char *host);
void tls_free(connection_t *con);

/* now a bunch of functions cloned from sock.h */

int tls_write_string(SSL *tls, const char *buff);
int tls_write(SSL *tls, const char *fmt, ...);
int tls_write_line(SSL *tls, const char *fmt, ...);
int tls_read_line(SSL *tls, char *buff, const int len);
int tls_read_lines_with_timeout(SSL *tls, char *buff, const int len);
ssize_t tls_recv(SSL *tls, void *buf, size_t len);

#endif
#endif
