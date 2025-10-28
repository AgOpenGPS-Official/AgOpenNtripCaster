AC_INIT([ntripcaster],[2.0.46])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AC_USE_SYSTEM_EXTENSIONS

echo "Building Ntripcaster-$VERSION..."

AM_MAINTAINER_MODE
AM_CONDITIONAL(FSSTD, test x$enable_fsstd = xyes)
AC_SUBST(USE_FSSTD)

dnl reasonable guesses for where stuff is installed
if test "x$prefix" = "xNONE"; then
	prefix="/usr/local/ntripcaster"
fi

dnl reasonable guesses for where stuff is installed
if test "x$exec_prefix" = "xNONE"; then
	exec_prefix=$prefix
fi

AC_ARG_ENABLE(fsstd, [AS_HELP_STRING([--enable-fsstd], [install files following FSSTD [default=no]])], , enable_fsstd=no)

if test "x$enable_fsstd" = "xyes"; then
	NTRIPCASTER_BINDIR=${bindir}
	NTRIPCASTER_SBINDIR=${sbindir}
	NTRIPCASTER_ETCDIR=/etc/ntripcaster
	NTRIPCASTER_ETCDIR_INST=/etc/ntripcaster
	NTRIPCASTER_SHAREDIR=/usr/share/ntripcaster
	NTRIPCASTER_LOGDIR=/var/log/ntripcaster
	NTRIPCASTER_VARDIR=/var/run/ntripcaster
	NTRIPCASTER_LOGDIR_INST=/var/log/ntripcaster
	NTRIPCASTER_VARDIR_INST=/var/run/ntripcaster
	NTRIPCASTER_TEMPLATEDIR=/usr/share/ntripcaster/templates
	NTRIPCASTER_TEMPLATEDIR_INST=/usr/share/ntripcaster/templates
	NTRIPCASTER_CERTSDIR=/etc/ssl/certs/
	USE_FSSTD=yes
else
	NTRIPCASTER_ETCDIR=conf
	NTRIPCASTER_ETCDIR_INST=${prefix}/conf
	NTRIPCASTER_SHAREDIR=${prefix}
	NTRIPCASTER_LOGDIR=logs
	NTRIPCASTER_VARDIR=var
	NTRIPCASTER_LOGDIR_INST=${prefix}/logs
	NTRIPCASTER_VARDIR_INST=${prefix}/var
	NTRIPCASTER_TEMPLATEDIR=templates
	NTRIPCASTER_TEMPLATEDIR_INST=${prefix}/templates
	NTRIPCASTER_CERTSDIR=/etc/ssl/certs/
	USE_FSSTD=no
fi
# does not work with spaces in path, but escaping also does not work
TRANSCMD="sed -e s,.{prefix},${prefix},g -e s,.{exec_prefix},${exec_prefix},g -e s,.{bindir},${bindir},g -e s,^NONE,,g"
NTRIPCASTER_BINDIR=$(echo $bindir | ${TRANSCMD} | ${TRANSCMD})
NTRIPCASTER_SBINDIR=$(echo $sbindir | ${TRANSCMD} | ${TRANSCMD})

dnl TLS?
AC_ARG_WITH(tls,
  AS_HELP_STRING([--without-tls],[Disable TLS support (OpenTLS)]),
  [ with_tls=$withval ])

if test "x$with_tls" = "xno" ; then
  NTRIPCASTER_TLSTYPE="none"
fi

if test "x$NTRIPCASTER_TLSTYPE" != "xnone" ; then
  AC_ARG_WITH(tls-dir,
    AS_HELP_STRING([--with-tls-dir=DIR],[Root of TLS lib/include path]),
    [
      if test "x$withval" != "xno" ; then
        NTRIPCASTER_TLSDIR=$withval
        NTRIPCASTER_TLSINCLUDE=$NTRIPCASTER_TLSDIR/include
        NTRIPCASTER_TLSLIB=$NTRIPCASTER_TLSDIR/lib
      fi
    ])

  AC_ARG_WITH(tls-certs-dir,
    AS_HELP_STRING([--with-tls-certs-dir=DIR],[Path to TLS certificate directory]),
    [
      if test "x$withval" != "xno" ; then
        NTRIPCASTER_CERTSDIR=$withval
      fi
    ])

  AC_ARG_WITH(tls-include-dir,
    AS_HELP_STRING([--with-tls-include-dir=DIR],[TLS include file path]),
    [
      if test "x$withval" != "xno" ; then
        NTRIPCASTER_TLSINCLUDE=$withval
      fi
    ])

  AC_ARG_WITH(tls-lib-dir,
    AS_HELP_STRING([--with-tls-lib-dir=DIR],[TLS library path]),
    [
      if test "x$withval" != "xno" ; then
        NTRIPCASTER_TLSLIB=$withval
      fi
    ])

  dnl setup globals so tests below work
  if test -n "$NTRIPCASTER_TLSINCLUDE" ; then
    CPPFLAGS="-I$NTRIPCASTER_TLSINCLUDE $CPPFLAGS"
  elif test -n "$NTRIPCASTER_TLSDIR" ; then
    CPPFLAGS="-I${NTRIPCASTER_TLSDIR}/include $CPPFLAGS"
  fi
  if test -n "$NTRIPCASTER_TLSLIB" ; then
    LDFLAGS="-L$NTRIPCASTER_TLSLIB $LDFLAGS"
  elif test -n "$NTRIPCASTER_TLSDIR" ; then
    LDFLAGS="-L${NTRIPCASTER_TLSDIR}/lib $LDFLAGS"
  fi

  dnl Now check if we have working version of openssl
  AC_MSG_CHECKING([Openssl library version >= 1.0.0])
  AC_RUN_IFELSE( [AC_LANG_SOURCE([[
  #include <stdio.h>
  #include <stdlib.h>
  #if HAVE_STDINT_H
  #include <stdint.h>
  #endif /* HAVE_STDINT_H */
  #include <openssl/ssl.h>
  int main(void) {
          if (OPENSSL_VERSION_NUMBER >= 0x01000000L)
                  exit(0);

          exit(2);
  }
          ]])],
          [ AC_MSG_RESULT(yes)
            NTRIPCASTER_TLSTYPE="ok"
            LIBS="$LIBS -lssl -lcrypto"
          ],
          [ AC_MSG_RESULT(no)
            NTRIPCASTER_TLSTYPE="none"
          ],
          [ AC_MSG_WARN([cross compiling: not checking])])
fi


AC_CONFIG_HEADERS([config.h])

AC_DEFINE_UNQUOTED([VERSION], ["$VERSION"],  [The complete version of ntripcaster])
AC_SUBST(VERSION)

AC_SUBST(NTRIPCASTER_BINDIR)
AC_SUBST(NTRIPCASTER_SBINDIR)
AC_SUBST(NTRIPCASTER_CERTSDIR)
AC_SUBST(NTRIPCASTER_ETCDIR_INST)
AC_SUBST(NTRIPCASTER_LOGDIR_INST)
AC_SUBST(NTRIPCASTER_VARDIR_INST)
AC_SUBST(NTRIPCASTER_TEMPLATEDIR_INST)

AC_DEFINE_UNQUOTED(NTRIPCASTER_ETCDIR, "$NTRIPCASTER_ETCDIR", [Ntrip caster configuration directory])
AC_SUBST(NTRIPCASTER_ETCDIR)
AC_DEFINE_UNQUOTED(NTRIPCASTER_LOGDIR, "$NTRIPCASTER_LOGDIR", [Ntrip caster logging directory])
AC_SUBST(NTRIPCASTER_LOGDIR)
AC_DEFINE_UNQUOTED(NTRIPCASTER_VARDIR, "$NTRIPCASTER_VARDIR", [Ntrip caster variable directory])
AC_SUBST(NTRIPCASTER_VARDIR)
AC_DEFINE_UNQUOTED(NTRIPCASTER_TEMPLATEDIR, "$NTRIPCASTER_TEMPLATEDIR", [Ntrip caster templates directory])
AC_SUBST(NTRIPCASTER_TEMPLATEDIR)
AC_DEFINE_UNQUOTED(NTRIPCASTER_CERTSDIR, "$NTRIPCASTER_CERTSDIR", [TLS CA certificate directory])
AC_SUBST(NTRIPCASTER_CERTSDIR)

if test "x$NTRIPCASTER_TLSTYPE" != "xnone" ; then
        AC_DEFINE([HAVE_TLS], [1], [TLS library])
fi

AC_ARG_PROGRAM
AC_PROG_RANLIB

dnl Checks for programs.
AC_PROG_CC

if test "$CC" = "gcc"; then
	CFLAGS="$CFLAGS -Wall"
fi

AC_CHECK_PROG(UNAME,uname,uname)
AC_PROG_INSTALL

dnl weird ass systems
AC_SEARCH_LIBS([strerror],[cposix])

AC_MSG_CHECKING(your OS)
system=`$UNAME`
IRIX=no
case $system in
	Linux)
		LINUX=1
		AC_DEFINE([LINUX], [], [Some kind of Linux])
		AC_MSG_RESULT(Linux)
		;;
	SunOS)
		if test x`uname -r | cut -d . -f 1` = "x5"; then
			SYSV=1
			SVR4=1
			SOLARIS=1
			AC_MSG_RESULT(Solaris)
			AC_DEFINE([SYSV], [], [Are we sysv?])
			AC_DEFINE([SVR4], [], [Or svr4 perhaps?])
			AC_DEFINE([SOLARIS], [], [Definately Solaris])
		else
			AC_MSG_RESULT(SunOS)
		fi
		if test "$CC" = cc; then
			CFLAGS="$CFLAGS -mt"
		fi
		;;
	HP-UX)
		hpux=1
		AC_DEFINE([hpux], [], [We might be the silly hpux])
		AC_MSG_RESULT(HP-UX)
		CFLAGS="$CFLAGS -D_HPUX_SOURCE -DHPUX"
		if test "$CC" = cc; then
			CFLAGS="$CFLAGS -Aa"
		fi
		;;
	IRIX)
		IRIX=yes
		AC_DEFINE([IRIX], [], [Fucked up IRIX])
		AC_MSG_RESULT(IRIX)
		;;
	BSD/OS)
		__SOMEBSD__=yes
		AC_DEFINE([__SOMEBSD__], [], [Or perhaps some bsd variant?])
		AC_MSG_RESULT(BSD/OS)
		;;
	*BSD*)
		__SOMEBSD__=yes
		AC_DEFINE(__SOMEBSD__)
		AC_MSG_RESULT(Some BSD variant)
		OLDCPPFLAGS=$CPPFLAGS
		THREADCPPFLAGS="-pthread"
		CPPFLAGS="$OLDCPPFLAGS $THREADCPPFLAGS"
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <pthread.h>]], [[pthread_t thread; ]])],[],[CPPFLAGS="$OLDCPPFLAGS"])
		CPPFLAGS=$OLDCPPFLAGS
		if test "$CC" = gcc; then
			CFLAGS="-pthreads $CFLAGS"
		fi
		;;
	*)
		AC_MSG_RESULT($system)
		;;
esac

if test ${IRIX} = yes; then
	echo Skipping library tests because they confuse Irix.
else



  AC_CHECK_FUNCS(socket)

  if test "$ac_cv_func_socket" = no; then
    for lib in socket inet; do
	if test "$HAVE_SOCKET" != 1; then
		AC_CHECK_LIB($lib, socket,LIBS="$LIBS -l$lib";HAVE_SOCKET=1;AC_DEFINE(HAVE_SOCKET))
	fi
    done
  fi

  AC_CHECK_FUNCS(connect)

  if test "$ac_cv_func_connect" = no; then
    for lib in nsl; do
	if test "$HAVE_CONNECT" != 1; then
		AC_CHECK_LIB($lib, connect,LIBS="$LIBS -l$lib";HAVE_CONNECT=1;AC_DEFINE(HAVE_CONNECT))
	fi
    done
  fi

  AC_CHECK_FUNCS(log)

  if test "$ac_cv_func_log" = no; then
    for lib in m; do
	AC_CHECK_LIB($lib, log)
    done
  fi

  AC_CHECK_FUNCS(gethostbyname)
  if test "$ac_cv_func_gethostbyname" = no; then
	for lib in dns nsl resolv; do
	  if test "$HAVE_GETHOSTBYNAME" != 1; then
	    AC_CHECK_LIB($lib, gethostbyname, LIBS="$LIBS -l$lib";HAVE_GETHOSTBYNAME=1;AC_DEFINE(HAVE_GETHOSTBYNAME))
	  fi
	done
  fi

  AC_CHECK_FUNCS(nanosleep)

  if test "$ac_cv_func_nanosleep" = no; then
	for lib in rt; do
		AC_CHECK_LIB($lib, nanosleep,LIBS="$LIBS -l$lib";HAVE_NANOSLEEP=1;AC_DEFINE(HAVE_NANOSLEEP))
	done
  fi
fi

dnl Allow me to turn on/off poll
AC_ARG_ENABLE(poll, [AS_HELP_STRING([--disable-poll], [disable use of the poll() function [default=no]])],
if eval "test x$enable_poll = xyes"; then
	AC_CHECK_FUNCS(poll)
	AC_CHECK_HEADERS(poll.h sys/poll.h)
fi,
AC_CHECK_FUNCS(poll)
AC_CHECK_HEADERS(poll.h sys/poll.h)
)

opt_readline="no"

dnl Do we want libreadline ?
if test "$opt_readline" = "unset"; then
	if test -f "$withval/include/readline/readline.h"; then
    		READLINE_INCLUDE="-I$withval/include"
		LDFLAGS="$LDFLAGS -L$withval/lib"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
    	elif test -f "$withval/include/readline.h"; then
		READLINE_INCLUDE="-I$withval/include"
		LDFLAGS="$LDFLAGS -L$withval/lib"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
	elif test -f "$withval/readline.h"; then
		READLINE_INCLUDE="-I$withval"
		LDFLAGS="$LDFLAGS -L$withval"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
    	elif test -f "/usr/include/readline/readline.h"; then
		READLINE_INCLUDE="-I/usr/include/readline"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
	elif test -f "/usr/include/readline.h"; then
		AC_MSG_RESULT(yes)
		opt_readline="yes"
	elif test -f "/usr/local/include/readline.h"; then
		READLINE_INCLUDE="-I/usr/local/include"
		LDFLAGS="$LDFLAGS -L/usr/local/lib"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
    	elif test -f "/usr/local/include/readline/readline.h"; then
		READLINE_INCLUDE="-I/usr/local/include/readline"
		LDFLAGS="$LDFLAGS -L/usr/local/lib"
		AC_MSG_RESULT(yes)
		opt_readline="yes"
    	else
		AC_MSG_RESULT(no)
		opt_readline="no"
	fi
fi

if test "$opt_readline" = yes; then
  AC_CHECK_LIB(readline,
    readline,,
    [ AC_MSG_WARN(Testing with -ltermcap)
    unset ac_cv_lib_readline_readline
    AC_CHECK_LIB(readline,
      readline,
      LIBS="$LIBS -lreadline -ltermcap"
      AC_DEFINE(HAVE_LIBREADLINE),
      AC_MSG_WARN(Compiling without readline support),
      -ltermcap)]
  )
fi

CPPFLAGS="$CPPFLAGS $READLINE_INCLUDE"

dnl Do we want libwrap?
AC_MSG_CHECKING(whether to use libwrap)
AC_ARG_WITH(libwrap,
[AS_HELP_STRING([--with-libwrap@<:@=PATH@:>@], [compile in libwrap (tcp_wrappers) support.])],
[ case "$withval" in
  no)
	AC_MSG_RESULT(no)
	;;
  yes)
	AC_MSG_RESULT(yes)
	AC_CHECK_LIB(wrap, request_init,WRAPLIBS="-lwrap";opt_libwrap="yes";)
	;;
  *)
	AC_MSG_RESULT(yes)
	opt_libwrap="yes"
	if test -d "$withval"; then
		WRAPLIBS="-L$withval -lwrap"
		WRAPINCLUDES="-I$withval/include"
	else
		WRAPLIBS="$withval"
	fi
	OLDLIBS="$LIBS"
	LIBS="$WRAPLIBS $LIBS"
	AC_LINK_IFELSE([AC_LANG_PROGRAM([[int allow_severity; int deny_severity; ]], [[hosts_access(); ]])],[],[AC_MSG_ERROR(Could not find the $withval library. You must first install tcp_wrappers.)])
	    LIBS="$OLDLIBS"
	;;
  esac ],
  AC_MSG_RESULT(no)
)

if test "$opt_libwrap" = yes; then
AC_DEFINE([HAVE_LIBWRAP], [], [Whether to use tcp_wrappers])

AC_CHECK_FUNCS(yp_get_default_domain)

	if test "$ac_cv_func_yp_get_default_domain" = no; then
		for lib in wrap nsl nss_nis; do
			if test "$HAVE_YP_GET_DEFAULT_DOMAIN" != 1; then
				AC_CHECK_LIB($lib, yp_get_default_domain,LIBS="$LIBS -l$lib";HAVE_YP_GET_DEFAULT_DOMAIN=1;AC_DEFINE(HAVE_YP_GET_DEFAULT_DOMAIN))
			fi
		done
	fi

fi

AC_SUBST(WRAPLIBS)
AC_SUBST(WRAPINCLUDES)

AC_MSG_CHECKING(whether to use crypted passwords)
CRYPTLIB=
AC_ARG_ENABLE([crypt],
  AS_HELP_STRING([--disable-crypt], [do not use crypt() to encrypt server passwords.]))

AS_IF([test "x$enable_crypt" != "xno"], [
  AC_MSG_RESULT(yes)
  AC_CHECK_LIB(crypt, crypt, CRYPTLIB="-lcrypt",)
  AC_DEFINE([USE_CRYPT], [], [Whether to use crypted passwords])
], [
  AC_MSG_RESULT(no)
])

AC_SUBST(CRYPTLIB)

THREADLIBS="no"

AC_MSG_CHECKING(for pthread functions in standard libraries)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[pthread_create();]])],[AC_MSG_RESULT(yes);THREADLIBS=""],[AC_MSG_RESULT(no)])

if test "$THREADLIBS" = ""; then
	AC_MSG_CHECKING(if that definition really works)
	AC_RUN_IFELSE([AC_LANG_SOURCE([[int main() { return !pthread_equal(pthread_self(), pthread_self());}]])],[AC_MSG_RESULT(yes);THREADLIBS=""],[AC_MSG_RESULT(no);THREADLIBS="no"],[AC_MSG_RESULT(crosscompiling so no);THREADLIBS="no"])
fi

if test "$THREADLIBS" = "no"; then
    for lib in pthread c_r dce pthreads; do
      AC_CHECK_LIB($lib,pthread_create,THREADLIBS="-l$lib")
      if test $THREADLIBS != "no"; then
        break
      fi
    done
fi

if test "$THREADLIBS" = "no"; then
	  AC_CHECK_FUNCS(pthread_create,THREADLIBS="")
fi

if test "$THREADLIBS" = "no"; then
        AC_MSG_ERROR(Your system doesn't seem to support posix threads. Ntripcaster 0.2.0 and above won't work without threads support)
        exit
fi

AC_SUBST(THREADLIBS)
LIBS="$LIBS $THREADLIBS"
CFLAGS="$THREADCPPFLAGS $CPPFLAGS $CFLAGS"

AC_ARG_WITH(python,
  [AS_HELP_STRING([--with-python@<:@=ARG@:>@], [enable usage of the python interpreter [ARG=yes]])])

AC_ARG_WITH(python-includes,
[  --with-python-includes=DIR  Python include files are in DIR])
AC_ARG_WITH(python-libraries,
[  --with-python-libraries=DIR Python library file are in DIR])

if test "$with_python_includes"; then
        CPPFLAGS="$CPPFLAGS -I$with_python_includes"
        if test "$with_python" = ""; then
                with_python=yes
        fi
fi

if test "$with_python_libraries"; then
        LIBS="$LIBS -L$with_python_libraries"
        if test "$with_python" = ""; then
                with_python=yes
        fi
fi

if test "$with_python"; then
dnl I can't say for sure what libs we need for python, but this is what my system needs
AC_CHECK_LIB(dl, dlopen)
	for lib in python python2.0 python1.6 python1.5; do
		AC_CHECK_LIB($lib, Py_Initialize)
	done
fi

AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <syslog.h>]], [[int foo = LOG_DAEMON; ]])],[],[AC_DEFINE(NEED_SYS_SYSLOG_H, , Some systems have sys/syslog.h)])

dnl Checks for header files.
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h pthread.h assert.h sys/resource.h math.h signal.h sys/signal.h mcheck.h malloc.h history.h Python.h systemd/sd-daemon.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

AC_CHECK_HEADERS([sys/time.h])
AC_STRUCT_TM

AC_MSG_CHECKING([for unix98 socklen_t])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
                #include <sys/socket.h>]], [[socklen_t len;
                len = 1;]])],[AC_MSG_RESULT(yes)
        HAVE_SOCKLEN_T=yes],[AC_MSG_RESULT(no)
        HAVE_SOCKLEN_T=no])

if test ${HAVE_SOCKLEN_T} = yes ;then
	AC_DEFINE([HAVE_SOCKLEN_T], [], [UNIX98 and others want socklen_t])
fi

AC_MSG_CHECKING([for systemd support])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <systemd/sd-daemon.h>]],
        [[sd_notify(0,"");]])], [AC_MSG_RESULT(yes)
        HAVE_SDNOTIFY=yes], [AC_MSG_RESULT(no)
        HAVE_SDNOTIFY=no])

AC_MSG_CHECKING(whether to use systemd)
AC_ARG_ENABLE([systemd],
  AS_HELP_STRING([--disable-systemd], [do not use systemd sd_notify().]))

AS_IF([test ${HAVE_SDNOTIFY} = yes && test "x$enable_systemd" != "xno"], [
  AC_MSG_RESULT(yes)
  LIBS="$LIBS -lsystemd"
  AC_DEFINE([USE_SYSTEMD], [], [Whether to use systemd])
], [
  AC_MSG_RESULT(no)
])

dnl Checks for library functions.
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF

AC_CHECK_FUNCS(gettimeofday strstr snprintf vsnprintf rename setpgid basename setsockopt gethostbyname_r gethostbyaddr_r getrlimit setrlimit umask inet_addr inet_aton localtime_r select pthread_attr_setstacksize inet_ntoa mcheck mallinfo mallinfo2 mtrace sigaction pthread_sigmask lseek)

AC_MSG_CHECKING(if libm is bundled with some lib we're already linking)
AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[sin(1);]])],[AC_MSG_RESULT(yes);LDLAGS=""],[AC_MSG_RESULT(no);LDFLAGS="-lm"])

AC_CHECK_LIB(ldap,ldap_init)
AC_DEFINE([NC_LDAP_HOST], [""], [LDAP Host])
AC_DEFINE([NC_LDAP_UID_PREFIX], ["uid"], [LDAP UID Prefix])
AC_DEFINE([NC_LDAP_PEOPLE_CONTEXT], ["ou=people"], [LDAP People Container])

AC_CONFIG_FILES([Makefile src/Makefile src/authenticate/Makefile conf/Makefile templates/Makefile conf/ntripcaster.conf.dist scripts/Makefile scripts/ntripcaster scripts/casterwatch scripts/ntripcaster.service])
AC_OUTPUT

echo "Ok, everything seems ok. Now do 'make'."
echo "You can then run make install to install as you specified"
