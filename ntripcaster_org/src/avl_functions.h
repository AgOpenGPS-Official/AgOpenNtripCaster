/* avl_functions.h
 * - Avl Functions Headers
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

#ifndef __AVL_FUNCTIONS_H
#define __AVL_FUNCTIONS_H

int compare_groups (const void *first, const void *second, void *param);
int compare_users (const void *first, const void *second, void *param);
int compare_mounts (const void *first, const void *second, void *param);
int compare_restricts (const void *first, const void *second, void *param);
int compare_statisticsentry (const void *first, const void *second, void *param);
int compare_vars (const void *first, const void *second, void *param);
int compare_header_vars (const void *first, const void *second, void *param);
int compare_strings (const void *first, const void *second, void *param);
int compare_connection(const void *first, const void *second, void *param);
int compare_aliases(const void *first, const void *second, void *param);
int compare_threads(const void *first, const void *second, void *param);
int compare_mutexes(const void *first, const void *second, void *param);
int compare_directories(const void *first, const void *second, void *param);
int compare_relays(const void *first, const void *second, void *param);
int compare_mem (const void *first, const void *second, void *param);
int compare_item (const void *first, const void *second, void *param);
int compare_sockets (const void *first, const void *second, void *param);
int compare_sourcetable_entrys (const void *first, const void *second, void *param);
int compare_sessions (const void *first, const void *second, void *param);
int compare_header_elements (const void *first, const void *second, void *param);
int compare_messages (const void *first, const void *second, void *param);
int compare_nontrip_sources (const void *first, const void *second, void *param); // nontrip. ajd

void free_connection(void *data, void *param);
void zero_trav(avl_traverser *trav);
void *avl_get_any_node (avl_tree *tree);
#endif
