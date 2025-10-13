/* commandline.c
 * - Commandline parsing
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
#include "definitions.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "log.h"
#include "commands.h"
#include "memory.h"

#include "commandline.h"

extern server_info_t info;

void usage()
{
  printf("NtripCaster - Version %s\n", info.version);
  printf("----------------------------\n");
  printf("Usage:\n");
  printf("ntripcaster [-P <port>] [-p password] [-l <file>] [-d <directory>] [-c <configfile>] [-b]\n");
  printf("\n");
  printf("\tOptions explained (compiled default in parenthesis):\n");
  printf("\t-c: Configuration file to use (%s)\n", DEFAULT_CONFIG_FILE);
  printf("\t-P: port on which the server will listen for client connections (%d)\n", DEFAULT_PORT);
  printf("\t-p: password to validate encoders (%s)\n", DEFAULT_ENCODER_PASSWORD);
  printf("\t-l: filename for logging (%s)\n", DEFAULT_LOGFILENAME);
  printf("\t-b: Force NtripCaster server into the background\n");
  printf("\t-d: Use this directory as the location of the config files\n");
  printf("\n\n");
}

void
parse_directory_args_only (int argc, char **argv)
{
  int arg;
  char *s;

  arg = 1;

  if (!argv || !argv[0]) {
    write_log (LOG_DEFAULT, "WARNING: parse_directory_args_only() called with invalid argv");
    return;
  }

  xa_debug (1, "DEBUG: Parsing command line directory arguments");

  while (arg < argc) {
    s = argv[arg];

    if (s[0] == '-') {
      if ((s[1] == 'd') && arg >= (argc - 1))
      {
        fprintf (stderr, "Option %c requires an argument!\n", s[1]);
        exit (1);
      }
      switch (s[1]) {
        case 'd':
          arg++;
          if (info.etcdir)
          {
            nfree (info.etcdir);
          }
          info.etcdir = nstrdup(argv[arg]);
          break;
      }
    }
    arg++;
  }
}

void
parse_args(int argc, char **argv)
{
  int arg = 1;
  char *s;

  xa_debug (1, "DEBUG: Parsing command line arguments");

  while (arg < argc) {
    s = argv[arg];

    if (s[0] == '-') {
      if (s[1] != 'b' && s[1] != 'V' && s[1] != 'h' && arg >= (argc - 1))
      {
        fprintf (stderr, "Option %c requires an argument!\n", s[1]);
        exit (1);
      }
      switch (s[1]) {
        case 'c':
          arg++;
          if (info.configfile)
          {
            nfree (info.configfile);
          }
          info.configfile = nstrdup (argv[arg]);
          parse_config_file (info.configfile);
          break;
        case 'P':
          arg++;
          info.port[0] = atoi(argv[arg]);
          break;
/*        case 'p':
          arg++;
          if (info.encoder_pass)
          nfree (info.encoder_pass);
          info.encoder_pass = nstrdup (argv[arg]);
          break;*/
        case 'b':
          info.console_mode = CONSOLE_BACKGROUND;
          break;
        case 'l':
          arg++;
          if(info.logfilename)
          {
            nfree (info.logfilename);
          }
          info.logfilename = nstrdup (argv[arg]);
          break;
        case 'd':
          arg++;
          break;
        case 'V':
          info.logfiledebuglevel = 8;
          info.consoledebuglevel = 8;
          break;
        default:
          usage();
          exit(1);
      }
    } else {
      usage();
      exit(1);
    }
    arg++;
  }
}
