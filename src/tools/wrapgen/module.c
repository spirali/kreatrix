%include%

#include <kreatrix/kxmodule.h>
#include <kreatrix/kxmessage.h>
#include <kreatrix/kxcfunction.h>
#include <kreatrix/kxobject.h>
#include <kreatrix/kxexception.h>
#include <kreatrix/kxinteger.h>
#include <kreatrix/kxstring.h>

static void kx%modname%_add_method_table(KxObject *self);


KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{

%call_init_extensions%

%add_prototypes%

%init_prototype%

	kx%modname%_add_method_table(self);

	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
%remove_prototypes%
}

%cfunctions%

static void
kx%modname%_add_method_table(KxObject *self)
{
	KxMethodTable table[] = {
%cfunctions_table%
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
