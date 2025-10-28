/* memory.c
 * - Memory management functions
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

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include "win32config.h"
#else
#include "config.h"
#endif
#endif

#include "definitions.h"

#include <stdio.h>

#ifndef __USE_BSD
#define __USE_BSD
#endif

#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#include <string.h>
#include <sys/types.h>

#include <time.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcaster.h"
#include "ntripcastertypes.h"
#include "utility.h"
#include "ntripcaster_string.h"

#include "log.h"
#include "logtime.h"

#include "memory.h"
#include "main.h"

extern server_info_t info;

#ifdef HAVE_MCHECK

void
ntripcaster_mcheck_status (enum mcheck_status STATUS)
{
  fprintf (stderr, "WARNING MEMORY INTEGRITY COMPRIMISED!!!\n");
  switch (STATUS)
  {
    case MCHECK_HEAD:
      fprintf (stderr, "MCHECK_HEAD (pointer decremented to far)\n");
      break;
    case MCHECK_TAIL:
      fprintf (stderr, "MCHECK_TAIL (pointer incremented to far)\n");
      break;
    case MCHECK_FREE:
      fprintf (stderr, "MCHECK_FREE (block already free)\n");
      break;
    default:
      fprintf (stderr, "Unknown mcheck status\n");
      break;
  }
}

#endif

#ifdef DEBUG_MEMORY
/*
 * Create a dynamic memory info struct by calling malloc()
 * Set default values and return the created meminfo_t
 * Assert Class: 1
 */
meminfo_t *
create_meminfo ()
{
  meminfo_t *out = (meminfo_t *) malloc (sizeof (meminfo_t));
  if (!out)
    return NULL; /* What else? */
  out->ptr = NULL;
  out->file[0] = '\0';
  out->line = -1;
  out->thread_id = -1;
  out->size = -1;
  out->time = -1;
  return out;
}
#endif

/*
 * Dynamically allocate size bytes and return the chunk
 * Assert Class: 1
 */
void *
n_malloc (const unsigned int size, const int lineno, const char *file)
{
  void *buf;

  if (size <= 0)
  {
    fprintf (stderr, "WARNING: n_malloc called with negative or zero size\n");
    return NULL;
  }

  buf = malloc (size);

  if (buf == NULL) {
    fprintf (stderr, "OUCH, out of memory!");
    clean_resync (&info);
  }

#ifdef DEBUG_MEMORY
  {
    meminfo_t *mi;
    mythread_t *mt = thread_get_mythread ();
    mi = create_meminfo();

    if (!mi)
      return buf;

    mi->line = lineno;
    strncpy (mi->file, file ? file : "unknown", 19);
    mi->ptr = buf;
    mi->time = get_time ();
    mi->size = size;
    if (mt)
      mi->thread_id = mt->id;
    internal_lock_mutex (&info.memory_mutex);
    avl_insert (info.mem, mi);
    internal_unlock_mutex (&info.memory_mutex);
  }
#endif
  return buf;
}

/*
 * Create a dynamically allocated string with the same data as ptr
 * Assert Class: 1
 */
char *
n_strdup (const char *ptr, const int lineno, const char *file)
{
  char *buf;

  if (!ptr)
  {
    ptr = "(null)";
  }

  buf = strdup (ptr);
#ifdef DEBUG_MEMORY
  {
    meminfo_t *mi;
    mythread_t *mt = thread_get_mythread();
    mi = create_meminfo();
    mi->line = lineno;
    strncpy(mi->file, file, 19);
    mi->ptr = buf;
    mi->size = ntripcaster_strlen (ptr) + 1;
    mi->time = get_time ();
    if (mt)
      mi->thread_id = mt->id;
    internal_lock_mutex (&info.memory_mutex);
    avl_insert(info.mem, mi);
    internal_unlock_mutex (&info.memory_mutex);
  }
#endif

  return buf;
}

/*
 * free the memory chunk pointed to by ptr
 * Assert Class: 1
 */
void
n_free (const void *ptr, const int lineno, const char *file)
{
#ifdef DEBUG_MEMORY
  meminfo_t search, *out;
  search.ptr = ptr;
  internal_lock_mutex (&info.memory_mutex);
  out = avl_delete (info.mem, &search);
  internal_unlock_mutex (&info.memory_mutex);

  if (!out && ptr)
  {
    write_log (LOG_DEFAULT, "Couldn't find alloced memory at (%p)", ptr);
    return;
  }

  if (out)
    free (out);
#endif

  if (ptr)
    free ((void *)ptr);
}

char *
ntripcaster_cat (const char *first, const char *second)
{
  size_t sz = ntripcaster_strlen(first) + ntripcaster_strlen(second) + 1;
  char *res = (char *)nmalloc(sz);
  snprintf(res, sz, "%s%s", first, second);
  return res;
}

int
bytes_for (int bytes)
{
  return bytes * (int)(8 * (log(2) / (log(10)))) + 2;
}

char *
ntripcaster_itoa (int num)
{
  size_t sz = bytes_for(sizeof(int));
  char *res = (char *)nmalloc(sz);
  snprintf(res, sz, "%d", num);
  return res;
}

char *
ntripcaster_utoa (unsigned long int num)
{
  size_t sz = bytes_for(sizeof(unsigned long int));
  char *res = (char *)nmalloc(sz);
  snprintf(res, sz, "%lu", num);
  return res;
}

void
initialize_memory_checker ()
{
#if defined(DEBUG_MEMORY_MCHECK) && defined(HAVE_MCHECK)
  mcheck (ntripcaster_mcheck_status);
  mtrace();
  fprintf (stderr, "DEBUG: Starting memory checker\n");
#endif
}

void *parser_malloc(size_t size) {
  return nmalloc(size);
}

void parser_free(void *ptr) {
  nfree(ptr);
}
