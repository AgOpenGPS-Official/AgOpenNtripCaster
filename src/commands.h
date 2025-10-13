/* commands.h
 * - Command Function Headers
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

#ifndef __NTRIPCASTER_COMMANDS_H
#define __NTRIPCASTER_COMMANDS_H

typedef struct
{
  char *name;                   /* Printable name of this setting */
  type_t type;                  /* Integer, string or double? */
  char *doc;                    /* What does this setting do? */
  void *setting;                /* The actual data affected by this setting */
} set_element;

typedef struct
{
  char *name;                   /* User printable name of the function. */
  ntripcaster_int_function *func; /* Function to call to do the job. */
  char *doc;                    /* Documentation for this function.  */
  int oper;                     /* Has to be oper? */
  int show;
  char *doclong;                /* Long description of command */
} comp_element;

/* Now list all the functions */
int
  com_admins(), com_help(com_request_t *), com_tail(), com_untail(),
  com_sources(), com_sourcetable(), com_connections(),
  com_listeners(), com_rehash(), com_set(),
  com_uptime(), com_tell(), com_resync(), com_oper(),
  com_kick(), com_quit(), com_stats(com_request_t *req),
  com_stats_prom(com_request_t *req),
  com_relay(), com_list(), com_alias(), com_threads(),
  com_status(), com_allow(), com_deny(),
  com_modify(), com_locks(),
  com_debug (), com_mem (),
  com_describe (), com_acl (), com_auth (), com_scheme (),
  com_runtime ();

void handle_admin_command(connection_t *con, char *command, int command_len);
void show_settings(com_request_t *req);
void setup_admin_settings();
void setup_config_file_settings();
int parse_config_file(char *file);
void log_command (const char *command, const com_request_t *req);
char *clean_string(char *string);
void tell_admins(void *data, void *param);
void move_to (void *clientarg, void *sourcetargetarg);
avl_tree *get_acl_tree (char *type);
int parse_default_config_file ();
char *com_arg (const com_request_t *req);
void change_special_variable (com_request_t *req, set_element *s, char *variable_name, char *argument);
int is_special_variable (char *variable_name);
void change_variable (com_request_t *req, set_element *s, char *argument, char *arg);
const comp_element *find_comp_element (const char *name, const comp_element *el);
int com_auth_list (com_request_t *req, char *arg);
int com_auth_add (com_request_t *req, char *arg);
char *variable_to_string (char *varname);
#ifdef HAVE_LIBREADLINE
char *commands_generator (char *text, int state);
char *settings_generator (char *text, int state);
#endif

/* Indexes for com_sources() */
#define SOURCE_OPTS 27
#define SOURCE_SHOW_ID 0
#define SOURCE_SHOW_SOCKET 1
#define SOURCE_SHOW_CTIME 2
#define SOURCE_SHOW_IP 3
#define SOURCE_SHOW_HOST 4
#define SOURCE_SHOW_HEADERS 5
#define SOURCE_SHOW_STATE 6
#define SOURCE_SHOW_TYPE 7
#define SOURCE_SHOW_PROTO 8
#define SOURCE_SHOW_CLIENTS 9
#define SOURCE_SHOW_DUMPFILE 10
#define SOURCE_SHOW_PRIO 11
#define SOURCE_SHOW_SONG_TITLE 12
#define SOURCE_SHOW_SONG_URL 13
#define SOURCE_SHOW_STREAM_MSG 14
#define SOURCE_SHOW_SONG_LENGTH 15
#define SOURCE_SHOW_NAME 16
#define SOURCE_SHOW_GENRE 17
#define SOURCE_SHOW_BITRATE 18
#define SOURCE_SHOW_URL 19
#define SOURCE_SHOW_MOUNT 20
#define SOURCE_SHOW_DESC 21
#define SOURCE_SHOW_READ 22
#define SOURCE_SHOW_WRITTEN 23
#define SOURCE_SHOW_CONNECTS 24
#define SOURCE_SHOW_TIME 25
#define SOURCE_SHOW_AGENT 26

/* Message codes for admin output */

/* com_admins() */
#define ADMIN_SHOW_ADMIN_START 100
#define ADMIN_SHOW_ADMIN_ENTRY 101
#define ADMIN_SHOW_ADMIN_END   102
#define ADMIN_SHOW_ADMIN_CAPTIONS 103

/* show_settings() */
#define ADMIN_SHOW_SETTINGS_START      110
#define ADMIN_SHOW_SETTINGS_ENTRY_INT  111
#define ADMIN_SHOW_SETTINGS_ENTRY_REAL 112
#define ADMIN_SHOW_SETTINGS_ENTRY_STRING 113
#define ADMIN_SHOW_SETTINGS_ENTRY 114
#define ADMIN_SHOW_SETTINGS_END 115
#define ADMIN_SHOW_SETTINGS_CAPTION 116

/* com_help() */
#define ADMIN_SHOW_COMMANDS_START    120
#define ADMIN_SHOW_COMMANDS_ENTRY    121
#define ADMIN_SHOW_COMMANDS_END      122
#define ADMIN_SHOW_COMMANDS_INVALID  123
#define ADMIN_SHOW_COMMANDS_SPECIFIC 124

/* com_set() */
#define ADMIN_SHOW_SETTINGS_INVALID        130
#define ADMIN_SHOW_SETTINGS_CHANGED_INT    131
#define ADMIN_SHOW_SETTINGS_CHANGED_REAL   132
#define ADMIN_SHOW_SETTINGS_CHANGED_STRING 133

/* com_tail() / com_untail() */
#define ADMIN_SHOW_TAILING_ON 140
#define ADMIN_SHOW_TAILING_OFF 141

/* com_sources() */
#define ADMIN_SHOW_SOURCE_START 150
#define ADMIN_SHOW_SOURCE_ENTRY 151
#define ADMIN_SHOW_SOURCE_END 152
#define ADMIN_SHOW_SOURCE_CAPTIONS 153

/* com_listeners() */
#define ADMIN_SHOW_LISTENERS_START 160
#define ADMIN_SHOW_LISTENERS_ENTRY 161
#define ADMIN_SHOW_LISTENERS_END 162

/* com_rehash() */
#define ADMIN_SHOW_REHASH 170
#define ADMIN_SHOW_SOURCETABLE_UPDATE 171

/* com_uptime() */
#define ADMIN_SHOW_UPTIME 180

/* com_tell() */
#define ADMIN_SHOW_TELL 190
#define ADMIN_SHOW_TELL_INVALID 191

/* com_stats() */
#define ADMIN_SHOW_STATS_DAILY 200
#define ADMIN_SHOW_STATS_HOURLY 201
#define ADMIN_SHOW_STATS_CLEAR 202
#define ADMIN_SHOW_STATS_INVALID 203
#define ADMIN_SHOW_STATS_TOTAL 204
#define ADMIN_SHOW_STATS_NUMADMINS 205
#define ADMIN_SHOW_STATS_NUMSOURCES 206
#define ADMIN_SHOW_STATS_NUMLISTENERS 207
#define ADMIN_SHOW_STATS_MISC 208
#define ADMIN_SHOW_STATS_READ 209
#define ADMIN_SHOW_STATS_WRITTEN 210
#define ADMIN_SHOW_STATS_SOURCE_CONNECTS 211
#define ADMIN_SHOW_STATS_CLIENT_CONNECTS 212
#define ADMIN_SHOW_STATS_LISTENER_ALT 213
#define ADMIN_SHOW_STATS_LISTENER_ALTF 214
#define ADMIN_SHOW_STATS_SOURCE_ALT 215
#define ADMIN_SHOW_STATS_SOURCE_ALTF 216
#define ADMIN_SHOW_STATS_END 217
#define ADMIN_SHOW_STATS_TIME 218

/* com_resync() */
#define ADMIN_SHOW_RESYNC 230

/* com_oper() */
#define ADMIN_SHOW_OPER_INVALID 240
#define ADMIN_SHOW_OPER_OK 241

/* com_kick() */
#define ADMIN_SHOW_KICK_INVALID_SYNTAX 280
#define ADMIN_SHOW_KICKING_ADMINS_MATCHING 281
#define ADMIN_SHOW_KICKING_SOURCES_MATCHING 282
#define ADMIN_SHOW_KICKING_CLIENTS_MATCHING 283
#define ADMIN_SHOW_KICKING_ALL_MATCHING 284
#define ADMIN_SHOW_KICK_INVALID_ID 285
#define ADMIN_SHOW_KICK_YOURSELF 286

/* com_list () */
#define ADMIN_SHOW_LIST_START 300
#define ADMIN_SHOW_LIST_ENTRY 301
#define ADMIN_SHOW_LIST_END 302

/* com_relay() */
#define ADMIN_SHOW_RELAY_INVALID_SYNTAX 310
#define ADMIN_SHOW_RELAY_CONNECTING 311
#define ADMIN_SHOW_RELAY_INVALID_SOURCE 312
#define ADMIN_SHOW_RELAY_ARGUMENT_REQUIRED 313
#define ADMIN_SHOW_RELAY_CONNECT_FAILED 314
#define ADMIN_SHOW_RELAY_ITEM 315
#define ADMIN_SHOW_RELAY_OK 320
#define ADMIN_SHOW_RELAY_INVALID_URL 321
#define ADMIN_SHOW_RELAY_CONNECT_OK 322
#define ADMIN_SHOW_RELAY_LIST_START 323
#define ADMIN_SHOW_RELAY_LIST_END 324
#define ADMIN_SHOW_RELAY_REMOVED 325

/* com_alias () */
#define ADMIN_SHOW_ALIAS_INVALID_SYNTAX 330
#define ADMIN_SHOW_ALIAS_ADD_OK 331
#define ADMIN_SHOW_ALIAS_ADD_FAILED 332
#define ADMIN_SHOW_ALIAS_REMOVE_OK 333
#define ADMIN_SHOW_ALIAS_REMOVE_FAILED 334
#define ADMIN_SHOW_ALIAS_UNKNOWN_SUBCOMMAND 335
#define ADMIN_SHOW_ALIAS_START 336
#define ADMIN_SHOW_ALIAS_ENTRY 337
#define ADMIN_SHOW_ALIAS_END 338

/* com_threads() */
#define ADMIN_SHOW_THREADS_START 340
#define ADMIN_SHOW_THREADS_ENTRY 341
#define ADMIN_SHOW_THREADS_END   342

/* com_status() */
#define ADMIN_SHOW_STATUS_INVALID_SYNTAX 350
#define ADMIN_SHOW_STATUS_NEW 351
#define ADMIN_SHOW_STATUS 352

/* com_restrict() */
#define ADMIN_SHOW_RESTRICT_INVALID_SYNTAX 380
#define ADMIN_SHOW_RESTRICT_ADD_OK 381
#define ADMIN_SHOW_RESTRICT_ADD_FAILED 382
#define ADMIN_SHOW_RESTRICT_REMOVE_OK 383
#define ADMIN_SHOW_RESTRICT_REMOVE_FAILED 384
#define ADMIN_SHOW_RESTRICT_UNKNOWN_SUBCOMMAND 385
#define ADMIN_SHOW_RESTRICT_START_DENY 386
#define ADMIN_SHOW_RESTRICT_ENTRY 387
#define ADMIN_SHOW_RESTRICT_END 388
#define ADMIN_SHOW_RESTRICT_START_ALLOW 389
#define ADMIN_SHOW_RESTRICT_START_ALL 390
#define ADMIN_SHOW_RESTRICT_START_CON_ALL 391
#define ADMIN_SHOW_RESTRICT_START_CON_CLIENT 392
#define ADMIN_SHOW_RESTRICT_START_CON_SOURCE 393
#define ADMIN_SHOW_RESTRICT_START_CON_ADMIN 394

/* com_acl () */
#define ADMIN_SHOW_ACL_INVALID_SYNTAX 600

/* com_modify() */
#define ADMIN_SHOW_MODIFY_INVALID_SYNTAX 400
#define ADMIN_SHOW_MODIFY_INVALID_SOURCE_ID 401
#define ADMIN_SHOW_MODIFY_VALUE_CHANGED 402

/* com_locks() */
#define ADMIN_SHOW_LOCKS_ENTRY 410
#define ADMIN_SHOW_LOCKS_NOT_AVAIL 411

/* com_debug () */
#define ADMIN_SHOW_DEBUG_CURRENT 420
#define ADMIN_SHOW_DEBUG_CHANGED_TO 421

/* com_auth () */
#define ADMIN_SHOW_AUTH_INVALID_SYNTAX 430
#define ADMIN_SHOW_AUTH_USER_START 431
#define ADMIN_SHOW_AUTH_USER_ENTRY 432
#define ADMIN_SHOW_AUTH_USER_END 433
#define ADMIN_SHOW_AUTH_GROUP_START 434
#define ADMIN_SHOW_AUTH_GROUP_ENTRY 435
#define ADMIN_SHOW_AUTH_GROUP_END 436
#define ADMIN_SHOW_AUTH_MOUNT_START 437
#define ADMIN_SHOW_AUTH_MOUNT_ENTRY 438
#define ADMIN_SHOW_AUTH_MOUNT_END 439

/* com_describe () */
#define ADMIN_SHOW_DESCRIBE_INVALID_SYNTAX 480
#define ADMIN_SHOW_DESCRIBE_INVALID_ID 481
#define ADMIN_SHOW_DESCRIBE_INVALID_TYPE 482
#define ADMIN_SHOW_DESCRIBE_CLIENT_START 483
#define ADMIN_SHOW_DESCRIBE_CLIENT_MISC 484
#define ADMIN_SHOW_DESCRIBE_CLIENT_END 497
#define ADMIN_SHOW_DESCRIBE_SOURCE_START 485
#define ADMIN_SHOW_DESCRIBE_SOURCE_MISC 486
#define ADMIN_SHOW_DESCRIBE_SOURCE_END 487
#define ADMIN_SHOW_DESCRIBE_ADMIN_START 488
#define ADMIN_SHOW_DESCRIBE_ADMIN_MISC 489
#define ADMIN_SHOW_DESCRIBE_ADMIN_END 490
#define ADMIN_SHOW_DESCRIBE_CON_START 491
#define ADMIN_SHOW_DESCRIBE_CON_MISC 492
#define ADMIN_SHOW_DESCRIBE_CON_END 493
#define ADMIN_SHOW_DESCRIBE_CON_HEADERS_START 494
#define ADMIN_SHOW_DESCRIBE_CON_HEADERS_ENTRY 495
#define ADMIN_SHOW_DESCRIBE_CON_HEADERS_END 496

/* com_quit () */
#define ADMIN_SHOW_QUIT_BYE 500

/* com_scheme () */
#define ADMIN_SHOW_SCHEME_TYPE 510
#define ADMIN_SHOW_SCHEME_UNKNOWN_SCHEME 511
#define ADMIN_SHOW_SCHEME_CHANGED_TO 512

/* com_runtime */
#define ADMIN_SHOW_RUNTIME_START 530
#define ADMIN_SHOW_RUNTIME_SLEEP_METHOD 531
#define ADMIN_SHOW_RUNTIME_BACKLOG 532
#define ADMIN_SHOW_RUNTIME_RESOLV 533
#define ADMIN_SHOW_RUNTIME_MEMORY_DEBUG 534
#define ADMIN_SHOW_RUNTIME_MUTEX_DEBUG 535
#define ADMIN_SHOW_RUNTIME_POSIX_SIGNALS 536
#define ADMIN_SHOW_RUNTIME_THREADS 537
#define ADMIN_SHOW_RUNTIME_USE_CRYPT 538
#define ADMIN_SHOW_RUNTIME_HAVE_LIBWRAP 539
#define ADMIN_SHOW_RUNTIME_HAVE_LIBLDAP 540

/* com_mem () */
#define ADMIN_SHOW_MEM_NOT_AVAIL 550
#define ADMIN_SHOW_MEM_START 550
#define ADMIN_SHOW_MEM_ENTRY 551
#define ADMIN_SHOW_MEM_END 552
#define ADMIN_SHOW_MEM_MCHECK_TOTAL 553
#define ADMIN_SHOW_MEM_MCHECK_UNUSED 554
#define ADMIN_SHOW_MEM_MCHECK_MMAP 555
#define ADMIN_SHOW_MEM_MCHECK_START 556
#define ADMIN_SHOW_MEM_MCHECK_CHUNK_MMAP 557
#define ADMIN_SHOW_MEM_MCHECK_OCCUPIED 558
#define ADMIN_SHOW_MEM_MCHECK_FREE 559
#define ADMIN_SHOW_MEM_MCHECK_KEEPCOST 560

/* com_sourcetable (). */
#define ADMIN_SHOW_SOURCETABLE 600
#define ADMIN_SHOW_SOURCETABLE_LINE 601
#define ADMIN_SHOW_SOURCETABLE_RED_LINE 602
#define ADMIN_SHOW_SOURCETABLE_END 603
#define ADMIN_SHOW_SOURCETABLE_NEW_NET 605

/* to show html links. */
#define ADMIN_SHOW_LINKS 650

/* to show html authentication page. */
#define TABLE_START 700
#define TABLE_D 701
#define TABLE_R 702
#define TABLE_END 703
#define FORM_START 704
#define FORM_END 705
#define SELECT_START 706
#define SELECT_OPTION 707
#define SELECT_END 708
#define CHECK_ITEM 709
#define TEXT_FIELD 710
#define SUBMIT_BUTTON 711

/* com_connections() */
#define ADMIN_SHOW_CONNECTIONS_START 600
#define ADMIN_SHOW_CONNECTIONS_ENTRY 601
#define ADMIN_SHOW_CONNECTIONS_END 602
#define ADMIN_SHOW_CONNECTIONS_CAPTIONS 603

#endif
