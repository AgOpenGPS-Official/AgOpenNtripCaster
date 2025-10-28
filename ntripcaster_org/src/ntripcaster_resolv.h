/* ntripcaster_resolv.h
 * - General Resolving Function Headers
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

#ifndef __NTRIPCASTER_RESOLV_H
#define __NTRIPCASTER_RESOLV_H

#include "ntripcastertypes.h"

#ifndef _WIN32
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <sys/socket.h>
#endif

/* Resolv functions */
struct hostent *ntripcaster_gethostbyname (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error);
struct hostent *ntripcaster_gethostbyaddr (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error);
void ntripcaster_clean_hostent();
char *reverse (const char *hostname);
char *forward (const char *name, char *buf);

struct hostent *standard_gethostbyname(const char *hostname, struct hostent *res, char *buffer, int buflen, int *error);
struct hostent *standard_gethostbyaddr(const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error);

#ifdef SOLARIS_RESOLV_OK
struct hostent *solaris_gethostbyname_r (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error);
struct hostent *solaris_gethostbyaddr_r (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error);
#endif

#ifdef LINUX_RESOLV_OK
struct hostent *
linux_gethostbyname_r (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error);
struct hostent *linux_gethostbyaddr_r (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error);
#endif

#endif

