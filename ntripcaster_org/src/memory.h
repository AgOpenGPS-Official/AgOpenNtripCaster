/* memory.h
 * - Memory management headers
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

#ifndef __NTRIPCASTER_MEMORY_H
#define __NTRIPCASTER_MEMORY_H

#ifdef HAVE_MCHECK_H
#include <mcheck.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#define nmalloc(x) n_malloc (x, __LINE__,__FILE__)
#define nfree(x) n_free (x,__LINE__,__FILE__) ; x=NULL
#define nstrdup(x) n_strdup (x,__LINE__,__FILE__)

typedef struct meminfo_t
{
  int line;
  int size;
  char file[20];
  void *ptr;
  int thread_id;
  time_t time;
} meminfo_t;

void *n_malloc (const unsigned int size, const int lineno, const char *file);
void n_free (const void *ptr, const int lineno, const char *file);
char *n_strdup (const char *ptr, const int lineno, const char *file);
char *ntripcaster_cat (const char *first, const char *second);
char *ntripcaster_itoa (int num);
char *ntripcaster_utoa (unsigned long int num);

void initialize_memory_checker ();

void *parser_malloc(size_t size);
void parser_free(void *ptr);

#ifdef HAVE_MCHECK_H
void ntripcaster_mcheck_status (enum mcheck_status STATUS);
#endif

#endif





