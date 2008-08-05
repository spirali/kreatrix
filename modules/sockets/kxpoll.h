/*
   kxpoll.h
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
	

#ifndef __KXPOLL_H
#define __KXPOLL_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXPOLL_DATA(self) ((KxPollData*) ((self)->data.ptr))

#define IS_KXPOLL(self) ((self)->extension == &kxpoll_extension)

#define KXPOLL(addr) (kxpoll_new_with_addr(KXCORE,addr))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

typedef struct KxObject KxPoll;

typedef struct KxPollData KxPollData;

struct KxPollData {
	struct pollfd *pfd;
	int pfd_count;
	int timeout;
	List *obj_list;
};

void kxpoll_extension_init();
KxObject *kxpoll_new_prototype(KxCore *core);
KxPoll *kxpoll_new_with_addr(KxCore *core, struct sockaddr_in addr);

struct sockaddr_in kxpoll_to_sockaddr(KxPoll *self);

KxObject *kxpollresult_clone_with(KxObject *self, int result);


extern KxObjectExtension kxpoll_extension;

#endif // __KXPOLL_H
