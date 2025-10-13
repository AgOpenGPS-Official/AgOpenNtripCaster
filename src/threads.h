/* threads.h
 * - Thread Abstraction Function Headers
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

#ifndef __NTRIPCASTER_THREADS_H
#define __NTRIPCASTER_THREADS_H

#if defined (_WIN32)
# include <time.h>
# include <windows.h>
typedef DWORD icethread_t;
typedef CRITICAL_SECTION win32_mutex_t;
#else
#  if defined(HAVE_PTHREAD_H)
#   include <pthread.h>
#  elif defined(HAVE_PTHREAD_NP_H)
#   include <pthread_np.h>
#  endif
typedef pthread_t icethread_t;
#endif

typedef struct icemutex_St
{
  long int thread_id;
#ifndef _WIN32
  pthread_mutex_t mutex;
#else
  win32_mutex_t mutex;
#endif
  long int mutexid;
  int lineno;
  long int id;
} mutex_t;


#define thread_create(n,x,y) thread_create_c (n,x,y,__LINE__,__FILE__);
#define thread_create_mutex(x) thread_create_mutex_c (x,__LINE__,__FILE__);
#define thread_mutex_lock(x) thread_mutex_lock_c (x,__LINE__,__FILE__);
#define thread_mutex_unlock(x) thread_mutex_unlock_c (x,__LINE__,__FILE__);
#define thread_exit(x) thread_exit_c ((void *)x,__LINE__,__FILE__);

#define MUTEX_STATE_NOTLOCKED   -1
#define MUTEX_STATE_NEVERLOCKED -2
#define MUTEX_STATE_UNINIT -3

#define THREAD_CREATED -1
#define THREAD_RUNNING 1
#define THREAD_KILLED -2
#define THREAD_EXITED -3

typedef struct mythread_st
{
  icethread_t thread;
  int line;
  char *file;
  char *name;
  long int id;
  time_t created;
  int ping;
  int running;
} mythread_t;

void thread_lib_init();
icethread_t thread_create_c(char *name, void *(*start_routine)(void *), void *arg, int line, char *file);
void thread_create_mutex_c(mutex_t *mutex, int line, char *file);
void thread_mutex_lock_c(mutex_t *mutex, int line, char *file);
void thread_mutex_unlock_c(mutex_t *mutex, int line, char *file);
void thread_mutex_destroy (mutex_t *mutex);
void thread_exit_c(void *val, int line, char *file);
void internal_lock_mutex(mutex_t *mutex);
void internal_unlock_mutex(mutex_t *mutex);

/* added. ajd */
int thread_kill (int pid);
void thread_force_mutex_unlock(char *arg);

/*for using un-threadsafe library functions*/
void thread_library_lock();
void thread_library_unlock();
#define PROTECT_CODE(code) {thread_library_lock(); code ; thread_library_unlock();}

void thread_init();
void thread_wait_for_solitude ();

int thread_alive (mythread_t *mt);
void thread_block_signals();
void thread_catch_signals();
void thread_setup_default_attributes ();
icethread_t thread_self();
int thread_equal(icethread_t t1, icethread_t t2);
long thread_new();
long thread_mutex_new ();
void thread_create_mutex_nl (mutex_t *mutex);
mythread_t *thread_get_mythread();
mythread_t *thread_check_created();
void thread_mem_check (mythread_t *mt);
void thread_rename(const char *name); /* renames current thread */

#endif
