
system_pkglibdir=${pkglibdir}/system

system_pkglib_PROGRAMS = system.so
system_so_SOURCES = system.c pid.c pid.h
system_so_LDFLAGS = -module

CFLAGS+=-I$(top_srcdir)/src -fPIC

EXTRA_DIST=system.kx
system_pkglib_DATA=system.kx

LDFLAGS = -fPIC -shared -Walla
