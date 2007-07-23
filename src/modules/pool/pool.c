
#include <stdio.h>


#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxpool.h"

KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxpool_extension_init();

	KxPool *kxpool_prototype = kxpool_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxpool_prototype);
	
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Pool"),kxpool_prototype);
	
	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxpool_extension);
}




