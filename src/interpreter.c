/* interpreter.c
 * - Interpreter functions
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

#ifdef HAVE_PYTHON_H
#include <Python.h>
#endif

#include "avl.h"
#include "avl_functions.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "log.h"
#include "interpreter.h"

extern server_info_t info;

#ifdef HAVE_LIBPYTHON
static PyThreadState *mainthreadstate = NULL;
#include "python_api.c"
#endif

void
interpreter_init ()
{
#ifdef HAVE_LIBPYTHON
  interpreter_python_init();
#endif
}

void
interpreter_shutdown ()
{
#ifdef HAVE_LIBPYTHON
  interpreter_python_shutdown ();
#endif
}

#ifdef HAVE_LIBPYTHON
void
interpreter_python_init ()
{
  Py_Initialize ();
  PyEval_InitThreads ();

  mainthreadstate = PyThreadState_Get ();
  ntripcaster_python_init ();
  PyEval_ReleaseLock ();
}

void
interpreter_python_shutdown ()
{
  PyEval_AcquireLock ();
  Py_Finalize ();
}

PyThreadState *
interpreter_python_init_thread ()
{
  PyInterpreterState *maininterpreterstate = NULL;
  PyThreadState *newthreadstate;

  PyEval_AcquireLock ();

  maininterpreterstate = mainthreadstate->interp;
  newthreadstate = PyThreadState_New (maininterpreterstate);

  PyEval_ReleaseLock ();

  return newthreadstate;
}

void
interpreter_python_shutdown_thread (PyThreadState *threadstate)
{
  PyEval_AcquireLock ();

  PyThreadState_Clear (threadstate);

  PyThreadState_Delete (threadstate);

  PyEval_ReleaseLock ();
}

void
interpreter_python_eval_file (char *filename)
{
  PyThreadState *threadstate = interpreter_python_init_thread ();

  PyEval_AcquireLock ();

  PyThreadState_Swap (threadstate);

  xa_debug (1, "DEBUG: Interpreting [%s]", filename);

  PyRun_SimpleFile (fopen (filename, "r"), filename);

  PyThreadState_Swap (NULL);

  PyEval_ReleaseLock ();

  xa_debug (1, "DEBUG: Done interpreting [%s]", filename);

  interpreter_python_shutdown_thread (threadstate);
}

#endif





