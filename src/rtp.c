/* rtp.c
 * - RTP functions
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

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdlib.h>
#include <stdarg.h>
# ifndef __USE_BSD
#  define __USE_BSD
# endif
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#if defined (_WIN32)
#include <windows.h>
#define strncasecmp strnicmp
#else
#include <sys/socket.h> 
#include <sys/wait.h>
#include <netinet/in.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "sock.h"
#include "ntrip.h"
#include "rtsp.h"
#include "rtp.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "client.h"
#include "connection.h"
#include "log.h"
#include "source.h"
#include "memory.h"

rtp_t *rtp_create() {
  rtp_t *rtp;

  rtp = (rtp_t *)nmalloc(sizeof(rtp_t));
//  rtp->senddata = (rtp_datagram_t *)nmalloc(sizeof(rtp_datagram_t));
  rtp_init(rtp);

  return rtp;
}

void rtp_init(rtp_t *rtp) {
  int i;

  rtp->datagram = (rtp_datagram_t *)nmalloc(sizeof(rtp_datagram_t));
  rtp->datagram->version = 2;
  rtp->datagram->p = 0;
  rtp->datagram->x = 0;
  rtp->datagram->cc = 0;
  rtp->datagram->m = 0;
  rtp->datagram->pt = 96;
  rtp->datagram->ssrc = htonl((u_int32)rand());
  rtp->datagram->data_len = -1;

  rtp->host_seq = rand();
  rtp->last_host_seq = -1;
  rtp->host_ts = (u_int32)rand();
  rtp->sendtime.tv_sec = 0;
  rtp->sendtime.tv_usec = 0;
  rtp->virgin = 1;
  rtp->offset = 0;
  for (i=0; i<DATAGRAMBUFSIZE; i++) {
    rtp->datagrambuf[i] = NULL;
  }
}

void rtp_free(rtp_t *rtp) {
  int i;
  for (i=0; i<DATAGRAMBUFSIZE; i++) {
    if (rtp->datagrambuf[i] != NULL)
    {
      nfree(rtp->datagrambuf[i]);
    }
  }
  nfree (rtp->datagram);
}

void rtp_prepare_send(rtp_t *rtp) {
  struct timeval now;
  long udiff;

  gettimeofday(&now, NULL);

  udiff = (now.tv_usec - rtp->sendtime.tv_usec + ((now.tv_sec - rtp->sendtime.tv_sec)*1000000));

  rtp->host_seq++;
  rtp->host_ts = rtp->host_ts + (u_int32)(udiff/TIMESTAMP_RESOLUTION);
  rtp->datagram->seq = htons(rtp->host_seq);
  rtp->datagram->ts = htonl(rtp->host_ts);

  rtp->sendtime.tv_sec = now.tv_sec;
  rtp->sendtime.tv_usec = now.tv_usec;
}

int rtp_recieve_datagram_buffered(connection_t *con) {
  int diff;
  int len = -1;
  int i;

  if (con->rtp->datagrambuf[con->rtp->offset] != NULL) { /* found previously recieved datagram in buffer, use it */
    nfree(con->rtp->datagram);
    con->rtp->datagram = con->rtp->datagrambuf[con->rtp->offset];
    con->rtp->datagrambuf[con->rtp->offset] = NULL;
    len = con->rtp->datagram->data_len;
    con->rtp->offset = (con->rtp->offset+1)%DATAGRAMBUFSIZE;
    con->rtp->host_seq = ntohs(con->rtp->datagram->seq);
    con->rtp->last_host_seq = con->rtp->host_seq;

    xa_debug (4, "DEBUG: Previously recieved datagram found: seq: %d", con->rtp->host_seq);

  } else {
    len = recv(con->sock, con->rtp->datagram, MAXUDPSIZE+12, 0)-12;
    if (len > 0) {
      con->rtp->host_seq = ntohs(con->rtp->datagram->seq);
      con->rtp->datagram->data_len = len;
      if (con->rtp->virgin == 1) {

        xa_debug (4, "DEBUG: First UDP DATAGRAM recieved: seq: %d, last_seq: %d", con->rtp->host_seq, con->rtp->last_host_seq);

        con->rtp->offset = (con->rtp->offset+1)%DATAGRAMBUFSIZE;
        con->rtp->last_host_seq = con->rtp->host_seq;
        con->rtp->virgin = 0;
      } else {

/* PROBLEM: if (last_host_seq >= host_seq), solved by constraining (diff < 32768)
[10/Jan/2007:16:55:39] UDP DATAGRAM recieved in order: seq: 20, last_seq: 19
[10/Jan/2007:16:55:40] WARNING: UDP DATAGRAM out of order: seq: 22, last_seq: 20
[10/Jan/2007:16:55:40] Putting datagram with seq 22 (offset=5) into bufferpos 6
[10/Jan/2007:16:55:41] WARNING: UDP DATAGRAM out of order: seq: 27, last_seq: 20
[10/Jan/2007:16:55:41] Putting datagram with seq 27 (offset=5) into bufferpos 3
[10/Jan/2007:16:55:42] WARNING: UDP DATAGRAM out of order: seq: 25, last_seq: 20
[10/Jan/2007:16:55:42] Putting datagram with seq 25 (offset=5) into bufferpos 1
[10/Jan/2007:16:55:43] WARNING: UDP DATAGRAM out of order: seq: 29, last_seq: 20
---> [10/Jan/2007:16:55:43] WARNING: buffer overflow!!!!! deleting 1 bufpositions

[10/Jan/2007:16:55:43] Putting datagram with seq 29 (offset=6) into bufferpos 5
[10/Jan/2007:16:55:43] Previously recieved datagram found: seq: 22
[10/Jan/2007:16:55:44] WARNING: UDP DATAGRAM out of order: seq: 24, last_seq: 22
[10/Jan/2007:16:55:44] Putting datagram with seq 24 (offset=7) into bufferpos 0

and then after deletion of bufposition earlyer datagram 21 arrives
---> [10/Jan/2007:16:55:45] WARNING: UDP DATAGRAM out of order: seq: 21, last_seq: 22
[10/Jan/2007:16:55:45] WARNING: buffer overflow!!!!! deleting 65527 bufpositions

--->[10/Jan/2007:16:55:45] Putting datagram with seq 21 (offset=0) into bufferpos 7

[10/Jan/2007:16:55:46] WARNING: UDP DATAGRAM out of order: seq: 26, last_seq: 13
[10/Jan/2007:16:55:46] WARNING: buffer overflow!!!!! deleting 5 bufpositions
[10/Jan/2007:16:55:46] Putting datagram with seq 26 (offset=5) into bufferpos 4
[10/Jan/2007:16:55:47] WARNING: UDP DATAGRAM out of order: seq: 28, last_seq: 18
*/

        diff = (65536 - (con->rtp->last_host_seq - con->rtp->host_seq))%65536;
        if (diff == 1) { /* recieved datagram in order */

          xa_debug (4, "DEBUG: UDP DATAGRAM recieved in order: seq: %d, last_seq: %d", con->rtp->host_seq, con->rtp->last_host_seq);

          con->rtp->offset = (con->rtp->offset+1)%DATAGRAMBUFSIZE;
          con->rtp->last_host_seq = con->rtp->host_seq;
        } else if ((diff > 1) && (diff < 32768)) { /* recieved datagram out of order -> put it into the buffer */

          xa_debug (4, "DEBUG: UDP DATAGRAM out of order: seq: %d, last_seq: %d", con->rtp->host_seq, con->rtp->last_host_seq);

          diff--;
          if (diff >= DATAGRAMBUFSIZE) { /* BUFFER OVERFLOW ->  */

            xa_debug (4, "DEBUG: WARNING: buffer overflow!!!!! deleting %d bufpositions", (diff-DATAGRAMBUFSIZE+1));

            for (i=0; i < (diff-DATAGRAMBUFSIZE+1); i++) {
              if (con->rtp->datagrambuf[con->rtp->offset] != NULL) {
                nfree(con->rtp->datagrambuf[con->rtp->offset]);
                con->rtp->datagrambuf[con->rtp->offset] = NULL;
              }
              con->rtp->offset = (con->rtp->offset+1)%DATAGRAMBUFSIZE;
              if (i >= DATAGRAMBUFSIZE) break;
            }
            con->rtp->last_host_seq += (diff-DATAGRAMBUFSIZE+1);
            diff = DATAGRAMBUFSIZE-1;
          }

          if (con->rtp->datagrambuf[(con->rtp->offset+diff)%DATAGRAMBUFSIZE] != NULL) {
            nfree(con->rtp->datagrambuf[(con->rtp->offset+diff)%DATAGRAMBUFSIZE]);
          }

          xa_debug (4, "DEBUG: Putting datagram with seq %d (offset=%d) into buffer pos %d", con->rtp->host_seq, con->rtp->offset, ((con->rtp->offset+diff)%DATAGRAMBUFSIZE) );

          /* write recieved datagram into buffer */
          con->rtp->datagrambuf[(con->rtp->offset+diff)%DATAGRAMBUFSIZE] = (rtp_datagram_t *)nmalloc(sizeof(rtp_datagram_t));
          memcpy(con->rtp->datagrambuf[(con->rtp->offset+diff)%DATAGRAMBUFSIZE], con->rtp->datagram, sizeof(rtp_datagram_t));
          len = -1;
        } else {
          xa_debug (4, "DEBUG: UDP DATAGRAM ignored: seq: %d, last_seq: %d", con->rtp->host_seq, con->rtp->last_host_seq);

          len = -1;
        }
      }
    }
  }
  
  return len;
}
