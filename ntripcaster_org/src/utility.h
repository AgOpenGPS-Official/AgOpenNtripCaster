/* utility.h
 * - Utility Function Headers
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

#ifndef __NTRIPCASTER_UTILITY_H
#define __NTRIPCASTER_UTILITY_H

char *clean_string_from_spaces(char *string);
char *clean_string_from_leading_spaces(char *string);
void clean_away_source(source_t *source);
void assign_old_listeners(source_t *ser);
int map_id_to_source_socket(char *idstring);
source_t *source_with_id(int id);
int password_match(const char *crypted, const char *uncrypted);
int check_pass(int sockfd, char *pass, int *counter, char *string);
void print_connection (void *data, void *param);
void print_client(void *data, void *param);
void print_admin(void *data, void *param);
int field_ok (char *xac, char *field);
void kick_connection_not_me (void *conarg, void *reasonarg);
void kick_connection(void *conarg, void *reasonarg);
void kick_everything();
void kick_if_match (char *pattern);
void kick_not_connected (connection_t *con, char *reason);
void kick_silently (connection_t *con);
connection_t *get_admin_with_id(int id);
void close_connection(void *data);
void close_socket(sock_t sock);
source_t *source_with_client(connection_t *con);
void threaded_detach ();
int server_detach();
connection_t *find_id (int id);
connection_t *find_client_with_id (int id);
connection_t *find_con_with_host_and_udpport (const char *hostptr, const int portnr);
connection_t *find_con_with_host (const struct sockaddr_in *sin);
connection_t *find_source_with_id (int id);
connection_t *find_source_with_mount (char *mount);
void kill_threads ();
unsigned long int new_id ();
void do_if_match_all (char *pattern, avl_node_func func, void *buf, int destructive);
void do_if_match_tree (avl_tree *tree, char *pattern, avl_node_func func, void *buf, int destructive);
void do_if_match_tree_destructive (avl_tree *tree, char *pattern, avl_node_func func, void *buf);
time_t tree_time(avl_tree *tree);
void write_ntripcaster_header ();
void print_startup_server_info ();
void sanity_check ();
unsigned long int transfer_average (unsigned long int bytes, unsigned long int connections);
char *connect_average (unsigned long int seconds, unsigned long int connections, char *buf);
int hostname_local (char *name);
void build_request (connection_t *con, char *line, ntrip_request_t *req);
connection_t *mount_exists (char *mount);
void zero_request (ntrip_request_t *req);
void generate_request (char *line, ntrip_request_t *req);
void generate_http_request (char *line, ntrip_request_t *req);
void init_thread_tree (int line, char *file);
char *next_mount_point();
connection_t *get_pending_mount (char *mount);
void pending_connection (connection_t *con);
int pending_source_signoff (connection_t *con);
int hosteq (connection_t *con, connection_t *con2);
int hostmatch (const connection_t *con, const char *pattern);
int open_for_reading (const char *filename);
int open_for_writing (const char *filename);
int open_for_append (const char *filename);
char *get_ntripcaster_file(const char *filename, filetype_t type, int flags, char *path_and_file);
char *get_log_file (const char *filename);
void stat_add_write (statistics_t *stat, int len);
void stat_add_read (statistics_t *stat, int len);
char * type_of_str (contype_t type, char *buf);
void my_sleep (int microseconds);
void show_runtime_configuration ();
int is_recoverable (int error);
void set_run_path (char **argv);
void dispose_audiocast (audiocast_t *au);
int is_valid_http_request (ntrip_request_t *req);
void free_con (connection_t *con);
int count_clients();
char *get_arguments_of_type(com_request_t *req, char *type);
void sleep_random(int max);
long read_starttime();
void start_new_day();
int is_empty_request(ntrip_request_t *req);
int get_file_size(FILE *ifp);
transfer_encoding_t get_transfer_encoding(const char *var);
int is_big_endian();

list_t *list_create();
void list_dispose_with_data(list_t *l, ntripcaster_function *free_func);
void list_add(list_t *l, void *object);
void *list_get(list_t *l,int i);
void *list_next(list_enum_t *l);
list_enum_t *list_get_enum(list_t *l);
void list_reset(list_enum_t *l);

string_buffer_t *string_buffer_create(int size);
void dispose_string_buffer(string_buffer_t *sb);
int write_string_to_buffer(string_buffer_t *sb, char *string);
int write_line_to_buffer(string_buffer_t *sb, char *string);

int is_server_running(void);
void set_server_running(int state);

#ifndef _WIN32
#define min(x,y) ((x)<(y)?(x):(y))
#endif

#endif
