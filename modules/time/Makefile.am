time_pkglibdir=${pkglibdir}/time
time_pkglib_PROGRAMS = time.so
time_so_SOURCES = \
	time.c

time_so_LDFLAGS = -module

EXTRA_DIST=time.kx

time_pkglib_DATA=time.kx

CFLAGS+=-I$(top_srcdir)/src -fPIC

LDFLAGS= -fPIC -shared -Walla

