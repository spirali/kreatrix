
#BUILT_SOURCES = grammar.h
#AM_YFLAGS = -d


noinst_LIBRARIES = libkxcompiler.a


libkxcompiler_a_SOURCES = \
		kxcomp.c \
		kxcomp.h \
		kxparser.c \
		kxparser.h \
		kxcompiler.c \
		kxcompiler.h \
		block.c \
		block.h \
		kxinstr.h \
		kxinstr.c \
		block_optimise.c \
		block_optimise.h

#libkxcompiler_a_LDFLAGS = -L../utils kxutils
libkxcompiler_a_LIBADD = ../utils/libkxutils.a
