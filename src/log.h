/* log.h
 * - Logging Function Headers
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

#ifndef __NTRIPCASTER_LOG_H
#define __NTRIPCASTER_LOG_H

void write_log(int whichlog, char *fmt, ...);
void write_clf (connection_t *clicon, source_t *source);
void xa_debug (int level, char *fmt, ...);
void my_perror(char *where);
void stats_write(server_info_t *info);
void clear_logfile(char *logfilename);
//void redate_logfilenames();
int open_log_file (char *name, int oldfd);
void open_log_files ();
int fd_write (int fd, const char *fmt, ...);
int fd_read_line (int fd, char *buff, const int len);
int fd_read_line_nb (int fd, char *buff, const int len);
int fd_close (int fd);
int fd_write_line (int fd, const char *fmt, ...);
int fd_write_bytes (int fd, const char *buff, const int len);
void stats_write_html (server_info_t *info);
int get_log_fd (int whichlog);
void write_log_not_me (int whichlog, connection_t *nothim, char *fmt, ...);
void log_no_thread (int whichlog, char *fmt, ...);
void write_to_logfile (int whichlog, char *fmt, ...);
#endif
