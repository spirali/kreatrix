
bytecode_pkglibdir=${pkglibdir}/bytecode

bytecode_pkglib_PROGRAMS = bytecode.so
bytecode_so_SOURCES = bytecode.c kxinstructionobject.c kxinstructionobject.h
bytecode_so_LDFLAGS = -module

CFLAGS+=-I$(top_srcdir)/src -fPIC

EXTRA_DIST=bytecode.kx
bytecode_pkglib_DATA=bytecode.kx

LDFLAGS = -fPIC -shared -Walla
