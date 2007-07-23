sockets_pkglibdir=${pkglibdir}/sockets
sockets_pkglib_PROGRAMS = sockets.so
sockets_so_SOURCES = \
	sockets.c \
	kxipaddress.h \
	kxipaddress.c \
	kxsocket.h \
	kxsocket.c \
	sockutils.h \
	sockutils.c \
	kxpoll.h \
	kxpoll.c 
sockets_so_LDFLAGS = -module

EXTRA_DIST=sockets.kx

sockets_pkglib_DATA=sockets.kx

CFLAGS+=-I$(top_srcdir)/src -fPIC

LDFLAGS = -fPIC -shared -Walla
#pkglib_DATA = halloworld.kx
