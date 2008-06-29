#!/bin/sh

make distclean;
find -name \*.in -exec rm {} \;
rm -rf configure missing install-sh aclocal.m4 depcomp autom4te.cache

