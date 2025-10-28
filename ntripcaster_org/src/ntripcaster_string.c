/* string.c
 * - Utilities to manipulate character data
 *
 * Copyright (c) 2023
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

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <ctype.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif

#include "avl.h"
#include "threads.h"
#include "ntripcaster.h"
#include "ntripcastertypes.h"
#include "utility.h"
#include "ntripcaster_string.h"
#include "sock.h"
#include "log.h"
#include "logtime.h"
#include "memory.h"
#include "ntripcaster_string.h"

extern server_info_t info;
extern mutex_t authentication_mutex, library_mutex, sock_mutex;

const char const_null[] = "(null)";

/* this array contains ASCII characters and their URL code sequences
 * from 32 (0x20) to 126 (0x7E), i.e. array position i corresponds
 * to ascii character i+32. ajd */
http_special_char_t http_special_char[] =
{
{' ',"%20"}, {'!',"%21"}, {'"',"%22"}, {'#',"%23"}, {'$',"%24"}, {'%',"%25"}, {'&',"%26"}, {'\'',"%27"}, {'(',"%28"},
{')',"%29"}, {'*',"%2A"}, {'+',"%2B"}, {',',"%2C"}, {'-',"%2D"}, {'.',"%2E"}, {'/',"%2F"}, {'0',"%30"}, {'1',"%31"},
{'2',"%32"}, {'3',"%33"}, {'4',"%34"}, {'5',"%35"}, {'6',"%36"}, {'7',"%37"}, {'8',"%38"}, {'9',"%39"}, {':',"%3A"},
{';',"%3B"}, {'<',"%3C"}, {'=',"%3D"}, {'>',"%3E"}, {'?',"%3F"}, {'@',"%40"}, {'A',"%41"}, {'B',"%42"}, {'C',"%43"},
{'D',"%44"}, {'E',"%45"}, {'F',"%46"}, {'G',"%47"}, {'H',"%48"}, {'I',"%49"}, {'J',"%4A"}, {'K',"%4B"},{'L',"%4C"},
{'M',"%4D"}, {'N',"%4E"}, {'O',"%4F"}, {'P',"%50"}, {'Q',"%51"}, {'R',"%52"}, {'S',"%53"}, {'T',"%54"}, {'U',"%55"},
{'V',"%56"}, {'W',"%57"}, {'X',"%58"}, {'Y',"%59"}, {'Z',"%5A"}, {'[',"%5B"}, {'\\',"%5C"}, {']',"%5D"}, {'^',"%5E"},
{'_',"%5F"}, {'`',"%60"}, {'a',"%61"}, {'b',"%62"}, {'c',"%63"}, {'d',"%64"}, {'e',"%65"}, {'f',"%66"}, {'g',"%67"},
{'h',"%68"}, {'i',"%69"}, {'j',"%6A"}, {'k',"%6B"}, {'l',"%6C"}, {'m',"%6D"}, {'n',"%6E"}, {'o',"%6F"}, {'p',"%70"},
{'q',"%71"}, {'r',"%72"}, {'s',"%73"}, {'t',"%74"}, {'u',"%75"}, {'v',"%76"}, {'w',"%77"}, {'x',"%78"}, {'y',"%79"},
{'z',"%7A"}, {'{',"%7B"}, {'|',"%7C"}, {'}',"%7D"}, {'~',"%7E"}
};

char *
splitc (char *first, char *rest, const char divider)
{
  char *p;

  if (!rest)
  {
    write_log (LOG_DEFAULT, "WARNING: splitc called with NULL pointers");
    return NULL;
  }

  p = strchr(rest, divider);
  if (p == NULL) {
    if ((first != rest) && (first != NULL)) first[0] = 0;
    return NULL;
  }

  *p = 0;
  if (first != NULL) strcpy(first, rest);
  if (first != rest)
  {
    char *x = rest;
    while(*(++p))
      *(x++) = *p;
    *x = 0;
  }
  //if (first != rest) strcpy(rest, p + 1); // causes memory error in valgrind. ajd

  return rest;
}

/* to get the n-th part string of a string divided multiple times by 'divider'. ajd */
char *
splitnc (char *buf, char *string, const char divider, int n)
{
  int i;
  char *start, *end;

  if ((!string) || (!buf))
  {
    write_log (LOG_DEFAULT, "WARNING: splitnc called with NULL pointers");
    return NULL;
  }

  start = string;
  end = strchr(start, divider);

  for (i=1; i<n; i++) {
    if (end != NULL) {
      start = end + 1 ;
      end = strchr(start, divider);
    } else start = NULL;
  }

  if (start == NULL) {
    if ((buf != string) && (buf != NULL)) {
      *buf = 0;
    }
    return NULL;
  }

  if (end == NULL) {
    strcpy(buf, start);
    *string = 0;
  } else {
    *end = 0;
    strcpy(buf, start);
    if (buf != string) strcpy(string, end + 1); // causes memory error in valgrind. ajd
  }

  return string;
}


char *
clean_string(char *string)
{
  register unsigned i;

  i = 0;
  while (string[i] == ' ' && string[i] != '\0')
    i++;

  return &string[i];
}

void
flags2string(admin_t *adm, void *param)
{
  char fls[5] = "";
  if (adm->oper)
    strcat (fls, "O");
  if (adm->tailing)
    strcat (fls, "T");
  if (adm->status)
    strcat (fls, "S");
  if (param)
    sock_write (*(sock_t *)param, "%s", fls);
}

int
is_pattern (const char *string)
{
  if (strchr (string, '*'))
    return 1;
  else if (strchr (string, '?'))
    return 1;
  else if (strchr (string, '.'))
    return 1;
  return 0;
}

int
is_number (const char *string)
{
  int i, length;
  if ((length = ntripcaster_strlen(string)) == 0) {
    return 0;
  }

  for(i=0; i < length; i++) {
    if (isalpha((int)string[i]) != 0)
      return 0;
  }
  return 1;
}

const char *con_host (connection_t *con) {

  if (!con) {
    write_log (LOG_DEFAULT, "WARNING: con_host called with NULL connection");
    return const_null;
  }

  if (con->hostname)
    return con->hostname;
  else if (con->host)
    return con->host;

  return const_null;
}

char *
my_strdup (const char *string)
{
  const char *ptr = string;
  if (!string)
  {
    xa_debug (1, "DEBUG: my_strdup called with NULL pointer!");
    return NULL;
  }
  while (ptr && *ptr && *ptr == ' ')
    ptr++;
  return nstrdup (ptr);
}

char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *util_base64_encode(char *message)
{
  char *encoded;
  unsigned long length, encoded_length;
  unsigned long left, bitqueue, i = 0, j = 0;

  length = ntripcaster_strlen(message);

  if (length == 0) return NULL;

  encoded_length = (4 * (length + ((3 - (length % 3)) % 3)) / 3);
  encoded = (char *)nmalloc(encoded_length + 1);

  while (i < length) {
    left = length - i;

    if (left > 2) {
      bitqueue = message[i++];
      bitqueue = (bitqueue << 8) + message[i++];
      bitqueue = (bitqueue << 8) + message[i++];

      encoded[j++] = alphabet[(bitqueue & 0xFC0000) >> 18];
      encoded[j++] = alphabet[(bitqueue & 0x3F000) >> 12];
      encoded[j++] = alphabet[(bitqueue & 0xFC0) >> 6];
      encoded[j++] = alphabet[bitqueue & 0x3F];
    } else if (left == 2) {
      bitqueue = message[i++];
      bitqueue = (bitqueue << 8) + message[i++];
      bitqueue <<= 8;

      encoded[j++] = alphabet[(bitqueue & 0xFC0000) >> 18];
      encoded[j++] = alphabet[(bitqueue & 0x3F000) >> 12];
      encoded[j++] = alphabet[(bitqueue & 0xFC0) >> 6];
      encoded[j++] = '=';
    } else {
      bitqueue = message[i++];
      bitqueue <<= 16;

      encoded[j++] = alphabet[(bitqueue & 0xFC0000) >> 18];
      encoded[j++] = alphabet[(bitqueue & 0x3F000) >> 12];
      encoded[j++] = '=';
      encoded[j++] = '=';
    }
  }

  encoded[encoded_length] = '\0'; // added. ajd

  return encoded;
}

char unalphabet(char alpha)
{
  if (alpha >= 'A' && alpha <= 'Z')
    return (alpha - 'A');
  else if (alpha >= 'a' && alpha <= 'z')
    return (alpha - 'a' + 26);
  else if (alpha >= '0' && alpha <= '9')
    return (alpha - '0' + 52);
  else if (alpha == '+')
    return 62;
  else if (alpha == '/')
    return 63;
  else if (alpha == '=')
    return 64;
  else
    return 65;
}

char *
util_base64_decode(char *message)
{
  char *decoded, temp;
  long length, decoded_length;
  long bitqueue, pad, i = 0, j = 0;

  length = ntripcaster_strlen(message);

  if (((length % 4) != 0) || (length == 0)) return NULL;

  decoded_length = length / 4 * 3;

  if (message[length - 1] == '=') {
    decoded_length--;
    if (message[length - 2] == '=')
      decoded_length--;
  }

  decoded = (char *)nmalloc(decoded_length + 1);
  memset (decoded, 0, decoded_length + 1);

  while (i < length) {
    pad = 0;

    temp = unalphabet(message[i++]);
    if (temp == 64) {
      free(decoded);
      return NULL;
    }
    bitqueue = temp;

    temp = unalphabet(message[i++]);
    if (temp == 64) {
      free(decoded);
      return NULL;
    }
    bitqueue <<= 6;
    bitqueue += temp;

    temp = unalphabet(message[i++]);
    if (temp == 64) {
      if (i != length - 1) {
        free(decoded);
        return NULL;
      }
      temp = 0; pad++;
    }
    bitqueue <<= 6;
    bitqueue += temp;

    temp = unalphabet(message[i++]);
    if (pad == 1 && temp != 64) {
        free(decoded);
        return NULL;
    }

    if (temp == 64) {
      if (i != length) {
        free(decoded);
        return NULL;
      }
      temp = 0; pad++;
    }
    bitqueue <<= 6;
    bitqueue += temp;

    decoded[j++] = ((bitqueue & 0xFF0000) >> 16);
    if (pad < 2) {
      decoded[j++] = ((bitqueue & 0xFF00) >> 8);
      if (pad < 1)
        decoded[j++] = (bitqueue & 0xFF);
    }
  }

  decoded[decoded_length] = '\0';

  return decoded;
}

char *safe_strcat(char *dest, const char *src, unsigned int maxsize)
{
  // int size = 0; // not used. ajd

  if (!dest || !src) return dest;

  if (ntripcaster_strlen(dest) + ntripcaster_strlen(src)  + 1 >= maxsize) {
    if (ntripcaster_strlen(dest) + 1 >= maxsize) {
      /* no more room */
      return dest;
    } else {
      return strncat(dest, src, maxsize - (ntripcaster_strlen(dest) + 1));
    }
  } else {
    return strcat(dest, src);
  }
}

char *
mutex_to_string (mutex_t *mutex, char *out)
{
  if (mutex == &info.source_mutex)
    strcpy (out, "Source Tree Mutex");
  else if (mutex == &info.relay_mutex)
    strcpy (out, "Relay Tree Mutex");
  else if (mutex == &info.admin_mutex)
    strcpy (out, "Admin Tree Mutex");
  else if (mutex == &info.alias_mutex)
    strcpy (out, "Alias Tree Mutex");
  else if (mutex == &info.misc_mutex)
    strcpy (out, "Misc. Mutex");
  else if (mutex == &info.hostname_mutex)
    strcpy (out, "Hostname Tree Mutex");
  else if (mutex == &info.acl_mutex)
    strcpy (out, "ACL Tree Mutex");
  else if (mutex == &info.double_mutex)
    strcpy (out, "Double Mutex Mutex");
  else if (mutex == &info.thread_mutex)
    strcpy (out, "Thread Tree Mutex");
#ifdef DEBUG_MEMORY
  else if (mutex == &info.memory_mutex)
    strcpy (out, "Memory Tree Mutex");
#endif
#ifdef DEBUG_SOCKETS
  else if (mutex == &sock_mutex) {
    strcpy (out, "Socket Tree Mutex");
  }
#endif
  else if (mutex == &info.resolvmutex)
    strcpy (out, "DNS Lookup Mutex");
  else if (mutex == &library_mutex)
    strcpy (out, "Library Mutex");
  else if (mutex == &authentication_mutex)
    strcpy (out, "Authentication Mutex");
  else
    strcpy (out, "Unknown Mutex (probably source)");


  return out;
}

const char *
skip_before (const char *ptr, const char *search)
{
  return (const char *)strstr(ptr, search);
}

const char *
skip_after (const char *ptr, const char *search)
{
  char *hit = strstr (ptr, search);
  if (hit)
    return (const char *)hit + ntripcaster_strlen (search);
  else
    return NULL;
}

char *create_malloced_ascii_host (struct in_addr *in)
{
  char *buf = (char *)nmalloc(20);

  if (!in) {
    xa_debug(1, "ERROR: Dammit, don't send NULL's to create_malloced_ascii_host()");
    return NULL;
  }

  return makeasciihost(in, buf);
}

char *makeasciihost(const struct in_addr *in, char *buf)
{
  if (!buf) {
    write_log(LOG_DEFAULT, "ERROR: makeasciihost called with NULL arguments");
    return NULL;
  }

#ifdef HAVE_INET_NTOA

  /* Argh! How do we know what size "buf" is?  - dave@jetcafe.org
   * I'll have to make an educated guess. */
  strncpy(buf, inet_ntoa(*in), 20);

#else

  unsigned char *s = (unsigned char *)in;
  int a, b, c, d;
  a = (int)*s++;
  b = (int)*s++;
  c = (int)*s++;
  d = (int)*s;

  /* guessing here about size of buf */
  snprintf(buf, 20, "%d.%d.%d.%d", a, b, c, d);

#endif

  return buf;
}

char *
nntripcaster_time_minutes (unsigned long int minutes, char *buf)
{
  unsigned long int days, hours, remains;
//  char buf2[BUFSIZE];

  if (!buf)
  {
    write_log (LOG_DEFAULT, "ERROR: nntripcaster_time_minutes called with NULL argument");
    return NULL;
  }

  buf[0] = '\0';

  days = minutes / 1440;
  remains = minutes % 1440;
  hours = remains / 60;
  remains = remains % 60;

  if (days > 0)
    snprintf(buf, BUFSIZE, "%lu days, %lu hours, %lu minutes", days, hours, remains);
  else if (hours > 0)
    snprintf(buf, BUFSIZE, "%lu hours, %lu minutes", hours, remains);
  else
  {
    snprintf(buf, BUFSIZE, "%lu minutes", remains);
    return buf;
  }

/* does not make sense, does it? ajd
  if (remains > 0)
  {
    snprintf(buf2, BUFSIZE, " and %lu minutes", remains);
    strncat(buf, buf2, BUFSIZE - strlen(buf2));
  }
*/

  return buf;
}

char *
nntripcaster_time (unsigned long int seconds, char *buf)
{
  unsigned long int days, hours, minutes, nseconds, remains;
  char buf2[BUFSIZE];

  if (!buf)
  {
    write_log (LOG_DEFAULT, "ERROR: nntripcaster_time called with NULL argument");
    return NULL;
  }

  buf[0] = '\0';

  days = seconds / 86400;
  remains = seconds % 86400;
  hours = remains / 3600;
  remains = remains % 3600;
  minutes = remains / 60;
  nseconds = remains % 60;
  if (days > 0)
    snprintf(buf, BUFSIZE, "%lu days, %lu hours, %lu minutes", days, hours, minutes);
  else if (hours > 0)
    snprintf(buf, BUFSIZE, "%lu hours, %lu minutes", hours, minutes);
  else if (minutes > 0)
    snprintf(buf, BUFSIZE, "%lu minutes", minutes);
  else
  {
    /* Only seconds */
    snprintf(buf, BUFSIZE, "%lu seconds", nseconds);
    return buf;
  }

  if (nseconds > 0)
  {
    snprintf(buf2, BUFSIZE, " and %lu seconds", nseconds);
    strncat(buf, buf2, BUFSIZE - strlen(buf2));
  }

  return buf;
}

char *
ntripcaster_sprintf (const char *template, const char *arg)
{
  char *ptr;
  size_t sz;

  if (!template)
  {
    if (!arg)
      return nstrdup ("(null)");
    return nstrdup (arg);
  }

  if (!arg)
    return nstrdup ("(null)");

  if (strchr (template, '%') == NULL)
    return nstrdup (template);

  sz = ntripcaster_strlen(arg) + ntripcaster_strlen(template) + 2;
  ptr = (char *)nmalloc(sz);

  snprintf(ptr, sz, template, arg);
  return ptr;
}

size_t
ntripcaster_strlen (const char *string)
{
  if (!string)
  {
    xa_debug (1, "ERROR: ntripcaster_strlen() called with NULL pointer!");
    return 0;
  }
  return strlen (string);
}

int
ntripcaster_strcmp (const char *s1, const char *s2)
{
  if (!s1 || !s2)
  {
    xa_debug (1, "ERROR: ntripcaster_strcmp() called with NULL pointers!");
    return 0;
  }
  return strcmp (s1, s2);
}

int
ntripcaster_strncmp (const char *s1, const char *s2, size_t n)
{
  if (!s1 || !s2)
  {
    xa_debug (1, "ERROR: ntripcaster_strncmp() called with NULL pointers!");
    return 0;
  }
  return strncmp (s1, s2, n);
}

int
ntripcaster_strcasecmp (const char *s1, const char *s2)
{
  if (!s1 || !s2)
  {
    xa_debug (1, "ERROR: ntripcaster_strcasecmp() called with NULL pointers");
    return 0;
  }
#ifdef _WIN32
  return stricmp (s1, s2);
#else
  return strcasecmp (s1, s2);
#endif
}

char*
ntripcaster_strstr (const char* haystack, const char* needle)
{
  if (!needle || !haystack) {
    xa_debug (1, "ERROR: ntripcaster_strstr() called with NULL pointers");
    return NULL;
  }
  return strstr (haystack, needle);
}

const char *
nullcheck_string (const char *string)
{
  if (!string)
    return const_null;
  return string;
}

void
catsnprintf (char *line, size_t sz, const char *fmt, ...)
{
  char buff[BUFSIZE];

  va_list ap;

  va_start (ap, fmt);

  vsnprintf(buff, BUFSIZE, fmt, ap);

  va_end(ap);

  strncat(line, buff, sz);

}

/* Decodes characters like ' ', '<', '>' that were coded in an http url.
 * Because the decoded string is not longer than the coded one, the decoding
 * is performed in-place. ajd */
void decode_url_string(char *string) {
  char hex[3];
  char *src;
  char *dest;
  int i;

  hex[2] = '\0';
  src = string;
  dest = string;

  while (*src != '\0') {

    *dest = *src;

    if (*src == '+')
      *dest = ' ';
    else if (*src == '%') {
      hex[0] = *(++src);
      hex[1] = *(++src);
      i = (int)strtol(hex, NULL, 16);

      if ((i > 31) && (i < 127))
        *dest = http_special_char[i-32].c;
      else {
        *(++dest) = hex[0];
        *(++dest) = hex[1];
      }
    }
    src++;
    dest++;
  }
  *dest = '\0';
}

char *get_closing_parenthesis(char *c) {
  char *t = c;
  int pcount = 0;

  while (*t != '\0') {
    if (*t == ')') {
      pcount--;
      if (pcount == 0) return t;
    } else if (*t == '(')
      pcount++;
    t++;
  }

  return NULL;
}

/* ensures, that s starts with '/'. size must specify the size
 * of the buffer *s points to. If size is < 1, then a pointer to
 * a newly allocated buffer will be returned. ajd */
char *slashalize(char *s, int size) {
  char buf[BUFSIZE];

  if (s[0] == '/') {
    if (size > 0)
      return s;
    else
      return nstrdup(s);
  }

  snprintf(buf, BUFSIZE, "/%s", s);

  if (size > 0) {
    strncpy(s, buf, size);
    return s;
  } else
    return nstrdup(buf);
}

char *create_random_mountpoint(int len) {
  char buf[len+2];
  int i;

  buf[0] = '/';
  for (i=1; i<=len; i++) buf[i] = (char)(65 + (rand()%26));
  buf[len+1] = '\0';

  return nstrdup(buf);
}
