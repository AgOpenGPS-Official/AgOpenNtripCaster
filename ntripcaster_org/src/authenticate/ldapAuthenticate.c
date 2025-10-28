/*
 *  NtripCaster with LDAP authentication
 *
 */

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <win32config.h>
#else
#include <config.h>
#endif
#endif

#ifdef HAVE_LIBLDAP
#define LDAP_DEPRECATED 1
#include <ldap.h>
#include <stdio.h>
#include <string.h>
#include "ldapAuthenticate.h"

#include <stdlib.h>

#include "avl.h"
#include "threads.h"
#include "ntripcastertypes.h"
#include "ntripcaster.h"
#include "log.h"
#include "memory.h"
#include "ntripcaster_string.h"

extern server_info_t info;

/*
 * Given username and password, authenticate against the LDAP server a simple
 * bind.
 *
 * Return 1 for successful authentication.
 * Return 0 for unsuccessful authentication.
 *
 */
int ldap_authenticate(const char *username, const char *password)
{
    char loginDN[255];
    LDAP *ld;
    int result;
    int ldapPort = 389;
    int auth_method = LDAP_AUTH_SIMPLE;
    int desired_version = LDAP_VERSION3;

    xa_debug(1, "LDAP session started (%s %d).", info.ldap_server,
    ldapPort);
    /* initialize LDAP session */
    if(!(ld = ldap_init(info.ldap_server, ldapPort)))
    {
        xa_debug(1, "LDAP session initialization failed");
        return 0;
    }
    xa_debug(1, "New LDAP session initialized");

    /* set the LDAP version to be 3 */
    if((result = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION,
    &desired_version)) != LDAP_OPT_SUCCESS)
    {
       ldap_unbind_s(ld);
       xa_debug(1, "LDAP set option error: %s", ldap_err2string(result));
       return 0;
    }

    /* try to authenticate */
    snprintf(loginDN, sizeof(loginDN),"%s=%s,%s", info.ldap_uid_prefix,
    username, info.ldap_people_context);
    loginDN[sizeof(loginDN)-1] = 0; // ensure zero termination
    xa_debug(1, "LDAP login started (%s).", loginDN);
    if((result = ldap_bind_s(ld, loginDN, password, auth_method))
    != LDAP_SUCCESS)
    {
       ldap_unbind_s(ld);
       xa_debug(1, "LDAP bin authentication unsuccessful: %s",
       ldap_err2string(result));
       return 0;
    }
    xa_debug(1, "Authentication successful!");

    /* clean up */
    if((result = ldap_unbind_s(ld)) != 0)
    {
      xa_debug(1, "LDAP unbind error: %s", ldap_err2string(result));
    }
    xa_debug(1, "LDAP session ended.");

    return 1;
}
#endif
