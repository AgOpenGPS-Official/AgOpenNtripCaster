## Process this with automake to create Makefile.in

AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libauthenticate.a
noinst_HEADERS = basic.h user.h group.h mount.h ldapAuthenticate.h

libauthenticate_a_SOURCES = basic.c user.c group.c mount.c ldapAuthenticate.c

AM_CPPFLAGS = -D_REENTRANT -I$(top_srcdir)/src @WRAPINCLUDES@ 
