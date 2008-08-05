/*
   kxipaddress.h
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
	

#ifndef __KXIPADDRESS_H
#define __KXIPADDRESS_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXIPADDRESS_DATA(self) ((KxIPAddressData*) ((self)->data.ptr))

#define IS_KXIPADDRESS(self) ((self)->extension == &kxipaddress_extension)

#define KXIPADDRESS(addr) (kxipaddress_new_with_addr(KXCORE,addr))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct KxObject KxIPAddress;

typedef struct KxIPAddressData KxIPAddressData;

struct KxIPAddressData {
	struct sockaddr_in addr;
	//socklen_t addr_len;
};

void kxipaddress_extension_init();
KxObject *kxipaddress_new_prototype(KxCore *core);
KxIPAddress *kxipaddress_new_with_addr(KxCore *core, struct sockaddr_in addr);

struct sockaddr_in kxipaddress_to_sockaddr(KxIPAddress *self);


extern KxObjectExtension kxipaddress_extension;

#endif // __KXIPADDRESS_H
