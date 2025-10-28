/*
 *  NtripCaster with LDAP authentication
 *
 */

#ifdef HAVE_LIBLDAP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Given username and password, authenticate agains the LDAP server a simple bind.
 *
 * Return 1 for successful authentication.
 * Return 0 for unsuccessful authentication.
 *
 */
int ldap_authenticate(const char *username, const char *password);
#endif /* HAVE_LIBLDAP */
