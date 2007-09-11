
threads_pkglibdir=${pkglibdir}/threads

threads_pkglib_PROGRAMS = threads.so
threads_so_SOURCES = threads.c kxthread.c kxthread.h kxmutex.h kxmutex.c
threads_so_LDFLAGS = -module -D__USE_REENTRANT -lpthread

CFLAGS+=-I$(top_srcdir)/src -fPIC

EXTRA_DIST=threads.kx
threads_pkglib_DATA=threads.kx

LDFLAGS = -fPIC -shared -Walla
