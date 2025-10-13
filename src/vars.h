/* vars.h
 * - function declarations for vars.c
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

void extract_vars (vartree_t *request_vars, char *requeststring);
void add_varpair (vartree_t *request_vars, char *varpair);
void add_varpair2 (vartree_t *request_vars, char *name, char *value);
const char *get_variable (vartree_t *request_vars, const char *name);
void free_variables (vartree_t *request_vars);
vartree_t *create_header_vars ();
void extract_header_vars (char *line, vartree_t *vars);
void free_con_variables (connection_t *con);
const char *get_con_variable (connection_t *con, const char *name);
char *get_all_vars_of_type (const char *type, vartree_t *request_vars);
void typecast_arguments(vartree_t *request_vars, com_request_t *comreq);
void nfree_typecasted_arguments(com_request_t *comreq);
const char *get_con_variable (connection_t *con, const char *name);
