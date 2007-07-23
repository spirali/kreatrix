
#include <stdlib.h>

#include <kxmessage.h>
#include <kxobject.h>
#include <kxcfunction.h>

#include "kx%%name%%.h"

KxObjectExtension kx%%name%%_extension;

/**
 *   Prototype init:
 *
 *   KxObject *%%name%% = kx%%name%%_new_prototype(KXCORE);
 *   kxcore_add_prototype(KXCORE, %%name%%);
 *   kxobject_set_slot_no_ref2(self, KXSYMBOL("%%Name%%"), %%name%%);
 *
 */

static void
kx%%name%%_add_method_table(Kx%%Name%% *self);

static KxObject *
kx%%name%%_clone(Kx%%Name%% *self)
{
	Kx%%Name%% *clone = kxobject_raw_clone(self);
	
	// TODO

	return clone;
}

static void
kx%%name%%_free(Kx%%Name%% *self)
{
	// TODO
}

void kx%%name%%_extension_init() {
	kxobjectext_init(&kx%%name%%_extension);
	kx%%name%%_extension.type_name = "%%Name%%";

	kx%%name%%_extension.clone = kx%%name%%_clone;
	kx%%name%%_extension.free = kx%%name%%_free;
}

KxObject *
kx%%name%%_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kx%%name%%_extension;
	
	// TODO Init data
	
	kx%%name%%_add_method_table(self);
	return self;
}


static KxObject *
kx%%name%%_method(Kx%%Name%% *self, KxMessage *message)
{
	KXRETURN(self);
}

static void
kx%%name%%_add_method_table(Kx%%Name%% *self)
{
	KxMethodTable table[] = {
		{"method",0, kx%%name%%_method },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

