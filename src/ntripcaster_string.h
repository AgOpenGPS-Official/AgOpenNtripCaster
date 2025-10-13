/* string.h
 * - Headers for the string functions
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

#ifndef __NTRIPCASTER_ICE_STRING_H
#define __NTRIPCASTER_ICE_STRING_H

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif

typedef struct
{
  char c;
  char sp[3];
} http_special_char_t;

char *splitc(char *first, char *rest, const char divider);
char *splitnc (char *buf, char *string, const char divider, int n);
char *clean_string(char *string);
void flags2string(admin_t *adm, void *param);
int is_pattern (const char *string);
int is_number (const char *string);
const char *con_host (connection_t *con);
char *my_strdup (const char *string);
char *util_base64_encode(char *message);
char *util_base64_decode(char *message);
char *safe_strcat (char *dest, const char *src, unsigned int maxsize);
char *mutex_to_string (mutex_t *mutex, char *out);
const char *skip_after (const char *ptr, const char *search);
const char *skip_before (const char *ptr, const char *search);
char *create_malloced_ascii_host(struct in_addr *in);
char *makeasciihost(const struct in_addr *in, char *buf);
char *nntripcaster_time(unsigned long int seconds, char *buf);
char *ntripcaster_sprintf (const char *template, const char *arg);
int ntripcaster_strcasecmp (const char *s1, const char *s2);
int ntripcaster_strncmp (const char *s1, const char *s2, size_t n);
int ntripcaster_strcmp (const char *s1, const char *s2);
char *ntripcaster_strstr (const char *haystack, const char *needle);
size_t ntripcaster_strlen (const char *string);
char *nntripcaster_time_minutes (unsigned long int minutes, char *buf);
void catsnprintf (char *line, size_t sz, const char *fmt, ...);
const char *nullcheck_string (const char *string);
void decode_url_string(char *string);
char *get_closing_parenthesis(char *c);
char *slashalize(char *s, int size);
char *create_random_mountpoint(int len);

#endif
