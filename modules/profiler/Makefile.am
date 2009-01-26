
profiler_pkglibdir=${pkglibdir}/profiler

profiler_pkglib_PROGRAMS = profiler.so
profiler_so_SOURCES = profiler.c ptracker.c ptracker.h
profiler_so_LDFLAGS = -module

CFLAGS+=-I$(top_srcdir)/src -fPIC

EXTRA_DIST=profiler.kx
profiler_pkglib_DATA=profiler.kx

LDFLAGS = -fPIC -shared -Walla
