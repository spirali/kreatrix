
SUBDIRS = utils compiler tools vminit .
# testing purpose
#SUBDIRS = utils compiler .

bin_PROGRAMS = kreatrix

CFLAGS+=-Wall -rdynamic
CFLAGS+=-DPKGLIBDIR="\"$(pkglibdir)\""
CFLAGS+=-DPKGINCLUDEDIR="\"$(pkgincludedir)\""
#-DLIBEXT="\"$(EXEEXT)\"" 

kreatrix_LDFLAGS = -ldl -lm
kreatrix_LDADD = compiler/libkxcompiler.a utils/libkxutils.a


kreatrix_SOURCES = \
	main.c \
	kxtypes.h \
	kxobject.c \
	kxobject.h \
	kxobject_struct.h \
	kxobjectext.h \
	kxobjectext.c \
	kxcore.c \
	kxcore.h \
	kxsymbol.c \
	kxsymbol.h \
	kxbaseobject.h \
	kxbaseobject.c \
	kxcfunction.c \
	kxcfunction.h \
	kxstack.h \
	kxstack.c \
	kxinteger.h \
	kxinteger.c \
	kxmessage.h \
	kxmessage.c \
	kxstring.h \
	kxstring.c \
	kxcodeblock.c \
	kxcodeblock.h \
	kxactivation.c \
	kxactivation.h \
	kxbytecode.h \
	kxbytecode.c \
	kxlobby.h \
	kxlobby.c \
	kxgc.h \
	kxgc.c \
	kxboolean.h \
	kxboolean.c \
	kxscopedblock.h \
	kxscopedblock.c \
	kxexception.h \
	kxexception.c \
	kxlist.h \
	kxlist.c \
	kxnil.h \
	kxnil.c \
	kxfile.c \
	kxfile.h \
	kxbytearray.h \
	kxbytearray.c \
	kxcharacter.h \
	kxcharacter.c \
	kxdictionary.h \
	kxdictionary.c \
	vminit/kxvminit.h \
	vminit/kxvminit.c \
	kxvm.h \
	kxvm.c \
	kxfloat.h \
	kxfloat.c \
	kxmodule.h \
	kxmodule.c \
	kxmodules_path.c \
	kxcompiler.c \
	kxcompile_utils.c \
	kxset_utils.h \
	kxset_utils.c \
	kxset.h \
	kxset.c \
	kxdictionary_utils.c \
	kxdictionary_utils.h \
	kxobject_slots.c \
	kxobject_slots.h \
	kxglobals.h \
	kxglobals.c \
	kxarray2d.c \
	kxarray2d.h \
	kxlocalcontext.c \
	kxlocalcontext.h \
	kxdecompiler.h \
	kxdecompiler.c \
	kxiterator.c \
	kxiterator.h \
	kxobject_profile.h \
	kxobject_profile.c \
	kxassociation.c \
	kxassociation.h \
	kxcallback.c \
	kxcallback.h 

pkginclude_HEADERS=\
	kxtypes.h \
	kxobject.h \
	kxobject_struct.h \
	kxobjectext.h \
	kxcore.h \
	kxsymbol.h \
	kxbaseobject.h \
	kxcfunction.h \
	kxstack.h \
	kxinteger.h \
	kxmessage.h \
	kxstring.h \
	kxcodeblock.h \
	kxactivation.h \
	kxbytecode.h \
	kxlobby.h \
	kxgc.h \
	kxboolean.h \
	kxscopedblock.h \
	kxexception.h \
	kxlist.h \
	kxnil.h \
	kxfile.h \
	kxbytearray.h \
	kxcharacter.h \
	kxdictionary.h \
	vminit/kxvminit.h \
	kxvm.h \
	kxfloat.h \
	kxmodule.h \
	kxmodules_path.h \
	kxcompiler.h \
	kxcompile_utils.h \
	kxset_utils.h \
	kxset.h \
	kxdictionary_utils.h \
	kxobject_slots.h \
	kxglobals.h \
	kxarray2d.h \
	kxlocalcontext.h \
	kxdecompiler.h \
	kxiterator.h \
	kxobject_profile.h \
	kxassociation.h \
	kxcallback.h

EXTRA_DIST=vminit/update.sh




