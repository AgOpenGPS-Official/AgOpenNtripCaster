/* restrict.h
 * - acl list function headers
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

#ifndef NTRIPCASTER_RESTRICT_H
#define NTRIPCASTER_RESTRICT_H

void list_rule (com_request_t *req, restrict_t *res);
int list_restrict (com_request_t *req, avl_tree *tree, acltype_t type);
void list_acl_control (com_request_t *req, avl_tree *acl, acltype_t type);
void list_all_acls (com_request_t *req);
restrict_t *create_restrict ();
restrict_t *add_restrict (avl_tree *tree, char *mask, acltype_t type);
int del_restrict (avl_tree *tree, char *name, acltype_t type);
int allowed (connection_t *con, contype_t contype);
avl_tree *get_acl_list (contype_t contype);
int restrict_list (connection_t *con, avl_tree *list);
void free_acl_lists ();
void free_acl_list (avl_tree *list);
int allowed_no_policy (connection_t *con, contype_t contype);
#endif
