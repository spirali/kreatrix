
SUBDIRS = test system sockets time pool cgi

if KX_THREADS_SUPPORT
SUBDIRS+=threads
endif

if KX_XML_MODULE
SUBDIRS+=xml
endif

SUBDIRS+=gtk

noinst_SCRIPT= noinst_modules_gen.sh

EXTRA_DIST= noinst_modules_gen.sh

CLEANFILES = noinst_modules_paths.kx noinst_modules_paths.kxc

all-local: noinst_modules_paths.kx

noinst_modules_paths.kx: noinst_modules_gen.sh
	$(SHELL) $<
