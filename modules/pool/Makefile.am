
pool_pkglibdir=${pkglibdir}/pool

pool_pkglib_PROGRAMS = pool.so
pool_so_SOURCES = pool.c kxpool.c kxpool.h
pool_so_LDFLAGS = -module

EXTRA_DIST=pool.kx
pool_pkglib_DATA=pool.kx

CFLAGS+=-I$(top_srcdir)/src -fPIC

LDFLAGS = -fPIC -shared -Walla
