/*
 * basic.h
 * - Function definitions for basic.c
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
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

typedef avl_tree usertree_t;
typedef avl_tree grouptree_t;
typedef avl_tree mounttree_t;

typedef struct userSt {
  char *name;
  char *pass;
} ntripcaster_user_t;

typedef struct groupSt {
  char *name;
  int max_num_con; // maximal number of allowed simultaneous connections
  int max_num_ip; // maximal number of allowed simultaneous connections per ip
  usertree_t *usertree;
} group_t;

typedef struct mountSt {
  char *name;
  grouptree_t *grouptree;
} mount_t;

void init_authentication_scheme(void);
void parse_authentication_scheme(void);
void destroy_authentication_scheme(void);
void cleanup_authentication_scheme(void);
int authenticate_user_request(connection_t *con, ntrip_request_t *req, contype_t contype);
int authenticate_user_request_ntrip1upload(connection_t *con, ntrip_request_t *req, const char *pwd);
void rehash_authentication_scheme(void);
mount_t *need_authentication(ntrip_request_t * req, mounttree_t *mt);
mount_t *need_authentication_with_mutex(ntrip_request_t * req, mounttree_t *mt);
int check_ip_restrictions(connection_t *con);
int add_group_connection(connection_t *con);
void remove_group_connection(connection_t *con);
