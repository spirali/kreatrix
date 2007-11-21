
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxrandom.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"

void static kxrandom_add_method_table(KxObject *self);



KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxrandom_extension_init();

	KxObject *kxrandom_prototype = kxrandom_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxrandom_prototype);
	
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Random"),kxrandom_prototype);
	
	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxrandom_extension);
}


