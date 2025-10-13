/*
 * mount.h
 * - Function definitions for mount.c
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

void parse_mount_authentication_file(char *mountfilename, mounttree_t *mt);
mount_t *create_mount_from_line(char *line);
mount_t *create_mount();
mounttree_t *create_mount_tree();
void add_authentication_mount(mount_t * mount, mounttree_t *mt);
void free_mount_tree(mounttree_t * mt);
grouptree_t *get_grouptree_for_mount(const char *mountname, mounttree_t *mt);
void con_display_mounts(com_request_t * req, mounttree_t *mt);
void html_display_mounts(com_request_t *req, mounttree_t *mt);
int runtime_add_mount(const char *name, char *mountfile, mounttree_t *mt);
int runtime_add_mount_with_group(const char *name, char *groups, char *mountfile, mounttree_t *mt);
mounttree_t *get_client_mounttree();
mounttree_t *get_source_mounttree();
