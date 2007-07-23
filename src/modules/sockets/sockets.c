

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

