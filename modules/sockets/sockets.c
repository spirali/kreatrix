/*
   sockets.c
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
	


#include <stdio.h>
#include <errno.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxipaddress.h"
#include "kxsocket.h"
#include "kxpoll.h"


KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxipaddress_extension_init();
	kxsocket_extension_init();
	kxpoll_extension_init();

	KxObject *ipaddress = kxipaddress_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, ipaddress);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("IPAddress"),ipaddress);

	KxObject *socket = kxsocket_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, socket);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Socket"),socket);

	KxObject *poll = kxpoll_new_prototype(KXCORE);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Poll"),poll);



	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxipaddress_extension);
	kxcore_remove_prototype(KXCORE, &kxsocket_extension);
}

