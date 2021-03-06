AC_INIT([Kreatrix],[0.12.0],[spirali@kreatrix.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
#AC_PROG_LEX
#AC_PROG_YACC

AC_LIBTOOL_DLOPEN
#AC_PROG_LIBTOOL
AC_PROG_RANLIB

AC_EXEEXT

AC_CHECK_LIB(dl, dlopen)

AC_CONFIG_HEADERS([kxconfig.h])
AC_CONFIG_FILES([
        Makefile
        src/Makefile
		src/utils/Makefile
		src/compiler/Makefile
		src/tools/Makefile
		src/tools/kx2carray/Makefile
		src/tools/docgen/Makefile
		src/tools/objgen/Makefile
		src/tools/wrapgen/Makefile
		src/vminit/Makefile
		modules/Makefile
		modules/test/Makefile
		modules/system/Makefile
		modules/sockets/Makefile
		modules/time/Makefile
		modules/pool/Makefile
		modules/cgi/Makefile
		modules/random/Makefile
		modules/threads/Makefile
		modules/xml/Makefile
		modules/gtk/Makefile
		modules/bytecode/Makefile
		modules/profiler/Makefile
		tests/Makefile
		docs/Makefile
		docs/examples/Makefile
])

#AC_DEFINE_UNQUOTED([SHARE_EXT],["$shrext_cmds"],[Extension of modules])

# TODO: Autodetect extension
AC_DEFINE_UNQUOTED([SHARE_EXT],[".so"],[Extension of modules])



AC_ARG_ENABLE(debug-gc,
[  --enable-debug-gc   If this option is enabled, GC runs before every activation of CodeBlock],
[
AC_DEFINE_UNQUOTED([KX_DEBUG_GC],[1],[DEBUG! Run GC after every instruction])
AC_MSG_NOTICE([DEBUG OPTIONS: gc-debug enabled])
],[])

AC_ARG_ENABLE(vm-log,
[  --enable-vm-log   Debug option. Activities of VM is logged ],
[
AC_DEFINE_UNQUOTED([KX_LOG],[1],[DEBUG! Activities of VM is logged])
AC_MSG_NOTICE([DEBUG OPTIONS: vm-log enabled])
],[])

# MultiCore VM
AC_ARG_ENABLE(multistate,
[  --enable-multistate   Enable support for more VMs in one application ],
[ 
kx_multistate="${enableval}"
],[kx_multistate=no])

if test "$kx_multistate" = yes; then
AC_MSG_NOTICE([Multi state ENABLED])
AC_DEFINE_UNQUOTED([KX_MULTI_STATE],[1],[More KxCore's in one application])
fi



# Threads support
AC_ARG_ENABLE(threads,
[  --disable-threads   Disable threads support ],
[ 
kx_threads="${enableval}"
],[kx_threads=yes])

if test "$kx_threads" = no; then
AM_CONDITIONAL([KX_THREADS_SUPPORT], false)
AC_MSG_NOTICE([Threads support disabled])
else
AC_DEFINE_UNQUOTED([KX_THREADS_SUPPORT],[1],[Support threads in VM])
AM_CONDITIONAL([KX_THREADS_SUPPORT], true)
fi

# Inline cache support
AC_ARG_ENABLE(icache,
[  --disable-icache   Disable inline cache support ],
[ 
kx_icache="${enableval}"
],[kx_icache=yes])

if test "$kx_icache" = no; then
AM_CONDITIONAL([KX_INLINE_CACHE], false)
AC_MSG_NOTICE([Inline cache support disabled])
else
AC_DEFINE_UNQUOTED([KX_INLINE_CACHE],[1],[Support for inline cache])
AM_CONDITIONAL([KX_INLINE_CACHE], true)
fi


# Hotspot support
AC_ARG_ENABLE(hotspot,
[  --disable-hotspot   Disable hotspot support ],
[ 
kx_hotspot="${enableval}"
],[kx_hotspot=yes])

if test "$kx_hotspot" = no; then
AM_CONDITIONAL([KX_HOTSPOT], false)
AC_MSG_NOTICE([Hotspot support disabled])
else
AC_DEFINE_UNQUOTED([KX_HOTSPOT],[1],[Support for hotspot])
AM_CONDITIONAL([KX_HOTSPOT], true)
fi


# ---- XML module ---------------------------
AC_ARG_ENABLE(xml,
[  --disable-xml   Disable building xml module ],
[ 
kx_xml="${enableval}"
],[kx_xml=yes])

if test "$kx_xml" = yes; then

PKG_CHECK_MODULES(libxml, libxml-2.0)
LIBXML_CFLAGS="`xml2-config --cflags`"
LIBXML_LIBS="`xml2-config --libs`"

AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)

AM_CONDITIONAL([KX_XML_MODULE], true)
else
AM_CONDITIONAL([KX_XML_MODULE], false)
AC_MSG_NOTICE([Module XML disabled])
fi

# ---- GTK module ---------------------------
# ---- XML module ---------------------------
AC_ARG_ENABLE(gtk,
[  --disable-gtk   Disable building GTK module ],
[ 
kx_gtk="${enableval}"
],[kx_gtk=yes])

if test "$kx_gtk" = yes; then

PKG_CHECK_MODULES([gtk], [gtk+-2.0])

AM_CONDITIONAL([KX_GTK_MODULE], true)
else
AM_CONDITIONAL([KX_GTK_MODULE], false)
AC_MSG_NOTICE([Module GTK disabled])
fi


AC_OUTPUT
