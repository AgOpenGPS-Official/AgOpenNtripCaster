/* main.c
 * - Main Program
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

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#ifdef HAVE_SYSTEMD_SD_DAEMON_H
#include <systemd/sd-daemon.h>
#endif

#include "definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "definitions.h"

#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# endif
#else
#include <winsock.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcaster.h"
#include "ntripcastertypes.h"
#include "avl_functions.h"
#include "ntripcaster_resolv.h"
#include "sock.h"
#include "log.h"
#include "commands.h"
#include "logtime.h"

#include "main.h"
#include "ntrip.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "commandline.h"
#include "admin.h"
#include "source.h"
#include "sourcetable.h"
#include "rtsp.h"
#include "rtp.h"
#include "client.h"
#include "connection.h"
#include "threads.h"
#include "timer.h"
#include "memory.h"
#include "relay.h"
#include "authenticate/basic.h"
#include "pool.h"
#include "interpreter.h"
#include "match.h"

#ifndef _WIN32
#include <signal.h>
#endif

/* We need this for perror and for various sanity checks */
extern int errno;

/* Importing a tree and mutex from sock.c */
extern avl_tree *sock_sockets;
extern mutex_t sock_mutex;

/* This is global, doh! */
server_info_t info;
struct in_addr localaddr;

#ifdef HAVE_TLS
#if OPENSSL_VERSION_NUMBER < 0x10100000L
static pthread_mutex_t *lock_cs;
void pthreads_locking_callback(int mode, int type, char *file, int line)
{
  if(mode & CRYPTO_LOCK)
    pthread_mutex_lock(&(lock_cs[type]));
  else
    pthread_mutex_unlock(&(lock_cs[type]));
}

unsigned long pthreads_thread_id(void)
{
  return (unsigned long)pthread_self();
}
#endif
#endif /* HAVE_TLS */

/*
 * The world starts here.
 * - First initialize the run path and
 *   some structures and mutex locks needed for
 *   debugging output and other nifty features.
 * - Then set the default values for the info struct, which is basicly
 *   just a pot to keep all the server global variables in. The default
 *   values are taken from ntripcaster.h
 * - Then trap some system signals so they don't interfere with the server
 *   and allocate some extra resources.
 * - Run sanity_check() to make sure everything is ok.
 * - Parse the configuration file (ntripcaster.conf by default). This overrides
 *   any of the settings from ntripcaster.h
 * - Parse the command line options, which in turn overrides the configuration
 *   file options.
 * - Initialize some network stuff
 * - Leave the rest to startup_mode()
 *
 * Assert Class: 1
 */

int main (int argc, char **argv) {
  /* If defined, start the memory checker */
  initialize_memory_checker ();

#ifdef HAVE_TLS
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_library_init();
  SSL_load_error_strings();
  if((lock_cs = OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t))))
  {
    int i;
    for (i = 0; i < CRYPTO_num_locks(); i++)
      pthread_mutex_init(&(lock_cs[i]), NULL);
    CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
    CRYPTO_set_locking_callback((void (*)())pthreads_locking_callback);
  }
#endif
#endif /* HAVE_TLS */

  /* Setup run path and initialize memory debugging (if set) */
  set_run_path (argv);

  /* Initialize the system library mutex */
  thread_lib_init ();

  /* Need to create a new thread entry */
  init_thread_tree (__LINE__, __FILE__);

  /* Set all server variables to a default value */
  setup_defaults ();

  /* Trap some signals */
  setup_signal_traps ();

  /* Allocate client slots, admin slots, source slots, etc */
  allocate_resources ();

  /* Check for -d arguments, which will make us read the config file in another directory */
  parse_directory_args_only (argc, argv);

  /* Override the default values with the ones defined in this configfile */
  parse_default_config_file ();

  /* Read sourcetable.dat and build avl tree. ajd */
  read_sourcetable();

  /* Override them again, with the values from the command line */
  parse_args (argc, argv);

  /* Initialize some platform dependant network stuff */
  initialize_network ();

  /* Initialize the interpreter (if configured) */
  interpreter_init ();

  /* Parse all authentication files */
  init_authentication_scheme ();

  /* Initialize protocol messages. rtsp. ajd */
  ntrip_init();

  /* Make sure we can write to current directory, and other sanity checks */
  sanity_check ();

  /* Print header, select console mode, start the main loop */
  startup_mode (); /* Never returns */

  /* And we're done.. this will never happen */
  return 0;
}

/* A process can only open a certain number of filedescriptors (files/sockets),
   usually something like 64. We call setrlimit() and request a larger limit.
   If the kernel allows it it will increase the number of open file descriptors
   for this process. The kernel has a limit too, often set when you compile
   the kernel, or in /proc/sys/fs/file-max or something similar */
#ifndef _WIN32
void
increase_maximum_number_of_open_files()
{
#if defined(HAVE_SETRLIMIT) && defined (HAVE_GETRLIMIT)
  struct rlimit before, after;

  if (getrlimit (RLIMIT_NOFILE, &before) == 0) {
    xa_debug (1, "DEBUG: Max number of open files: soft: %d hard: %d",
        (int)before.rlim_cur, (int)before.rlim_max);
  } else {
    xa_debug (1, "WARNING: getrlimit() failed.");
    return;
  }

  after.rlim_cur = info.max_clients + info.max_sources + info.max_admins + 20;
  after.rlim_max = before.rlim_max > after.rlim_cur ? before.rlim_max : after.rlim_cur;

  if (setrlimit (RLIMIT_NOFILE, &after) == 0)
  {
    xa_debug (1, "DEBUG: Max number of open files raised from: soft %d hard: %d, to soft: %d hard: %d", before.rlim_cur, before.rlim_max, after.rlim_cur, after.rlim_max);
  } else {
    write_log (LOG_DEFAULT, "ERROR: Increasing maximum number of open files from %d:%d to: %d:%d failed, try lowering the maximum values for listeners, admins, and sources.", before.rlim_cur, before.rlim_max, after.rlim_cur, after.rlim_max);
    write_log (LOG_DEFAULT, "WARNING: The server will run out of file descriptors before reaching the specified limits!");
  }
#endif
}
#endif

void
initialize_network()
{
#ifdef _WIN32
  WSADATA wsad;

  /* Initialize Winsock*/
  WSAStartup(0x0101, &wsad);
#else
  /* On some systems, we need to increase the max number of open files. */
  increase_maximum_number_of_open_files();

  /* Create a tcp socket for DNS queries that stays open */
  sethostent(1);
#endif
}

/* Print header, select the console mode, and start the main server loop. */
void
startup_mode()
{
  pid_t icepid;
  char pathandfile[BUFSIZE];
  int fd;

//  redate_logfilenames(); // added. ajd

  /* Try to open the ntripcaster log files */
  open_log_files();

//  /* open new client/source logfile. ajd */
//  clear_logfile(info.clisrcfilename);

  /* Write startup information in the log file, and print a nice header on stdout */
  write_ntripcaster_header();

  /* Print some information available only in runtime */
  show_runtime_configuration();

  /* Set the running flag */
  set_server_running(SERVER_RUNNING);

  /* Select how the server should be started, i.e how we should use the console */
#ifdef _WIN32
  if ((info.console_mode == CONSOLE_ADMIN) || (info.console_mode == CONSOLE_ADMIN_TAIL))
  {
    add_ntripcaster_console();
  } else {
    write_log(LOG_DEFAULT, "Using stdout as NtripCaster logging window [%i]", info.console_mode);
  }
#else
  if (info.console_mode == CONSOLE_BACKGROUND)
  {
    server_detach();
  } else if ((info.console_mode == CONSOLE_ADMIN) || (info.console_mode == CONSOLE_ADMIN_TAIL)) {
    add_ntripcaster_console();
  } else {
    write_log(LOG_DEFAULT, "Using stdout as NtripCaster logging window");
  }
#endif

  icepid = getpid();
  get_ntripcaster_file(info.pidfile, var_file_e, W_OK, pathandfile);
  fd = open_for_writing(pathandfile);
  if (fd < 0)
    write_log(LOG_DEFAULT, "WARNING: Could not open caster pid file '%s'!", pathandfile);
  else {
    fd_write(fd, "%d", icepid);
    fd_close(fd); // added. ajd
  }
#ifdef USE_SYSTEMD
  {
    char text[40];
    snprintf(text, sizeof(text), "MAINPID=%d", icepid);
    text[sizeof(text)-1] = 0; /* ensure zero termination */
    sd_notify(0, text);
  }
#endif

// added. ajd
#ifdef WATCHDOG
  get_ntripcaster_file(info.watchfile, var_file_e, W_OK, pathandfile);
  fd = open_for_writing(pathandfile);
  if (fd < 0)
    write_log(LOG_DEFAULT, "WARNING: Could not open watchdog file '%s'!", pathandfile);
  else
    fd_close(fd); // added. ajd
#ifdef USE_SYSTEMD
  {
    char text[40];
    sd_notify(0, "READY=1");
    snprintf(text, sizeof(text), "WATCHDOGUSEC=%d", WATCHDOG_TIME * 1500000); /* value*1.5 */
    text[sizeof(text)-1] = 0; /* ensure zero termination */
    sd_notify(0, text);
    sd_notify(0, "WATCHDOG=1");
  }
#endif
#endif

//  ntrip_init(); // rtsp. ajd

  threaded_server_proc(&info); /* Never returns */
}

/* We need to trap some signals.
   SIGCHLD to avoid zombies
   SIGHUP to close/open logfiles
   SIGINT, SIGTERM to close down cleanly.
*/
void
setup_signal_traps()
{
  xa_debug (1, "DEBUG: Activating signal handler");

#ifdef _WIN32
  if (!SetConsoleCtrlHandler( win_sig_die, 1 ))
    write_log(LOG_DEFAULT, "FAILED setting up win32 signal handler");
#else

# if (defined(SYSV) && !defined(hpux)) || defined(SVR4)
#  define signal sigset
# endif

  signal(SIGHUP, sig_hup);
  signal(SIGINT, sig_die);
  signal(SIGTERM, sig_die);
  signal(SIGCHLD, sig_child);
  signal(SIGPIPE, SIG_IGN);
#endif
}

/* Set all global variables to their default values */
void setup_defaults() {
  int i;

  xa_debug (1, "DEBUG: Setting up default values");

  get_short_date(info.date);

  info.consoledebuglevel = 0;
  info.logfiledebuglevel = 0;

#ifdef HAVE_UMASK
  {
    mode_t before, after = 022;
    before = umask(after);
    xa_debug(1, "DEBUG: Changed umask from %d to %d", before, after);
  }
#endif

  /* Create the data locking mutexes */
  thread_create_mutex(&info.double_mutex);
  thread_create_mutex(&info.source_mutex);
  thread_create_mutex(&info.sourcesstats_mutex);
  thread_create_mutex(&info.admin_mutex);
  thread_create_mutex(&info.misc_mutex);
  thread_create_mutex(&info.alias_mutex);
  thread_create_mutex(&info.hostname_mutex);
  thread_create_mutex(&info.resolvmutex);
  thread_create_mutex(&info.relay_mutex);
  thread_create_mutex(&info.acl_mutex);
  thread_create_mutex(&info.sourcetable_mutex);
  thread_create_mutex(&info.client_mutex);
  thread_create_mutex(&info.logfile_mutex);
  /* rtsp. */
  thread_create_mutex(&info.session_mutex);
  thread_create_mutex(&info.header_mutex);

#ifdef DEBUG_SOCKETS
  thread_create_mutex(&sock_mutex);
#endif
  info.resolv_type = DEFAULT_RESOLV_TYPE;

  memset((void *)&localaddr, 0, sizeof (localaddr));

  /* Default value for hostname reverse lookups */
  info.reverse_lookups = DEFAULT_LOOKUPS;

  /* Statistics */
  zero_stats(&info.daily_stats);
  zero_stats(&info.hourly_stats);
  zero_stats(&info.total_stats);

  /* Time settings to zero */
  info.server_start_time = get_time();
  info.statuslasttime = 0;

  info.mount_fallback = DEFAULT_MOUNT_FALLBACK;
  info.force_servername = DEFAULT_FORCE_SERVERNAME;

  info.throttle = (double)DEFAULT_THROTTLE;
  info.sleep_ratio = (double)DEFAULT_SLEEP_RATIO;
  info.throttle_on = 0;
  info.bandwidth_usage = 0;

  info.id = 0;

  info.policy = DEFAULT_ACL_POLICY;
  info.allow_http_admin = DEFAULT_ALLOW_HTTP_ADMIN;

  info.port[0] = DEFAULT_PORT;
  for (i = 1; i < MAXLISTEN; i++) {
    info.port[i] = 0;
  }

  /* Variables that affect clients */
  info.num_clients = 0;
  info.max_clients = DEFAULT_MAX_CLIENTS;
  info.max_ip_connections = DEFAULT_MAX_IP_CONNECTIONS;
  info.max_clients_per_source = DEFAULT_MAX_CLIENTS_PER_SOURCE;
  info.client_timeout = DEFAULT_CLIENT_TIMEOUT; /* How long to wait after lost encoder to kick clients */

  /* Variables that affect sources */
  info.num_sources = 0;
  info.max_sources = DEFAULT_MAX_SOURCES;
  info.encoder_pass = nstrdup(DEFAULT_ENCODER_PASSWORD);
  info.default_sourceopts = nstrdup (DEFAULT_SOURCE_OPTS);

  /* Variables that affect admins */
  info.num_admins = 0;
  info.max_admins = DEFAULT_MAX_ADMINS;
  info.remote_admin_pass = nstrdup(DEFAULT_REMOTE_ADMIN_PASSWORD);

  /* Variables that affect stats dumping */
  info.statustime = DEFAULT_STATUSTIME;

  /* Other variables */
  info.console_mode = CONSOLE_ADMIN_TAIL;

  info.myhostname = NULL;
  info.server_name = nstrdup("localhost");

  info.version = VERSION;
  info.ntripversion = nstrdup(NTRIP_VERSION);
  info.ntripinfourl = nstrdup(DEFAULT_NTRIP_INFO_URL);
  info.name = nstrdup(DEFAULT_NAME);
  info.operator = nstrdup(DEFAULT_OPERATOR);
  info.operatorurl = nstrdup(DEFAULT_OPERATOR_URL);
  get_string_time(info.timezone, get_time(), "%Z");

  info.prompt = nstrdup ("-> ");

#ifdef USE_CRYPT
  info.encrypt_passwords = 0;
#endif /* USE_CRYPT */

  info.oper_pass = nstrdup(DEFAULT_OPER_PASSWORD);

  if (!info.runpath)
    fprintf (stderr, "WARNING: info.runpath == NULL!!\n");

  info.logdir = nstrdup(DEFAULT_LOG_DIR);
  if (info.logdir[0] != DIR_DELIMITER) {
    nfree(info.logdir);
    info.logdir = nmalloc(strlen(info.runpath) + strlen(DEFAULT_LOG_DIR) + 1);
    strcpy(info.logdir, info.runpath);
    strcat(info.logdir, DEFAULT_LOG_DIR);
  }
  info.etcdir = nstrdup(DEFAULT_ETC_DIR);
  if (info.etcdir[0] != DIR_DELIMITER) {
    nfree(info.etcdir);
    info.etcdir = nmalloc(strlen(info.runpath) + strlen(DEFAULT_ETC_DIR) + 1);
    strcpy(info.etcdir, info.runpath);
    strcat(info.etcdir, DEFAULT_ETC_DIR);
  }

  info.vardir = nstrdup(DEFAULT_VAR_DIR);
  if (info.vardir[0] != DIR_DELIMITER) {
    nfree(info.vardir);
    info.vardir = nmalloc(strlen(info.runpath) + strlen(DEFAULT_VAR_DIR) + 1);
    strcpy(info.vardir, info.runpath);
    strcat(info.vardir, DEFAULT_VAR_DIR);
  }

  info.templatedir = nstrdup(DEFAULT_TEMPLATE_DIR);
  if (info.templatedir[0] != DIR_DELIMITER) {
    nfree(info.templatedir);
    info.templatedir = nmalloc(strlen(info.runpath) + strlen(DEFAULT_TEMPLATE_DIR) + 1);
    strcpy(info.templatedir, info.runpath);
    strcat(info.templatedir, DEFAULT_TEMPLATE_DIR);
  }

  info.configfile = nstrdup(DEFAULT_CONFIG_FILE);
  info.userfile = nstrdup(DEFAULT_USER_FILE);
  info.client_mountfile = nstrdup(DEFAULT_CLIENT_MOUNT_FILE);
  info.source_mountfile = nstrdup(DEFAULT_SOURCE_MOUNT_FILE);
  info.groupfile = nstrdup(DEFAULT_GROUP_FILE);
  info.watchfile = nstrdup(DEFAULT_WATCH_FILE);
  info.pidfile = nstrdup(DEFAULT_PID_FILE);

// added. ajd
  info.sourcetablefile = nstrdup(DEFAULT_SOURCETABLE_FILE);
//  get_ntripcaster_file ("sourcetable.dat.utd", var_file_e, R_OK, pathandfile);
//  info.sourcetableutdfile = nstrdup(pathandfile);
//  get_ntripcaster_file ("sourcetable.dat.xxx", var_file_e, R_OK, pathandfile);
//  info.sourcetablexxxfile = nstrdup(pathandfile);

  info.accessfilename = nstrdup(DEFAULT_ACCESS_FILENAME);
  info.accessfile = -1;

  info.usagefilename = nstrdup(DEFAULT_USAGE_FILENAME);
  info.usagefile = -1;

  info.logfilename = nstrdup(DEFAULT_LOGFILENAME);
  info.logfile = -1;

//  info.statscount = 0; // added. ajd

  /* Transparent proxy support? */
  info.transparent_proxy = DEFAULT_TRANSPARENT_PROXY;

  info.kick_relays = DEFAULT_KICK_RELAYS;
  info.relay_reconnect_time = DEFAULT_RELAY_RECONNECT_TIME;
  info.relay_reconnect_tries = DEFAULT_RELAY_RECONNECT_TRIES;
  info.kick_clients = DEFAULT_KICK_CLIENTS;

  /* Server meta info */
  info.location = nstrdup(DEFAULT_LOCATION);
  info.rp_email = nstrdup(DEFAULT_RP_EMAIL);
  info.url = nstrdup(DEFAULT_URL);

  info.session_timeout = DEFAULT_SESSION_TIMEOUT;

#ifdef HAVE_LIBLDAP
  info.ldap_server = nstrdup(NC_LDAP_HOST);
  info.ldap_uid_prefix = nstrdup(NC_LDAP_UID_PREFIX);
  info.ldap_people_context = nstrdup(NC_LDAP_PEOPLE_CONTEXT);
#endif /* HAVE_LIBLDAP */

  /* Point variables, bit of a mess */
  setup_config_file_settings();
  setup_admin_settings();

  info.sourcetable.length = 0;
//  info.sourcetable.show_length = 0;
  info.sourcetable.lines = 0;
}

/* Allocate all the avl trees for admins, directory servers
   and sources, and make the admin settings point to the
   correct global values */
void
allocate_resources()
{
  xa_debug (1, "DEBUG: Allocating server resources");

  /* Allocate all the sources. */
  info.sources = avl_create(compare_connection, &info);

  /* Allocate all the sources stats. */
  info.sourcesstats = avl_create(compare_statisticsentry, &info);

  info.clients = avl_create(compare_connection, &info); // added. ajd

  /* Allocate all the admin slots */
  info.admins = avl_create(compare_connection, &info);

  info.rtsp_sessions = avl_create(compare_sessions, &info);

  info.nontripsources = avl_create(compare_nontrip_sources, &info); // nontrip. ajd

  /* And a tree of aliases */
  info.aliases = avl_create(compare_aliases, &info);

  /* And a tree of relays */
  info.relays = avl_create (compare_relays, &info);

  /* And a tree of hostnames that point to me */
  info.my_hostnames = avl_create(compare_strings, &info);

  info.sourcetable.tree = avl_create(compare_sourcetable_entrys, &info);

#ifdef DEBUG_SOCKETS
  sock_sockets = avl_create (compare_sockets, &info);
#endif

  info.all_acl = avl_create(compare_restricts, &info);
  info.admin_acl = avl_create(compare_restricts, &info);
  info.source_acl = avl_create(compare_restricts, &info);
  info.client_acl = avl_create(compare_restricts, &info);

  pool_init ();

  /* you might notice that the thread tree is not created here,
     this is on purpose :) */
  if (!info.sources || !info.relays || !info.admins || !info.threads || !info.aliases
      || !info.my_hostnames) {
    fprintf(stderr, "Cannot allocate tree resources, exiting");
    clean_resync(&info);
  }

}

static void freesourcestats(statisticsentry_t *st, void *param)
{
  nfree(st->mount);
  nfree(st);
}

/* Resyncing server, make sure sockets are closed, free up the memory */
void
clean_resync (server_info_t *info)
{
  connection_t *con;
//  directory_server_t *ds;
  int i;
  avl_traverser trav = {0};
  static int main_shutting_down = 0; // was 'static main_shutting_down'. ajd

  thread_library_lock ();
    if (!main_shutting_down)
      main_shutting_down = 1;
    else
      thread_exit (0);
  thread_library_unlock ();


  write_log(LOG_DEFAULT, "Resync...");

  write_log(LOG_DEFAULT, "Closing all listening sockets...");

  for (i = 0; i < MAXLISTEN; i++)
  {
    if (sock_valid (info->listen_sock[i]))
      sock_close(info->listen_sock[i]);
    if (sock_valid (info->listen_sock_udp[i]))
      sock_close(info->listen_sock_udp[i]);
  }

  write_log(LOG_DEFAULT, "Closing all NoNTRIP source listening sockets...");

  close_nontrip_listen_sockets(); // nontrip. ajd

  interpreter_shutdown ();

  /* Try to kill some threads off */
  kill_threads();

  /* Close all remaining bloody sockets */
  sock_close_all_sockets ();

  /* Wait for the last mad threads to die  */
  thread_wait_for_solitude ();

  thread_mutex_lock(&info->source_mutex);

  write_log(LOG_DEFAULT, "Removing remaining sources...");
  while ((con = avl_traverse(info->sources, &trav)))
    kick_connection(con, "Server resync");

  thread_mutex_unlock(&info->source_mutex);

  cleanup_authentication_scheme();
  cleanup_sourcetable();

  thread_mutex_lock(&info->sourcesstats_mutex);
  if (info->sourcesstats)
    avl_destroy(info->sourcesstats, (avl_node_func)freesourcestats);
  thread_mutex_unlock(&info->sourcesstats_mutex);

  pool_shutdown ();

  zero_trav(&trav);

#ifdef _WIN32
  /* Cleanup Winsock */
  WSACleanup();
#endif

  write_log(LOG_DEFAULT, "Exiting..");
  if (info->logfile != -1)
    fd_close(info->logfile);

#ifdef DEBUG_MEMORY
  /* Check if any other threads left some shit behind */
  {
    meminfo_t *mi;
    avl_traverser trav = {0};

    while ((mi = avl_traverse(info->mem, &trav))) {
      if (mi->thread_id != 0 && mi->thread_id != -1)
        write_log(LOG_DEFAULT, "WARNING: %d bytes allocated by thread %d at line %d in %s not freed before thread exit", mi->size, mi->thread_id, mi->line, mi->file);
    }
  }
#endif

  exit(0);
}

/* Main server loop, listen to the specified socket for new
 * connections, and immediately spawn a new thread for each
 * new connection */
void *
threaded_server_proc (void *infoarg)
{
  connection_t *con;
  mythread_t *mt = thread_get_mythread ();

/* added. ajd */
  mt->ping = 0;
  info.main_thread = mt;

  write_log(LOG_DEFAULT, "Starting main connection handler...");

  /* Setup listeners */
  setup_listeners();

  /* Just print some runtime server info */
  print_startup_server_info();

  write_log (LOG_DEFAULT, "Starting Calender Thread...");
  /* Fork another thread that handles stats dumping, directory servers and other time based stuff */
  thread_create("Calendar Thread", startup_timer_thread, NULL);

  thread_create("Watchdog Thread", startup_watchdog_thread, NULL);

  /*
   * And one heartbeat thread that should never have to do anything, but
   * will unlock mutexes when locked for more than MAX_MUTEX_LOCKTIME seconds.
   * This is currently disabled, until we find a better way to do this :)
   *
     thread_create("Heartbeat Thread", startup_heartbeat_thread, NULL);
  */

  /* And one to update udp info on clients */
//  write_log (LOG_DEFAULT, "Starting UDP handler thread...");

//  thread_create("UDP Handler Thread", startup_udp_info_thread, NULL); // not needed. ajd

  /* And one to connect and reconnect relays */
  write_log (LOG_DEFAULT, "Starting relay connector thread...");

  thread_create("Relay Connector Thread", startup_relay_connector_thread, NULL);

//  update_sourcetable(); // update 'sourcetable.dat.utd' at startup of the server. ajd

  thread_create("NoNTRIP Listen Thread", listen_to_nontrip_sources, NULL); // nontrip. ajd

  thread_create("UDP Listen Thread", listen_to_udp, NULL); // nontrip. ajd

  while (is_server_running())
  {
    // Try to get a new connection
    con = get_connection(info.listen_sock);

    if (con) {
      // Ok, we got one, handle it in a new thread
      thread_create("Connection Handler", handle_connection, (void *)con);
    }


    if (mt->ping == 1) mt->ping = 0;
  }

  /* I guess the user pressed ^C */
  clean_resync(&info);

  return NULL;
}

#ifdef _WIN32

BOOL WINAPI
win_sig_die(DWORD CtrlType)
{
  write_log(LOG_DEFAULT, "Caught signal %d, perhaps someone is at the door?", CtrlType);
  set_server_running(SERVER_DYING);
  return 1;
}

#else /* others */

void sig_child(int signo)
{
  /*pid_t pid;*/
  int stat;

  /*pid = */wait(&stat);
#ifdef __linux__
  signal(SIGCHLD, sig_child);
#endif
}

void sig_hup(int signo)
{
  parse_default_config_file();
  open_log_files();

  write_log(LOG_DEFAULT, "Caught SIGHUP, rehashed config and reopened logfiles...");

/* This might not be necessary on all systems, but I doubt it will hurt anyone */
  signal(SIGHUP, sig_hup);
}

void sig_die(int signo)
{
  write_log(LOG_DEFAULT, "Caught signal %d, resyncing!", signo);
  set_server_running(SERVER_DYING);
}

void sig_die_hard(int signo)
{
  printf("Caught signal %d!\n", signo);
  exit(1);
}

#endif /* sig stuff */

/*
 * Create and listen to the specified ports,
 * find out local ip if dynamic,
 * make sure the server name is resolvable
 */
void
setup_listeners()
{
  int i;

  for (i = 0; i < MAXLISTEN; i++)
  {
    info.listen_sock[i] = INVALID_SOCKET;
    info.listen_sock_udp[i] = INVALID_SOCKET;
  }

  /* Create the socket, on the correct hostname or INADDR_ANY and bind it to the port. */
  for (i = 0; i < MAXLISTEN; i++)
  {
    if (info.port[i] <= 0) {
      info.port[i] = INVALID_SOCKET;
      continue;
    }

    info.listen_sock[i] = sock_get_server_socket(info.port[i], 0);

    if (info.listen_sock[i] == INVALID_SOCKET)
    {
      write_log(LOG_DEFAULT, "ERROR: Could not listen to port %d. Perhaps another process is using it?", info.port[i]);
      clean_resync(&info);
    }

    /* Set the socket to nonblocking */
    sock_set_blocking(info.listen_sock[i], SOCK_BLOCKNOT);

    if (listen(info.listen_sock[i], LISTEN_QUEUE) == SOCKET_ERROR)
    {
      write_log(LOG_DEFAULT, "Could not listen for clients on port %d", info.port[i]);
      clean_resync(&info);
    }
  }

  /* Create the socket, on the correct hostname or INADDR_ANY and bind it to the port. */
  for (i = 0; i < MAXLISTEN; i++)
  {
    if (info.port[i] == INVALID_SOCKET) {
      continue;
    }

    info.listen_sock_udp[i] = sock_get_server_socket(info.port[i], 1);

    if (info.listen_sock_udp[i] == INVALID_SOCKET)
    {
      write_log(LOG_DEFAULT, "ERROR: Could not listen to UDP port %d. Perhaps another process is using it?", info.port[i]);
      clean_resync(&info);
    }

    /* Set the socket to nonblocking */
    sock_set_blocking(info.listen_sock[i], SOCK_BLOCKNOT);
  }

  if (ntripcaster_strcasecmp(info.server_name, "dynamic") == 0)
  {
    info.server_name = sock_get_local_ipaddress();
    write_log(LOG_DEFAULT, "Dynamic server name, using the local ip [%s]", info.server_name);
  } else {
    char *res, *buf = (char *)nmalloc(20);
    res = forward(info.server_name, buf);
    if (!res) {
      nfree(buf);
      write_log(LOG_DEFAULT, "WARNING: Resolving the server name [%s] does not work!", info.server_name);
      return;
    }

    thread_mutex_lock(&info.hostname_mutex);

    avl_insert(info.my_hostnames, info.server_name);
    avl_insert(info.my_hostnames, res);

    thread_mutex_unlock(&info.hostname_mutex);
  }
}

static int store_udp_data(connection_t *con, unsigned char *buffer, int len, unsigned int seq)
{
  /* handle 16 bit overflows, needs an upper limit in the following if clause */
  unsigned int diff = seq < con->udpbuffers->seq ? 65536+seq-con->udpbuffers->seq : seq-con->udpbuffers->seq;
  /* when 1000 blocks got missing essentially consider the stream broken or
   * the overflow detection handling was wrong */
  if(diff > 0 && diff < 1000)
  {
    con->udpbuffers->seq = seq;
    if(len)
    {
      thread_mutex_lock(&con->udpbuffers->buffer_mutex);
      if(con->udpbuffers->len+len <= sizeof(con->udpbuffers->buffer))
      {
        memcpy(con->udpbuffers->buffer+con->udpbuffers->len, buffer, len);
        con->udpbuffers->len += len;
      }
      else
      {
        xa_debug (2, "DEBUG: Skipping UDP packet due to missing space");
      }
      thread_mutex_unlock(&con->udpbuffers->buffer_mutex);
    }
    con->udpbuffers->lastactive = time(0);
  }
  else
  {
    xa_debug (2, "DEBUG: Skipping UDP packet due to wrong sequence");
  }
  return 1;
}

static int compareaddress(struct sockaddr_in *a, struct sockaddr_in *b)
{
  return (a->sin_family == AF_INET && b->sin_family == AF_INET && a->sin_port == b->sin_port
  && a->sin_addr.s_addr == b->sin_addr.s_addr);
}

connection_t *find_udp_connection(unsigned int ssrc, struct sockaddr_in *sin)
{
  connection_t *found = 0;
  avl_traverser trav = {0};
  connection_t *clicon = NULL;

  /* Search for clients for this source */
  while(!found && (clicon = avl_traverse (info.sources, &trav)))
  {
    if (clicon->udpbuffers && clicon->rtp && clicon->rtp->datagram->ssrc == htonl(ssrc)
    && compareaddress(sin, clicon->sin))
    {
      found = clicon;
    }
    else
    {
      source_t *s = clicon->food.source;
      avl_traverser travs = {0};
      connection_t *scon = NULL;
      while(!found && (scon = avl_traverse (s->clients, &travs)))
      {
        if (scon->udpbuffers && scon->rtp && scon->rtp->datagram->ssrc == htonl(ssrc)
        && compareaddress(sin, scon->sin))
        {
          found = scon;
        }
      }
    }
  }

  return found;
}

static void handle_udp_packet(unsigned char *buffer, int len, unsigned int seq, unsigned int tim, unsigned int ssrc, int command, struct sockaddr_in *sin, SOCKET sockfd)
{
  connection_t *con;
//printf("Handle UDP len %d seq %d time %d ssrc %u\n%.*s\n", len, seq, tim, htonl(ssrc), len, buffer);

  switch(command)
  {
  case 96:
    thread_mutex_lock(&info.source_mutex);
    if((con = find_udp_connection(ssrc, sin)))
      store_udp_data(con, buffer, len, seq);
    thread_mutex_unlock(&info.source_mutex);
    break;
  case 97:
    con = create_connection();
    con->sin = (struct sockaddr_in *)nmalloc(sizeof(struct sockaddr_in));
    if (!con->sin)
    {
      xa_debug (1, "ERROR: NULL sockaddr struct, wft???");
      nfree(con);
      return;
    }

    con->host = create_malloced_ascii_host(&(sin->sin_addr));
    con->sock = -1;
    *(con->sin) = (*sin);
    con->sinlen = sizeof(struct sockaddr_in);
    xa_debug (2, "DEBUG: Getting new UDP connection from host %s", con->host ? con->host : "(null)");
    con->hostname = NULL;
    con->headervars = NULL;
    con->id = new_id ();
    con->connect_time = get_time ();
    con->udpbuffers = (udpbuffers_t *) nmalloc(sizeof(udpbuffers_t));
    thread_create_mutex(&con->udpbuffers->buffer_mutex);
    memcpy(con->udpbuffers->buffer, buffer, len);
    con->udpbuffers->len = len;
    con->udpbuffers->sock = sockfd;
    con->udpbuffers->lastsend = con->udpbuffers->lastactive = time(0);
    con->data_protocol = udp_e;
    con->rtp = rtp_create();
    con->rtp->host_seq = rand();
    con->udpbuffers->seq = seq;
    con->rtp->datagram->ssrc = htonl(ssrc);
    while(find_udp_connection(ssrc, sin))
      ssrc = rand();
    con->udpbuffers->ssrc = ssrc;
    con->rtp->datagram->pt = 97;
    thread_create("Connection Handler", handle_connection, (void *)con);
    break;
  case 98:
    thread_mutex_lock(&info.source_mutex);
    if((con = find_udp_connection(ssrc, sin)))
      kick_connection(con, "Close packet received");
    thread_mutex_unlock(&info.source_mutex);
    break;
  };
}

static unsigned char udpbuffer[2048];
void *listen_to_udp(void *arg) {
  thread_init();
  while (is_server_running()) {
    int sockfd;
    socklen_t sin_len;
    fd_set rfds;
    struct timeval tv;
    int i, maxport = 0;
    struct sockaddr_in sin;

    /* setup sockaddr structure */
    sin_len = sizeof(sin);

    /* try to accept a connection */
    FD_ZERO(&rfds);

    for (i = 0; i < MAXLISTEN; i++) {
      sockfd = info.listen_sock_udp[i];
      if (sock_valid (sockfd)) {
        FD_SET(sockfd, &rfds);
        if (sockfd > maxport)
          maxport = sockfd;
      }
    }
    maxport += 1;

    tv.tv_sec = 0;
    tv.tv_usec = 30000;

    if (select(maxport, &rfds, NULL, NULL, &tv) > 0) {
      for (i = 0; i < MAXLISTEN; i++) {
        sockfd = info.listen_sock_udp[i];
        if (sock_valid (sockfd) && FD_ISSET(sockfd, &rfds))
        {
          int len = recvfrom(sockfd, udpbuffer, sizeof(udpbuffer), 0, (struct sockaddr *)&sin, &sin_len);
          if(len >= 12 && udpbuffer[0] == (2<<6) && (udpbuffer[1] >= 96 && udpbuffer[1] <= 98)) /* can be an RTP packet */
          {
            unsigned int sequence = (udpbuffer[2]<<8)|udpbuffer[3];
            unsigned int rtptime = (udpbuffer[4]<<24)|(udpbuffer[5]<<16)|(udpbuffer[6]<<8)|udpbuffer[7];
            unsigned int rtpsess = (udpbuffer[8]<<24)|(udpbuffer[9]<<16)|(udpbuffer[10]<<8)|udpbuffer[11];

            handle_udp_packet(udpbuffer+12, len-12, sequence, rtptime, rtpsess, udpbuffer[1], &sin, sockfd);
          }
        }
      }
    }
  }

  thread_exit(0);
  return NULL;
}

void *listen_to_nontrip_sources(void *arg) { // nontrip. ajd
  connection_t *con;

  thread_init();
  setup_nontrip_listen_sockets();

  while (is_server_running()) {
    con = get_nontrip_connection();
    if (con) thread_create("NoNtrip Source Connection Handler", handle_nontrip_connection, (void *)con);
  }

  thread_exit(0);
  return NULL;
}

void setup_nontrip_listen_sockets() { // nontrip. ajd
  avl_traverser trav = {0};
  nontripsource_t *nsource;

  while ((nsource = avl_traverse (info.nontripsources, &trav))) {
    nsource->listen_sock = sock_get_bound_tcp_socket(nsource->port);

    if (nsource->listen_sock != INVALID_SOCKET) {
      sock_set_blocking(nsource->listen_sock, SOCK_BLOCKNOT);
      if (listen(nsource->listen_sock, LISTEN_QUEUE) == SOCKET_ERROR) {
        write_log(LOG_DEFAULT, "Could not listen for NoNTRIP sources on mount %s port %d", nsource->mount, nsource->port);
      }
    }
  }
}

void close_nontrip_listen_sockets() { // nontrip. ajd
  avl_traverser trav = {0};
  nontripsource_t *nsource;

  while ((nsource = avl_traverse (info.nontripsources, &trav))) {
    if (sock_valid (nsource->listen_sock)) sock_close(nsource->listen_sock);
  }
}
