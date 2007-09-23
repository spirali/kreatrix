
gtk_pkglibdir=${libdir}/kreatrix/gtk

LDFLAGS+= -fPIC -shared -Wall $(gtk_LIBS)
CFLAGS+= -fPIC $(gtk_CFLAGS) -I$(top_srcdir)/src

gtk_pkglib_PROGRAMS = gtk.so

gtk_so_SOURCES= \
   gobject.h \
gobject.c \
gtkobject.h \
gtkobject.c \
gtkwidget.h \
gtkwidget.c \
gtkcontainer.h \
gtkcontainer.c \
gtkbin.h \
gtkbin.c \
gtkwindow.h \
gtkwindow.c \
gtkbutton.h \
gtkbutton.c \
gtkcheckbutton.h \
gtkcheckbutton.c \
gtkbox.h \
gtkbox.c \
gtkvbox.h \
gtkvbox.c \
gtkhbox.h \
gtkhbox.c \
gtkmisc.h \
gtkmisc.c \
gtklabel.h \
gtklabel.c \
gtkaccelgroup.h \
gtkaccelgroup.c \
gtkrequisition.h \
gtkrequisition.c \
gdkgeometry.h \
gdkgeometry.c \
gtk.c \
   gtk_utils.c \
   gtk_utils.h \
   kxclosure.c \
   kxclosure.h \
   gdkevent.c \
   gdkevent.h

gtk_so_LDFLAGS= -module

EXTRA_DIST = gtk.xml gtk.kx

gtk_pkglib_DATA=gtk.kx


