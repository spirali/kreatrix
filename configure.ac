AC_INIT([Kreatrix],[0.8.2],[spirali@spiralia.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
#AC_PROG_LEX
#AC_PROG_YACC

AC_LIBTOOL_DLOPEN
#AC_PROG_LIBTOOL
AC_PROG_RANLIB

AC_EXEEXT

# Allow disabling this feature
PKG_CHECK_MODULES(libxml, libxml-2.0)


AC_CHECK_LIB(dl, dlopen)

AC_CONFIG_HEADERS([config.h])
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
		src/modules/Makefile
		src/modules/test/Makefile
		src/modules/system/Makefile
		src/modules/sockets/Makefile
		src/modules/pool/Makefile
		src/modules/cgi/Makefile
		src/modules/threads/Makefile
		src/modules/xml/Makefile
		src/vminit/Makefile
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


# XML module
AC_ARG_ENABLE(xml,
[  --disable-xml   Disable building xml module ],
[ 
kx_xml="${enableval}"
],[kx_xml=yes])

if test "$kx_xml" = yes; then
LIBXML_CFLAGS="`xml2-config --cflags`"
LIBXML_LIBS="`xml2-config --libs`"

AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)

AM_CONDITIONAL([KX_XML_MODULE], true)
else
AM_CONDITIONAL([KX_XML_MODULE], false)
AC_MSG_NOTICE([Module XML disabled])
fi

AC_OUTPUT