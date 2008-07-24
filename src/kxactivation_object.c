/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>

#include "kxactivation_object.h"
#include "kxobject.h"
#include "kxcore.h"
#include "kxactivation.h"
#include "utils/utils.h"
#include "kxexception.h"

KxObjectExtension kxactivationobject_extension;

static void kxactivationobject_free(KxActivationObject *self);
static void kxactivationobject_add_method_table(KxActivationObject *self);
static void kxactivationobject_mark(KxActivationObject *self);
static void kxactivationobject_clean(KxActivationObject *self);


void
kxactivationobject_init_extension() {
	kxobjectext_init(&kxactivationobject_extension);
	kxactivationobject_extension.type_name = "Activation";
	kxactivationobject_extension.free = kxactivationobject_free;
	kxactivationobject_extension.mark = kxactivationobject_mark;
	kxactivationobject_extension.clean = kxactivationobject_clean;
}


KxObject *
kxactivationobject_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxactivationobject_extension;
	object->data.ptr = NULL;

	kxactivationobject_add_method_table(object);

	return object;
}

KxActivationObject *
kxactivationobject_new(KxCore *core, struct KxActivation *activation)
{
	KxObject *proto = kxcore_get_basic_prototype(core,KXPROTO_ACTIVATION);
	KxObject *self = kxobject_raw_clone(proto);

	activation->ref_count++;
	self->data.ptr = activation;

	return self;
}



static void
kxactivationobject_free(KxActivationObject *self) 
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL)
		return;

	if (--activation->ref_count == 0)  {
		kxactivation_free(activation);
	}
}

static void
kxactivationobject_clean(KxActivationObject *self) 
{
	kxactivationobject_free(self);
	self->data.ptr = NULL;
}


static void
kxactivationobject_mark(KxActivationObject *self) 
{
	KxActivation *activation = self->data.ptr;
	if (activation == NULL)
		return;
	if (--activation->ref_count == 0)  {
		kxactivation_free(activation);
	}
}

static KxObject *
kxactivationobject_throw_invalid_activation(KxActivationObject *self)
{
	KxException *excp = kxexception_new_with_text(KXCORE, "Invalid activation");
	KXTHROW(excp);
}

static KxObject *
kxactivationobject_codeblock(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;
	if (activation == NULL) {
		return kxactivationobject_throw_invalid_activation(self);
	}

	KXRETURN(activation->codeblock);
}

static KxObject *
kxactivationobject_get_local(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL) {
		return kxactivationobject_throw_invalid_activation(self);
	}

	KxSymbol *name = message->params[0];
	KXCHECK_SYMBOL(name, 0);
	KxObject *obj = kxactivation_get_local(activation, name);
	if (obj)
		return obj;

	KxException *excp = kxexception_new_with_text(KXCORE,
				"Local slot '%s' not found", KXSYMBOL_AS_CSTRING(name));
	KXTHROW(excp);
}

static KxObject *
kxactivationobject_put_local(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL) {
		return kxactivationobject_throw_invalid_activation(self);
	}

	KxSymbol *name = message->params[0];
	KXCHECK_SYMBOL(name, 0);
	if (kxactivation_put_local(activation, name, message->params[1])) {
		KXRETURN(self);
	}

	KxException *excp = kxexception_new_with_text(KXCORE,
				"Local slot '%s' not found", KXSYMBOL_AS_CSTRING(name));
	KXTHROW(excp);


	KXRETURN(activation->codeblock);
}


static void 
kxactivationobject_add_method_table(KxActivationObject *self)
{
	KxMethodTable table[] = {
		{"codeblock",0, kxactivationobject_codeblock },
		{"local:",1, kxactivationobject_get_local },
		{"local:put:",2, kxactivationobject_put_local },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

