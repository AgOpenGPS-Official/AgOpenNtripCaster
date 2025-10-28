## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS = authenticate

sbin_PROGRAMS = ntripdaemon

noinst_HEADERS = admin.h alias.h avl.h avl_functions.h client.h		\
			definitions.h commandline.h commands.h connection.h	\
			http.h		\
			ntripcaster_string.h ntripcaster.h ntripcastertypes.h log.h		\
			logtime.h main.h match.h memory.h relay.h	\
			restrict.h sock.h source.h sourcetable.h threads.h	\
			timer.h utility.h vars.h ntripcaster_resolv.h item.h    \
			pool.h interpreter.h vsnprintf.h rtsp.h ntrip.h rtp.h parser.h tls.h

ntripdaemon_SOURCES = main.c client.c admin.c source.c sourcetable.c connection.c log.c	\
			commands.c sock.c threads.c		\
			logtime.c commandline.c utility.c avl.c		\
			avl_functions.c match.c relay.c timer.c		\
			alias.c restrict.c http.c		\
			ntripcaster_string.c vars.c memory.c ntripcaster_resolv.c \
			item.c pool.c interpreter.c vsnprintf.c rtsp.c ntrip.c rtp.c parser.c tls.c

ntripdaemon_LDADD = authenticate/libauthenticate.a @WRAPLIBS@ @CRYPTLIB@

AM_CPPFLAGS = -D_REENTRANT @WRAPINCLUDES@ 

#if FSSTD
#bindir=$(NTRIPCASTER_SBINDIR)
#else
#bindir=$(NTRIPCASTER_BINDIR)
#endif
