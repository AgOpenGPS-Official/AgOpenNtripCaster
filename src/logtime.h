/* time.h
 * - Utility Time Function Headers
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

#ifndef __NTRIPCASTER_TIME_H
#define __NTRIPCASTER_TIME_H

#define CLF_TIME "%d/%b/%Y:%H:%M:%S %z"
#define REGULAR_DATETIME "%d/%b/%Y:%H:%M:%S"
#define REGULAR_TIME "%H:%M:%S"
#define REGULAR_DATE "%d/%b/%Y"
#define SHORT_DATE "%y%m%d"
//#define HEADER_TIME "%d/%b/%Y:%H:%M:%S GMT"
#define HEADER_TIME "%a, %d %b %Y %H:%M:%S %Z"

long get_time();
void get_regular_time(char *s);
void get_log_time(char *s);
void get_regular_date(char *s);
void get_short_date(char *s);
char *get_formatted_time(char *format, char *buf);
void get_string_time (char *s, time_t tt, char *format);
char *get_string_time_buf(time_t tt, char *format, char* buff);
void get_clf_log_time (char *s);

#endif
