/* ntripcaster_resolv.c
 * - General Resolving Functions
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
#include <stdlib.h>
#include <stdarg.h>

#include "definitions.h"

#include <sys/types.h>
#include <ctype.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include <fcntl.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#else
#include <winsock.h>
#endif

#ifdef HAVE_LIBWRAP
# include <tcpd.h>
# ifdef NEED_SYS_SYSLOG_H
#  include <sys/syslog.h>
# else
#  include <syslog.h>
# endif
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "sock.h"
#include "ntripcaster_resolv.h"
#include "log.h"
#include "avl_functions.h"
#include "main.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "memory.h"

#ifndef _WIN32
extern int h_errno, errno;
#endif

extern server_info_t info;
extern struct in_addr localaddr;

struct hostent *
ntripcaster_gethostbyname (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error)
{
  switch (info.resolv_type)
  {
#ifdef SOLARIS_RESOLV_OK
    case solaris_gethostbyname_r_e:
      xa_debug (2, "Resolving %s using solaris reentrant type function", hostname);
      return solaris_gethostbyname_r (hostname, res, buffer, buflen, error);
      break;
#endif
#ifdef LINUX_RESOLV_OK
    case linux_gethostbyname_r_e:
      xa_debug (2, "Resolving %s using linux reentrant type function", hostname);
      return linux_gethostbyname_r (hostname, res, buffer, buflen, error);
      break;
#endif
    case standard_gethostbyname_e:
      xa_debug (2, "Resolving %s using standard nonreentrant type function", hostname);
      return standard_gethostbyname (hostname, res, buffer, buflen, error);
      break;
    default:
      xa_debug (1, "DEBUG: gethostbyname (%s) failed cause no resolv function was defined (%d)", hostname,
          info.resolv_type);
      return NULL;
      break;
  }
}

struct hostent *
ntripcaster_gethostbyaddr (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error)
{
  char outhost[20];
  makeasciihost ((struct in_addr *)host, outhost);

  switch (info.resolv_type)
  {
#ifdef SOLARIS_RESOLV_OK
    case solaris_gethostbyname_r_e:
      xa_debug (2, "Resolving %s using solaris reentrant type function", outhost);
      return solaris_gethostbyaddr_r (host, hostlen, he, buffer, buflen, error);
      break;
#endif
#ifdef LINUX_RESOLV_OK
    case linux_gethostbyname_r_e:
      xa_debug (2, "Resolving %s using linux reentrant type function", outhost);
      return linux_gethostbyaddr_r (host, hostlen, he, buffer, buflen, error);
      break;
#endif
    case standard_gethostbyname_e:
      xa_debug (2, "Resolving %s using standard nonreentrant type function", outhost);
      return standard_gethostbyaddr (host, hostlen, he, buffer, buflen, error);
      break;
    default:
      xa_debug (1, "DEBUG: gethostbyaddr (%s) failed cause no resolv function was defined", outhost);
      return NULL;
      break;
  }
}

#ifdef SOLARIS_RESOLV_OK
struct hostent *
solaris_gethostbyname_r (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error)
{
  *error = 0;

//write_log (LOG_DEFAULT, "!!!!!!!!!!!!!!!! gethostbyname() called !!!!!!!!!!!!!!!!!!"); // ajd

  return gethostbyname_r (hostname, res, buffer, buflen, error);
}
struct hostent *
solaris_gethostbyaddr_r (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error)
{
  *error = 0;
  return gethostbyaddr_r (host, hostlen, AF_INET, he, buffer, buflen, error);
}
#endif

#ifdef LINUX_RESOLV_OK
struct hostent *
linux_gethostbyname_r (const char *hostname, struct hostent *res, char *buffer, int buflen, int *error)
{
  *error = 0;

//write_log (LOG_DEFAULT, "!!!!!!!!!!!!!!!! gethostbyname() called !!!!!!!!!!!!!!!!!!"); // ajd

  if (gethostbyname_r (hostname, res, buffer, buflen, &res, error) >= 0)
    return res;
  else
    return NULL;
}

struct hostent *
linux_gethostbyaddr_r (const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error)
{
  int out;
  *error = 0;
  if ((out = gethostbyaddr_r (host, hostlen, AF_INET, he, buffer, buflen, &he, error) >= 0))
  {
    return he;
  }
  xa_debug (2, "gethostbyaddr_r() returned %d, error is %d", out, *error);
  return NULL;
}
#endif

struct hostent *
standard_gethostbyname(const char *hostname, struct hostent *res, char *buffer, int buflen, int *error)
{
  thread_mutex_lock(&info.resolvmutex);
  *error = 0;

//write_log (LOG_DEFAULT, "!!!!!!!!!!!!!!!!!!! gethostbyname() called !!!!!!!!!!!!!!!!!!!"); // ajd

  res = gethostbyname(hostname);
  if (!res) {
    xa_debug(1, "DEBUG: gethostbyname (%s) failed", hostname);
    *error = errno;
  }
  return res;
}

struct hostent *
standard_gethostbyaddr(const char *host, int hostlen, struct hostent *he, char *buffer, int buflen, int *error)
{
  *error = 0;
  thread_mutex_lock(&info.resolvmutex);
  he = gethostbyaddr(host, hostlen, AF_INET);
  *error = errno;
  return he;
}

void
ntripcaster_clean_hostent()
{
  /* When not using reentrant versions of gethostbyname and his brothers, we lock this
     mutex before calling gethostbyname() and therefor, unlock it here. */
  if (info.resolv_type == standard_gethostbyname_e)
    thread_mutex_unlock (&info.resolvmutex);
}

char *
reverse (const char *host)
{
  struct hostent hostinfo, *hostinfoptr;
  struct in_addr addr;
  int error;
  char *outhost;
  char buffer[BUFSIZE];

  if (!host)
  {
    write_log (LOG_DEFAULT, "ERROR: reverse() called with NULL host");
    return NULL;
  }

  xa_debug (1, "reverse() reverse resolving %s", host);

  if (inet_aton (host, &addr))
  {
    hostinfoptr = ntripcaster_gethostbyaddr((char *) &addr, sizeof (struct in_addr), &hostinfo, buffer, BUFSIZE, &error);

    if (hostinfoptr && hostinfoptr->h_name)
      outhost = nstrdup (hostinfoptr->h_name);
    else
      outhost = NULL;

    ntripcaster_clean_hostent ();
    return outhost;
  }
  else
    return NULL;
}

char *
forward (const char *name, char *target)
{
  struct hostent hostinfo, *hostinfoptr;
  struct sockaddr_in sin;
  char buf[BUFSIZE];
  int error;

  xa_debug (1, "forward() resolving %s", name);

  if (isdigit ((int)name[0]) && isdigit ((int)name[strlen(name) - 1]))
    return NULL; /* No point in resolving ip's */

  hostinfoptr = ntripcaster_gethostbyname (name, &hostinfo, buf, BUFSIZE, &error);

  if (!hostinfoptr)
  {
    ntripcaster_clean_hostent();
    return NULL;
  }

  memset (&sin, 0, sizeof (sin));

  sin.sin_addr.s_addr = *(unsigned long *)hostinfoptr->h_addr_list[0];

  makeasciihost(&sin.sin_addr, target);

  ntripcaster_clean_hostent();

  return target;
}












