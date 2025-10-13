/* timer.c
 * - Thread for periodic events
 *
 * Copyright (c) 2022
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
#include <errno.h>

#ifndef __USE_BSD
#  define __USE_BSD
#endif

#ifndef __EXTENSIONS__
# define __EXTENSIONS__
#endif

#include <string.h>

#ifdef HAVE_SYSTEMD_SD_DAEMON_H
#include <systemd/sd-daemon.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <fcntl.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <utime.h>
#endif

#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "ntrip.h"
#include "rtsp.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "threads.h"
#include "timer.h"
#include "logtime.h"
#include "avl_functions.h"
#include "log.h"
#include "sock.h"
#include "memory.h"
#include "client.h"
#include "commands.h"
#include "relay.h"
#include "source.h"

#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

extern int errno;
extern server_info_t info;

void display_stats(statistics_t *stat);

/* Writes the one line status report to the log and the console if needed */
void status_write(server_info_t *infostruct)
{
  char timebuf[BUFSIZE];
  avl_traverser trav = {0};
  connection_t *con;
  char lt[100];
  long filetime;

  get_log_time(lt);

  filetime = read_starttime();
  if (filetime > 0)
    nntripcaster_time(get_time () - filetime, timebuf);
  else
    nntripcaster_time(get_time () - info.server_start_time, timebuf);

  while (is_server_running() && (con = avl_traverse(info.admins, &trav))) {
    if (con->food.admin->status && con->food.admin->alive) {
      if (con->host && ntripcaster_strcmp(con->host, "NtripCaster console") == 0) {
        printf("[%s] [Bandwidth: %fKB/s] [Sources: %ld] [Clients: %ld] [Admins: %ld] [Uptime: %s]\n-> ", lt,
                info.bandwidth_usage, info.num_sources, info.num_clients, info.num_admins,
          timebuf);
        fflush(stdout);
      } else {
        if (con->food.admin->scheme != tagged_scheme_e) {
          sock_write_line(con->sock, "[%s] [Bandwidth: %fKB/s] [Sources: %ld] [Clients: %ld] [Admins: %ld] [Uptime: %s]",
             lt, info.bandwidth_usage, info.num_sources, info.num_clients, info.num_admins,
             timebuf);
        } else {
          sock_write_line(con->sock, "M%d [%s] [Bandwidth: %fKB/s] [Sources: %ld] [Clients: %ld] [Admins: %ld] [Uptime: %s]",
             ADMIN_SHOW_STATUS, lt, info.bandwidth_usage, info.num_sources, info.num_clients, info.num_admins,
             timebuf);
        }

        sock_write(con->sock, "-> ");
      }
    }
  }

  write_log(LOG_USAGE, "Bandwidth:%fKB/s Sources:%ld Clients:%ld Admins:%ld", info.bandwidth_usage, info.num_sources, info.num_clients, info.num_admins);
}

/* Starts up the calendar thread.  the calendar thread is responsible
 * for directory server updates, cron jobs and such
 */
void *startup_timer_thread(void *arg)
{
  time_t justone = 0, trottime = 0;
  statistics_t trotstat;
  mythread_t *mt;

  thread_init();

  mt = thread_get_mythread();

  while (thread_alive (mt)) {
    time_t stime = get_time();

    timer_handle_status_lines (stime);

    timer_handle_transfer_statistics (stime, &trottime, &justone, &trotstat);

#ifdef CHANGE5
#ifdef DAILY_LOGFILES
    timer_check_date(); // start_new_day in handle_transfer_statistics?
#endif
#endif

    if (mt->ping == 1) mt->ping = 0;

    my_sleep(400000);
  }

  /* We don't know if we even got here, cause a thread_cancel() might have
     killed us, so closing files and directories here is pointless */
  thread_exit(7);
  return NULL;
}

void
timer_handle_status_lines (time_t stime)
{
  if ((stime - info.statuslasttime) >= info.statustime) {
    info.statuslasttime = stime;
    status_write(&info);
  }
}

void
timer_handle_transfer_statistics (time_t stime, time_t *trottime, time_t *justone, statistics_t *trotstat)
{
  /* We keep the running statistics on a per hour basis.
     Every hour the daily statistics get updated, and
     we start over. Every day, the total statistics get
     updated, and we start over for the day */
  if (get_time() != *justone) {
    *justone = get_time();

    /* Daily */
    if ((stime % 86400) == 0) {
      statistics_t stat, hourlystats;

      zero_stats(&stat);

      get_hourly_stats(&hourlystats);
      zero_stats(&info.hourly_stats);
      update_daily_statistics(&hourlystats);
      write_hourly_stats(&stat);

      get_daily_stats(&stat);
      zero_stats(&info.daily_stats);
      update_total_statistics(&stat);
      write_daily_stats(&stat);
    } else if ((stime % 3600) == 0) {  /* hourly */
      statistics_t stat;

      zero_stats(&stat);

      get_hourly_stats(&stat);
      zero_stats(&info.hourly_stats);
      update_daily_statistics(&stat);
      write_hourly_stats(&stat);
    }

    if ((stime % 60) == 0) { /* Every 60 seconds */
      time_t delta;
      statistics_t stat;
      unsigned int total_bytes;

      double KB_per_sec = 0;

      zero_stats(&stat);

      get_running_stats(&stat);

      if (*trottime == 0) {
        *trottime = get_time();
        get_running_stats(trotstat);
      } else {
        total_bytes = (stat.read_kilos - trotstat->read_kilos) + (stat.write_kilos - trotstat->write_kilos);
        delta = get_time() - *trottime; /* Should be about 60 unless weird stuff is going on */
        if (delta <= 0) {
          write_log(LOG_DEFAULT,
            "ERROR: Losing track of time.. is it xmas already? [%d - %d == %d <= 0]",
            get_time (), *trottime, delta);
        } else {
          KB_per_sec = (double)total_bytes / (double)delta;


          /* This is just me being paranoid, sometimes this value gets all fucked up for a while
             and will make the server refuse connects. */
          if (KB_per_sec < 40000000) {
            info.bandwidth_usage = KB_per_sec;
            if (!info.throttle_on && (info.throttle > 0.0) && (KB_per_sec > info.throttle)) {
              write_log(LOG_DEFAULT, "Throttling bandwidth: [Usage %f, specified throttle value: %f]",
                  KB_per_sec, info.throttle);
              info.throttle_on = 1;
            } else if (info.throttle_on && (KB_per_sec < info.throttle)) {
              write_log(LOG_DEFAULT, "Bandwidth [%f] back below limit [%f], allowing access", KB_per_sec,
                  info.throttle);
              info.throttle_on = 0;
            }
          }
        }

        get_running_stats(trotstat);
        *trottime = get_time();
      }
    }
  }
}

void
timer_kick_abandoned_relays (time_t stime)
{
  if ((stime % 100 == 0) && info.kick_relays) {
    avl_traverser trav = {0};
    connection_t *sourcecon;

    thread_mutex_lock(&info.source_mutex);
    while ((sourcecon = avl_traverse(info.sources, &trav))) {
      if (sourcecon->food.source->type == pulling_source_e &&
          sourcecon->food.source->num_clients <= 0 &&
          sourcecon->food.source->connected == SOURCE_CONNECTED
          && (get_time () - sourcecon->connect_time > info.kick_relays)) {

        kick_connection(sourcecon, "Closing relay (saving bandwidth)");
      }
    }
    thread_mutex_unlock(&info.source_mutex);
  }
}

void get_hourly_stats(statistics_t *stat)
{
  internal_lock_mutex (&info.misc_mutex);
  stat->read_bytes = info.hourly_stats.read_bytes;
  stat->write_bytes = info.hourly_stats.write_bytes;
  internal_unlock_mutex (&info.misc_mutex);

  stat->read_kilos = info.hourly_stats.read_kilos;
  stat->write_kilos = info.hourly_stats.write_kilos;

  stat->client_connections = info.hourly_stats.client_connections;
  stat->source_connections = info.hourly_stats.source_connections;
  stat->client_connect_time = info.hourly_stats.client_connect_time;
  stat->source_connect_time = info.hourly_stats.source_connect_time;
}

void write_hourly_stats(statistics_t *stat)
{
  char cct[BUFSIZE], sct[BUFSIZE];
  char timebuf[BUFSIZE];
  statistics_t running;

  get_current_stats(&running);
  add_stats(stat, &running, 0);

  strncpy(cct, connect_average (stat->client_connect_time, stat->client_connections + info.num_clients, timebuf), BUFSIZE);
  cct[BUFSIZE-1] = 0;
  strncpy(sct, connect_average (stat->source_connect_time, stat->source_connections + info.num_sources, timebuf), BUFSIZE);
  sct[BUFSIZE-1] = 0;

  write_log(LOG_USAGE, "Hourly statistics: [Client connects: %lu] [Source connects: %lu] [Bytes read: %lu] [Bytes written: %lu]",
       stat->client_connections, stat->source_connections, stat->read_bytes, stat->write_bytes);
  write_log(LOG_USAGE, "Hourly averages: [Client transfer: %lu bytes] [Source transfer: %lu] [Client connect time: %s] [Source connect time: %s]",
       transfer_average (stat->write_bytes, stat->client_connections), transfer_average (stat->read_bytes, stat->source_connections),
       cct, sct);
}

void update_daily_statistics(statistics_t *stat)
{
  thread_mutex_lock(&info.misc_mutex);
  info.daily_stats.read_bytes += (stat->read_bytes / 1000);
  info.daily_stats.write_bytes += (stat->write_bytes / 1000);
  info.daily_stats.client_connections += stat->client_connections;
  info.daily_stats.source_connections += stat->source_connections;
  info.daily_stats.client_connect_time += stat->client_connect_time;
  info.daily_stats.source_connect_time += stat->source_connect_time;
  thread_mutex_unlock(&info.misc_mutex);
}

void get_daily_stats (statistics_t *stat)
{
  thread_mutex_lock(&info.misc_mutex);
  stat->read_bytes = info.daily_stats.read_bytes;
  stat->write_bytes = info.daily_stats.write_bytes;
  stat->client_connections = info.daily_stats.client_connections;
  stat->source_connections = info.daily_stats.source_connections;
  stat->client_connect_time = info.daily_stats.client_connect_time;
  stat->source_connect_time = info.daily_stats.source_connect_time;
  thread_mutex_unlock(&info.misc_mutex);
}

void update_total_statistics(statistics_t *stat)
{
  thread_mutex_lock(&info.misc_mutex);
  info.total_stats.read_bytes += (stat->read_bytes / 1000);
  info.total_stats.read_kilos += (stat->read_bytes);

  info.total_stats.write_bytes += (stat->write_bytes / 1000);
  info.total_stats.write_kilos += (stat->write_bytes);

  info.total_stats.client_connections += stat->client_connections;
  info.total_stats.source_connections += stat->source_connections;
  info.total_stats.client_connect_time += stat->client_connect_time;
  info.total_stats.source_connect_time += stat->source_connect_time;
  thread_mutex_unlock(&info.misc_mutex);
}

void write_daily_stats(statistics_t *stat)
{
  char cct[BUFSIZE], sct[BUFSIZE];
  statistics_t running;
  char timebuf[BUFSIZE];

  get_current_stats(&running);
  add_stats(stat, &running, 0);

  strncpy(cct, connect_average (stat->client_connect_time, stat->client_connections + info.num_clients, timebuf), BUFSIZE);
  cct[BUFSIZE-1] = 0;
  strncpy(sct, connect_average (stat->source_connect_time, stat->source_connections + info.num_sources, timebuf), BUFSIZE);
  sct[BUFSIZE-1] = 0;

  write_log(LOG_USAGE, "Daily statistics: [Client connects: %lu] [Source connects: %lu] [Kbytes read: %lu] [Kbytes written: %lu]",
       stat->client_connections, stat->source_connections, stat->read_bytes, stat->write_bytes);
  write_log(LOG_USAGE, "Daily averages: [Client transfer: %lu Kbytes] [Source transfer: %lu Kbytes] [Client connect time: %s] [Source connect time: %s]",
       transfer_average (stat->write_bytes, stat->client_connections), transfer_average (stat->read_bytes, stat->source_connections),
       cct, sct);
}

void get_current_stats(statistics_t *stat)
{
  get_current_stats_proc (stat, 1);
}

void get_current_stats_proc (statistics_t *stat, int lock)
{
  time_t ec = 0, cc = 0;

  zero_stats(stat);

  /* Lock the double mutex whenever you're about to lock twice */
  if (lock) thread_mutex_lock(&info.double_mutex);

  thread_mutex_lock(&info.source_mutex);
  ec = (time_t)tree_time(info.sources);
  thread_mutex_unlock(&info.source_mutex);

  thread_mutex_lock(&info.client_mutex);
  cc = (time_t)tree_time(info.clients);
  thread_mutex_unlock(&info.client_mutex);

  if (lock) thread_mutex_unlock(&info.double_mutex);

  stat->client_connect_time = cc;
  stat->source_connect_time = ec;
}

void get_running_stats(statistics_t *stat)
{
  get_running_stats_proc (stat, 1);
}

void get_running_stats_nl (statistics_t *stat)
{
  get_running_stats_proc (stat, 0);
}

void get_running_stats_proc (statistics_t *stat, int lock)
{
  statistics_t bufstat;

  /* in megabytes. */
  stat->read_bytes = info.total_stats.read_bytes;
  stat->write_bytes = info.total_stats.write_bytes;
  //stat->read_megs = info.total_stats.read_megs;
  //stat->write_megs = info.total_stats.write_megs;

  /*in kilobytes. */
  stat->read_kilos = info.total_stats.read_kilos;
  stat->write_kilos = info.total_stats.write_kilos;

  stat->client_connections = info.total_stats.client_connections;
  stat->source_connections = info.total_stats.source_connections;
  stat->client_connect_time = info.total_stats.client_connect_time;
  stat->source_connect_time = info.total_stats.source_connect_time;

        /* These in bytes */
  get_current_stats_proc (&bufstat, lock);
  add_stats(stat, &bufstat, 0);

  /* These in bytes */
  get_hourly_stats(&bufstat);
  add_stats(stat, &bufstat, 0);

  /* These in kilobytes */
  get_daily_stats(&bufstat);
  add_stats(stat, &bufstat, 1000);
}

void zero_stats(statistics_t *stat)
{
  if (!stat) {
    write_log (LOG_DEFAULT, "WARNING: zero_stats() called with NULL stat pointer");
    return;
  }

  stat->read_bytes = 0;
  stat->read_kilos = 0;

  stat->write_bytes = 0;
  stat->write_kilos = 0;

  stat->client_connections = 0;
  stat->source_connections = 0;
  stat->client_connect_time = 0;
  stat->source_connect_time = 0;
}

void add_stats(statistics_t *target, statistics_t *source, unsigned long int factor)
{
  double div;

  if (factor == 0)
    div = 1000000.0;
  else
    div = (1000000.0 / (double)factor);

  target->read_bytes += (unsigned long)(source->read_bytes / div);
  target->read_kilos += (unsigned long)(source->read_bytes / (div / 1000));

  target->write_bytes += (unsigned long)(source->write_bytes / div);
  target->write_kilos += (unsigned long)(source->write_bytes / (div / 1000));

  target->client_connections += source->client_connections;
  target->client_connect_time += source->client_connect_time;
  target->source_connections += source->source_connections;
  target->source_connect_time += source->source_connect_time;
}

void display_stats(statistics_t *stat)
{
  xa_debug(1, "DEBUG: rb: %lu wb: %lu", stat->read_bytes, stat->write_bytes);
}

void *startup_relay_connector_thread(void *arg)
{
  mythread_t *mt;

  if (!info.relays) {
    write_log (LOG_DEFAULT, "WARNING: startup_relay_connector_thread(): info.relays is NULL, weird!");
  }

  thread_init();

  mt = (mythread_t *) thread_get_mythread ();

  while (thread_alive (mt))
  {
    relay_connect_all_relays ();
    my_sleep ((info.relay_reconnect_time / 2) * 1000000);

    if (mt->ping == 1)
      mt->ping = 0;
  }

  thread_exit (2);
  return NULL;
}

void *startup_heartbeat_thread(void *arg)
{
  thread_init();

#ifdef NTRIP_NUMBER
  xa_debug(1, "DEBUG: Server is optimized, can't use heartbeat thread");
  thread_exit(0);
#endif

  /* This might do something one day..
     Problem is adding a time variable to every mutex and a call to time(NULL)
     for every lock/unlock, would create a big overhead.
     An alternative would be to check the thread_id on every lock 3 times every
     MAX_MUTEX_LOCKTIME seconds, and if the thread_id is the same all thread
     checks, then it is probably safe to presume it is deadlocked */

    thread_exit(0);
  return 0;
}

void *startup_watchdog_thread(void *arg)
{
  mythread_t *mt;
  char watchdog[BUFSIZE];

  thread_init();

  mt = thread_get_mythread();

  get_ntripcaster_file(info.watchfile, var_file_e, R_OK, watchdog);

  while (thread_alive (mt)) {
    if (info.main_thread->ping == 0) {
      utime(watchdog, NULL);
#ifdef USE_SYSTEMD
      sd_notify(0, "WATCHDOG=1");
#endif
    }
    info.main_thread->ping = 1;
    my_sleep(WATCHDOG_TIME * 1000000);
  }

  thread_exit(0);
  return 0;
}

void add_fmt_string(char *buf, const char *fmt, char *val)
{
  char buf2[256];
  if (!buf || !fmt || !val)
    return;

  if (ntripcaster_strlen (val) > 230)
    val[230] = '\0';

  snprintf(buf2, 256, fmt, val);
  strncat(buf, buf2, 1023 - ntripcaster_strlen (buf));
}

void add_fmt_int(char *buf, const char *fmt, long int val)
{
  char buf2[256];
  if (!buf || !fmt)
    return;
  snprintf(buf2, 256, fmt, val);
  strncat(buf, buf2, 1023 - ntripcaster_strlen (buf));
}

void
timer_check_date() {

  char today[50];

  get_short_date(today);

  if (strncmp(info.date, today, 6) != 0) {
    start_new_day();
  }
}
