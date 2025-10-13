/* tls.c
 * - TLS Functions
 *
 * Copyright (c) 2018
 * German Federal Agency for Cartography and Geodesy (BKG)
 *
 * Developed for Networked Transport of RTCM via Internet Protocol (NTRIP)
 * for streaming GNSS data over the Internet.
 *
 * Designed by Alberding GmbH https://www.alberding.eu/
 *
 * The BKG disclaims any liability nor responsibility to any person or entity
 * with respect to any loss or damage caused, or alleged to be caused,
 * directly or indirectly by the use and application of the NTRIP technology.
 *
 * For latest information and updates, access:
 * https://igs.bkg.bund.de/ntrip/index
 *
 * BKG, Frankfurt, Germany
 * E-mail: euref-ip@bkg.bund.de
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

#ifdef HAVE_TLS
#include "avl.h"
#include "ntripcastertypes.h"
#include "ntripcaster_string.h"
#include "log.h"
#include "sock.h"
#include "tls.h"
#include "utility.h"

#include <openssl/err.h>
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
#include <openssl/x509v3.h>
#endif

int tls_connect(connection_t *con, const char *host)
{
  BIO *sslbio;
  int res;
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
  con->tls_context = SSL_CTX_new(TLS_method());
#else
  con->tls_context = SSL_CTX_new(SSLv23_method());
#endif
  if(!SSL_CTX_load_verify_locations(con->tls_context, 0, NTRIPCASTER_CERTSDIR))
  {
    tls_free(con);
    return -1;
  }
#if defined(X509_V_FLAG_PARTIAL_CHAIN)
  /* Normally you need a full chain for verification for unknown reason.
   * This allows this supply either the final cert or any intermediate
   * to verify (as anybody would expect it to be) */
  X509_STORE_set_flags(SSL_CTX_get_cert_store(con->tls_context),
  X509_V_FLAG_PARTIAL_CHAIN);
#endif
  SSL_CTX_set_options(con->tls_context, SSL_OP_NO_SSLv2);
  SSL_CTX_set_options(con->tls_context, SSL_OP_NO_SSLv3);
  con->tls_socket = SSL_new(con->tls_context);
  SSL_set_verify(con->tls_socket, SSL_VERIFY_PEER, 0);
#if OPENSSL_VERSION_NUMBER >= 0x10002000L
  /* Enable automatic hostname checks */
  {
    X509_VERIFY_PARAM *param = SSL_get0_param(con->tls_socket);

    X509_VERIFY_PARAM_set_hostflags(param,
    X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);

    X509_VERIFY_PARAM_set1_host(param, host, 0);
  }
#endif
  sslbio = BIO_new_socket(con->sock, BIO_NOCLOSE);
  SSL_set_bio(con->tls_socket, sslbio, sslbio);
#if OPENSSL_VERSION_NUMBER >= 0x10000000L
  SSL_set_tlsext_host_name(con->tls_socket, host);
#endif

  res = SSL_connect(con->tls_socket);
  if(res < 0)
  {
    const char *e = ERR_reason_error_string(ERR_peek_last_error());
    xa_debug(1, "ERROR: tls_connect() error %s", e);
  }
  return res;
}

void tls_free(connection_t *con)
{
  if(con->tls_socket)
  {
    SSL_shutdown(con->tls_socket);
    SSL_free(con->tls_socket);
    con->tls_socket = 0;
  }
  if(con->tls_context)
  {
    SSL_CTX_free(con->tls_context);
    con->tls_context = 0;
  }
}

/* now a bunch of functions cloned from sock.c */

int tls_write_string(SSL *tls, const char *buff)
{
  int write_bytes = 0, res = 0, len = ntripcaster_strlen(buff);

  if (!tls) {
    fprintf(stderr,
      "ERROR: tls_write_string() called with invalid socket\n");
    return -1;
  } else if (!buff) {
    fprintf(stderr,
      "ERROR: tls_write_string() called with NULL format\n");
    return -1;
  }

  /*
   * Never use send() to sockets 0 or 1 in Win32. What about 2 (stderr?).
   * Also, if the server is running, and is used as an  admin console, or an
   * admin console with console tail, don't use send(), cause it's not a
   * network socket. Use fprintf()
   */

  while (write_bytes < len) {
    res = SSL_write(tls, &buff[write_bytes], len - write_bytes);
    if (res < 0 && SSL_get_error(tls, res) != SSL_ERROR_WANT_WRITE)
      return 0;
    if (res > 0)
      write_bytes += res;
    else
      my_sleep(30000);
  }

  return (write_bytes == len ? 1 : 0);
}

int tls_write(SSL *tls, const char *fmt, ...)
{
  char buff[BUFSIZE];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buff, BUFSIZE, fmt, ap);
  va_end(ap);

  return tls_write_string(tls, buff);
}

int tls_write_line(SSL *tls, const char *fmt, ...)
{
  char buff[BUFSIZE];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(buff, BUFSIZE, fmt, ap);
  va_end(ap);

  return tls_write(tls, "%s\r\n", buff);
}

int tls_read_line(SSL *tls, char *buff, const int len)
{
  char c = '\r';
  int read_bytes;
  int pos = 0;
  int maxpos = len-1;

  if (!tls) {
    xa_debug(1, "ERROR: tls_read_line() called with invalid socket");
    return -1;
  }

  if (maxpos < 0) return 0;

  read_bytes = tls_recv(tls, &c, 1);

  while (read_bytes > 0) {
    if (c != '\r') {
      if (c == '\n') {
        buff[pos] = '\0';
        return len+1;
      }
      buff[pos] = c;
      if (pos == maxpos) {
        return len;
      }
      pos++;
    }
    read_bytes = tls_recv(tls, &c, 1);
  }

  buff[pos] = '\0';
  if ((pos > 0) || (read_bytes == 0)) return pos;
  return -1;
}

int tls_read_lines_with_timeout(SSL *tls, char *buff, const int len)
{
  char c = '\r';
  char last = '\r';
  int read_bytes;
  int pos = 0;
  int maxpos = len-1;
  long timeout = time(NULL) + SOCK_READ_LINES_TIMEOUT;

  if (!tls) {
    xa_debug(1, "ERROR: tls_read_lines_with_timeout() called with invalid socket");
    return -1;
  } else if (!buff) {
    xa_debug(1, "ERROR: tls_read_lines_with_timeout() called with NULL storage pointer");
    return -1;
  } else if (len <= 0) {
    xa_debug(1, "ERROR: tls_read_lines_with_timeout() called with invalid length");
    return -1;
  }

  if (maxpos < 0) return 0;

  read_bytes = tls_recv(tls, &c, 1);

  while ((read_bytes != 0) && (timeout > 0)) {
    if (c != '\r') {
      if ((c == '\n') && (last == '\n')) {
        buff[pos] = '\0';
        return len+1;
      }
      buff[pos] = c;
      last = c;
      if (pos == maxpos) {
        return len;
      }
      pos++;
    }
    read_bytes = tls_recv(tls, &c, 1);

    while (read_bytes < 0) {
      if (time(NULL) > timeout) {
        timeout = -1;
        break;
      }
      my_sleep(200000);
      read_bytes = tls_recv(tls, &c, 1);
    }
  }

  buff[pos] = '\0';
  if ((pos > 0)|| (read_bytes == 0)) return pos;
  return -1;
}

ssize_t tls_recv(SSL *tls, void *buf, size_t len)
{
  return SSL_read(tls, buf, len);
}

#endif /* HAVE_TLS */
