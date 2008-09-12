
gtk_pkglibdir=${libdir}/kreatrix/gtk

LDFLAGS+= -fPIC -shared -Wall $(gtk_LIBS)
CFLAGS+= -fPIC $(gtk_CFLAGS) -I$(top_srcdir)/src

gtk_pkglib_PROGRAMS = gtk.so

gtk_so_SOURCES= \
   gtk.c \
   gtk_utils.c \
   gtk_utils.h \
   kxclosure.c \
   kxclosure.h \
   gdkevent.c \
   gdkevent.h \
   gtktextbuffer_extra.c \
   kxtree.c \
   kxtree.h \
   kxgtk_wrapper.c \
   kxgtk_wrapper.h

gtk_so_LDFLAGS= -module

gtk.c kxgtk_wrapper.c kxgtk_wrapper.h: generator/class.xml generator/structs.txt generator/enums.txt
	sh $(srcdir)/generate.sh $(top_builddir) $(top_srcdir) $(srcdir)
	
MAINTAINERCLEANFILES=\
	kxgtk_wrapper.c \
	kxgtk_wrapper.h \
	gtk.c

EXTRA_DIST = \
kxgtk_wrapper.c \
gtk.c gtk.kx generate.sh kxgenerated.tmp \
generator/structs.txt \
generator/wrapper.kx \
generator/functions.txt \
generator/struct.kx \
generator/function.kx \
generator/enum.kx \
generator/module.c \
generator/structs.txt \
generator/generator.kx \
generator/type.kx \
generator/wrapper.c \
generator/wrapper.h \
generator/class.xml \
generator/enums.txt \
generator/class.kx \
generator/utils.kx \
generator/constant.kx \
generator/constants.xml \
generator/wrapper_end.h \
generator/wrapper_head.h \
generator/wrapper_head.c


gtk_pkglib_DATA=gtk.kx


