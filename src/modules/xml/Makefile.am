xml_pkglibdir=${pkglibdir}/xml

xml_pkglib_PROGRAMS = xml.so

xml_so_SOURCES = \
	xml.c \
	kxxmldocument.c \
	kxxmldocument.h \
	kxxmlnode.c \
	kxxmlnode.h

xml_so_LDFLAGS = -module

EXTRA_DIST=xml.kx

xml_pkglib_DATA=xml.kx

CFLAGS+=-I$(top_srcdir)/src -fPIC $(LIBXML_CFLAGS)

LDFLAGS = -fPIC -shared -Walla $(LIBXML_LIBS)
