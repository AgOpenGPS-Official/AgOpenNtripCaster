/* vsnprintf.c
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

#include <config.h>

#ifndef HAVE_VSNPRINTF
#include <strings.h>
#include <stdarg.h>

int vsnprintf(char* s, int n, char* fmt, va_list stack)
{
  char *f, *sf = 0;
  int i, on, argl = 0;
  char myf[10], buf[20];
  char *arg, *myfp;

  on = n;
  f = fmt;
  arg = 0;
  while (arg || (sf = index(f, '%')) || (sf = f + strlen(f))) {
    if (arg == 0) {
      arg = f;
      argl = sf - f;
    }
    if (argl) {
      i = argl > n - 1 ? n - 1 : argl;
      strncpy(s, arg, i);
      s += i;
      n -= i;
      if (i < argl) {
        *s = 0;
        return on;
      }
    }
    arg = 0;
    if (sf == 0)
      continue;
    f = sf;
    sf = 0;
    if (!*f)
      break;
    myfp = myf;
    *myfp++ = *f++;
    while (((*f >= '0' && *f <='9') || *f == '#')
           && myfp - myf < 8)
    {
      *myfp++ = *f++;
    }
    *myfp++ = *f;
    *myfp = 0;
    if (!*f++)
      break;
    switch(f[-1])
    {
    case '%':
      arg = "%";
      break;
    case 'c':
    case 'o':
    case 'd':
    case 'x':
      i = va_arg(stack, int);
      snprintf(buf, 20, myf, i);
      arg = buf;
      break;
    case 's':
      arg = va_arg(stack, char *);
      if (arg == 0)
        arg = "NULL";
      break;
    default:
      arg = "";
      break;
    }
    argl = strlen(arg);
  }
  *s = 0;
  return on - n;

  va_end(stack);
}
#endif /* !HAVE_VSNPRINTF */


/*
 * Local Variables:
 * c-file-style: "python"
 * End:
 */
