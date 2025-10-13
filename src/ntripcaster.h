/* ntripcaster.h
 * - Configuration Information
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

#ifndef __NTRIPCASTER_CONFIG_H
#define __NTRIPCASTER_CONFIG_H

#ifdef _WIN32
#include "win32config.h"
#else
#include "config.h"
#endif

#define TIMEOUT 1

#define SERVER_RUNNING 1
#define SERVER_INITIALIZING 2
#define SERVER_DYING 0

/* Flags for the client_login() */
#define LIST_STREAMS -2

/* Flags for the console */
#define CONSOLE_ADMIN_TAIL 0
#define CONSOLE_ADMIN 1
#define CONSOLE_LOG 2
#define CONSOLE_BACKGROUND 3

/* Flags for the admin */
#define NONE 0
#define TAILING 1
#define OPER 2
#define OPER_TAILING 4

/* General return flags */
#define OK 1
#define UNKNOWN -1
#define ICE_ERROR_CONNECT -1
#define ICE_ERROR_INVALID_SYNTAX -2
#define ICE_ERROR_ARGUMENT_REQUIRED -3
#define ICE_ERROR_HEADER -4
#define ICE_ERROR_TRANSMISSION -5
#define ICE_ERROR_DUPLICATE -6
#define ICE_ERROR_FILE -7
#define ICE_ERROR_INVALID_PASSWORD -8
#define ICE_ERROR_NO_SUCH_MOUNT -9
#define ICE_ERROR_NULL -10
#define ICE_ERROR_NOT_FOUND -11
#define ICE_ERROR_INIT_FAILED -12
#define ICE_ERROR_INSERT_FAILED -13
#define ICE_ERROR_NOT_INITIALIZED -14
#define ICE_ERROR_MOUNTPOINT_TAKEN -15

#define DEFAULT_THROTTLE 100
#define DEFAULT_SLEEP_RATIO 0.10
#define DEFAULT_MOUNT_FALLBACK 1
#define DEFAULT_FORCE_SERVERNAME 0
#define DEFAULT_SOURCE_OPTS "istphxcrMRWCT"
#define DEFAULT_BODY_TAG "<body style=\"background-color:white; color:black;\">" /* This is just for the internally created pages.. use templates instead */
#define DEFAULT_ICE_ROOT "."
#define DEFAULT_ACL_POLICY 1 /* 1 means allow, 0 deny */
#define DEFAULT_ALLOW_HTTP_ADMIN 1
#define DEFAULT_CLIENT_TIMEOUT 0
#define DEFAULT_LOOKUPS 0
#define DEFAULT_PORT 2101

#ifdef SOMAXCONN
#define LISTEN_QUEUE SOMAXCONN
#else
#define LISTEN_QUEUE 50
#endif

#define DEFAULT_MAX_CLIENTS 1000
#define DEFAULT_MAX_IP_CONNECTIONS 1000
#define DEFAULT_MAX_CLIENTS_PER_SOURCE 1000
#define DEFAULT_MAX_SOURCES 150
#define DEFAULT_MAX_ADMINS 5
#define DEFAULT_ENCODER_PASSWORD "letmein"
#define DEFAULT_OPER_PASSWORD "breakin"
#define DEFAULT_LOGFILENAME "ntripcaster"
#define DEFAULT_USAGE_FILENAME "usage"
#define DEFAULT_ACCESS_FILENAME "access"
#define DEFAULT_REMOTE_ADMIN_PASSWORD "letmein"
#define DEFAULT_LOG_DIR NTRIPCASTER_LOGDIR
#define DEFAULT_ETC_DIR NTRIPCASTER_ETCDIR
#define DEFAULT_VAR_DIR NTRIPCASTER_VARDIR
#define DEFAULT_TEMPLATE_DIR NTRIPCASTER_TEMPLATEDIR
#define DEFAULT_CONFIG_FILE "ntripcaster.conf"
#define DEFAULT_USER_FILE "users.aut"
#define DEFAULT_GROUP_FILE "groups.aut"
#define DEFAULT_WATCH_FILE "watchdog.check"
#define DEFAULT_PID_FILE "caster.pid"
#define DEFAULT_CLIENT_MOUNT_FILE "clientmounts.aut"
#define DEFAULT_SOURCE_MOUNT_FILE "sourcemounts.aut"
#define DEFAULT_SOURCETABLE_FILE "sourcetable.dat"
#define DEFAULT_STATUSTIME 120
#define DEFAULT_LOCATION "Federal Agency of Cartography and Geodesy"
#define DEFAULT_RP_EMAIL "euref-ip@bkg.bund.de"
#define DEFAULT_URL "https://igs.bkg.bund.de/index_ntrip.htm"
#define DEFAULT_TRANSPARENT_PROXY 0
#define DEFAULT_KICK_RELAYS 0 /* Kick relays after this many seconds without clients */
#define DEFAULT_RELAY_RECONNECT_TIME 60
#define DEFAULT_RELAY_RECONNECT_TRIES -1
#define DEFAULT_KICK_CLIENTS 1
#define DEFAULT_NAME "EUREF"
#define DEFAULT_NTRIP_INFO_URL "http://igs.bkg.bund.de/index_ntrip.htm"
#define DEFAULT_OPERATOR "BKG"
#define DEFAULT_OPERATOR_URL "https://www.bkg.bund.de/"
#define DEFAULT_SESSION_TIMEOUT 300

#define NTRIP_VERSION "2.0"
#undef NTRIP_NUMBER
#undef NTRIP_FIGURE
#define SHORT_CONNECTION 60 // in seconds.
#define MAXNUM_SHORT_CONNECTION 2
#define WATCHDOG_TIME 10 // in seconds.
#define WATCHDOG 1 // undefine if you don't want a watchdog.
#define DAILY_LOGFILES 1

/******************************************************************************/
#define CHANGE1 1 // Relay
#define CHANGE5 1 // Log
/******************************************************************************/

/* Default values for what resolv method to use */
#if defined (SOLARIS) && defined (HAVE_GETHOSTBYNAME_R) && defined (HAVE_GETHOSTBYADDR_R)
# define DEFAULT_RESOLV_TYPE solaris_gethostbyname_r_e
# define SOLARIS_RESOLV_OK 1
#elif defined (LINUX) && defined (HAVE_GETHOSTBYNAME_R) && defined (HAVE_GETHOSTBYADDR_R)
# define DEFAULT_RESOLV_TYPE linux_gethostbyname_r_e
# define LINUX_RESOLV_OK 1
#else
# define DEFAULT_RESOLV_TYPE standard_gethostbyname_e
# define DEFAULT_RESOLV_OK 1
#endif

#define SOCK_UNUSED -1
#define SOCK_SIGNOFF -2

/* Flags for source->connected */
#define SOURCE_KILLED 0
#define SOURCE_CONNECTED 1
#define SOURCE_UNUSED 2
#define SOURCE_PAUSED 3
#define SOURCE_PENDING 4

#define CLIENT_ALIVE 1
#define CLIENT_DEAD 0
#define CLIENT_PAUSED 3
#define CLIENT_UNPAUSED 4
#define CLIENT_MOVE 5

/* Define these if you want a mess on the screen */
#undef DEBUG_MEMORY
#undef DEBUG_MEMORY_MCHECK
#undef DEBUG_MUTEXES
#undef DEBUG_SOCKETS
#undef DEBUG_SLEEP
#undef DEBUG_FULL

/* Flags for write_log () */
#define LOG_DEFAULT 0
#define LOG_USAGE 1
#define LOG_ACCESS 2

#endif
