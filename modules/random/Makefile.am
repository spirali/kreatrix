
random_pkglibdir=${pkglibdir}/random

random_pkglib_PROGRAMS = random.so
random_so_SOURCES = module.c kxrandom.c kxrandom.h
random_so_LDFLAGS = -module

CFLAGS+=-I$(top_srcdir)/src -fPIC

EXTRA_DIST=random.kx
random_pkglib_DATA=random.kx

LDFLAGS = -fPIC -shared -Walla
