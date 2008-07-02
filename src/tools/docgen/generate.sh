#!/bin/sh

export KREATRIX_MODULES_PATH=../../../modules

cd `dirname $0`
rm ../../../docs/html/*.html
../../kreatrix grabdoc.kx ../../*.c
../../kreatrix -d doc2html.kx Lobby sockets system pool cgi test threads xml sdl gtk random bytecode time


#../../kreatrix -dv doc2html.kx Lobby
