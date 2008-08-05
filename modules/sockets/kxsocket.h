/*
   kxsocket.h
   Copyright (C) 2007, 2008  Stanislav Bohm

   This file is part of Kreatrix.

   Kreatrix is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Kreatrix is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Kreatrix; see the file COPYING. If not, see 
   <http://www.gnu.org/licenses/>.
*/
	

#ifndef __KXSOCKET_H
#define __KXSOCKET_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXSOCKET_DATA(self) ((KxSocketData*) ((self)->data.ptr))

#define IS_KXSOCKET(self) ((self)->extension == &kxsocket_extension)

#define KXSOCKET(sock) (kxsocket_new_with_socket(KXCORE,sock))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define KXLISTEN_QUEUE 16

typedef struct KxObject KxSocket;

typedef struct KxSocketData KxSocketData;

#define KXSOCKET_READBUFFER_SIZE 4096

struct KxSocketData {
	int socket;
	int port;
	char readbuffer[KXSOCKET_READBUFFER_SIZE];
	int readbuffer_pos;
	int readbuffer_size;
};

void kxsocket_extension_init();
KxObject *kxsocket_new_prototype(KxCore *core);
KxSocket *kxsocket_new_with_socket(KxCore *core, int socket);


extern KxObjectExtension kxsocket_extension;

#endif // __KXSOCKET_H
