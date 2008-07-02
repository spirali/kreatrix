#!/bin/sh

cd `dirname $0`

aclocal || exit 1
autoconf || exit 1
autoheader || exit 1
automake -a -c || exit 1
