/* pool.c
 * - Connection pool functions
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
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#include "definitions.h"

#include <stdio.h>
#include <errno.h>
# ifndef __USE_BSD
#  define __USE_BSD
# endif
#ifndef __EXTENSIONS__
#define __EXTENSIONS__
#endif

#include <string.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include "avl.h"
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "log.h"
#include "pool.h"

extern server_info_t info;

/* The pool variables :) */
static mutex_t pool_mutex = {MUTEX_STATE_UNINIT};
static avl_tree *pool = NULL;

/* Initialize the connection pool.
 * No possible errors.
 * Assert Class: 1
 */
void
pool_init ()
{
  xa_debug (1, "DEBUG: Initializing Connection Pool.");
  thread_create_mutex (&pool_mutex);
  pool = avl_create (compare_connection, &info);
}

/* Shutdown the connection pool.
 * No possible errors.
 * Assert Class: 1
 */
void
pool_shutdown ()
{
  xa_debug (1, "DEBUG: Closing the pool.");
//  pool_cleaner ();
  avl_destroy (pool, NULL);
  xa_debug (1, "DEBUG: Pool closed.");
}

/*
 * Add a connection to the connection pool.
 * Possible error codes:
 * ICE_ERROR_NOT_INITIALIZED
 * ICE_ERROR_NULL - Argument was NULL
 * Assert Class: 3
 */
int
pool_add (connection_t *con)
{
  if (!con)
    return ICE_ERROR_NULL;

  if ((pool_mutex.thread_id == MUTEX_STATE_UNINIT) || pool == NULL) {
    xa_debug (1, "WARNING: Tried to use an unitialized pool");
    return ICE_ERROR_NOT_INITIALIZED;
  }

  /* Acquire mutex lock */
  pool_lock_write ();

  /* Throw connection into the pool */
  if (avl_replace (pool, con) != NULL)
    xa_debug (1, "WARNING: Duplicate connections in the pool (id = %d)", con->id);

  /* Release mutex lock */
  pool_unlock_write ();

  return OK;
}

/*
 * Called from a source, who wants to see if the pool has any connections
 * in stock for it.
 * Returns NULL on errors.
 * Assert Class: 3
 */
connection_t *
pool_get_my_clients (const source_t *source)
{
  avl_traverser trav = {0};
  connection_t *clicon = NULL;

  if (!source) {
    xa_debug (1, "WARNING: pool_get_my_clients() called with NULL source!");
    return NULL;
  }

  /* Acquire mutex lock */
  pool_lock_write ();

  /* Search for clients for this source */
  while ((clicon = avl_traverse (pool, &trav)))
    if (clicon->food.client->source == source)
      break;

  /* If found, remove it from the pool */
  if (clicon)
    if (avl_delete (pool, clicon) == NULL)
      xa_debug (1, "WARNING: pool_get_my_clients(): Connection Pool Security Comprimised!");

  /* Release mutex lock */
  pool_unlock_write ();

  return clicon;
}

/* We use internal_lock_mutex() here, because we trust
 * this code, and because this mutex is a "leaf" mutex.
 */
void
pool_lock_write ()
{
  assert (pool_mutex.thread_id != MUTEX_STATE_UNINIT);
  internal_lock_mutex (&pool_mutex);
}

void
pool_unlock_write ()
{
  assert (pool_mutex.thread_id != MUTEX_STATE_UNINIT);
  internal_unlock_mutex (&pool_mutex);
}

void
pool_cleaner ()
{

}






