/* time.c
 * - Utility Time Functions
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

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#include "definitions.h"
#include <stdio.h>
#include "definitions.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>

#ifdef _WIN32
# include <time.h>
# include <windows.h>
#else
# include <sys/types.h>
# include <time.h>
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# endif
# include <sys/socket.h>
#endif

/* added. ajd */
#ifndef _WIN32
#include <netinet/in.h>
#endif

#include <stdlib.h>

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "log.h"
#include "logtime.h"
#include "memory.h"
#include "ntripcaster_string.h"

extern server_info_t info;

long get_time()
{
  return time(NULL);
}

void get_regular_time(char *s) {
  get_string_time(s, get_time(), REGULAR_TIME);
}

void get_log_time(char *s)
{
  get_string_time(s, get_time(), REGULAR_DATETIME);
}

void get_clf_log_time(char *s)
{
  get_string_time(s, get_time(), CLF_TIME);
}

void get_regular_date(char *s) {
  get_string_time(s, get_time(), REGULAR_DATE);
}

void get_short_date(char *s) {

  get_string_time(s, get_time(), SHORT_DATE);
}

char *get_formatted_time(char *format, char *buf) {
  return get_string_time_buf(get_time(), format, buf);
}

void get_string_time(char *s, time_t tt, char *format)
{
  s[0] = '\0';

#ifdef HAVE_LOCALTIME_R
  {
    struct tm mt, *pmt;

    if (!(pmt = gmtime_r(&tt, &mt))) {
      strcpy (s, "error");
    } else {
      if (strftime(s, 40, format, pmt) == 0)
        strcpy (s, "error");
    }
  }
#else
  {
    struct tm *t;
    /* localtime is NOT threadsafe on all platforms */
    thread_library_lock(); // problem ??????. ajd
    t = gmtime(&tt);
    strftime(s, 40, format, t);
    thread_library_unlock();
  }
#endif

}

char *get_string_time_buf(time_t tt, char *format, char* buff) {
#ifdef HAVE_LOCALTIME_R
  {
    struct tm mt, *pmt;

    if (!(pmt = gmtime_r(&tt, &mt))) {
      strcpy (buff, "error");
    } else {
      if (strftime(buff, 40, format, pmt) == 0)
        strcpy (buff, "error");
    }
  }
#else
  {
    struct tm *t;
    /* localtime is NOT threadsafe on all platforms */
    thread_library_lock();
    t = gmtime(&tt);
    strftime(buff, 40, format, t);
    thread_library_unlock();
  }
#endif

  return buff;
}
