/*
   kxactivation_object.c
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
	
#include <stdlib.h>

#include "kxlocalcontext.h"
#include "kxobject.h"
#include "kxcore.h"
#include "kxactivation.h"
#include "utils/utils.h"
#include "kxexception.h"

KxObjectExtension kxlocalcontext_extension;

static void kxlocalcontext_free(KxActivationObject *self);
static void kxlocalcontext_add_method_table(KxActivationObject *self);
static void kxlocalcontext_mark(KxActivationObject *self);
static void kxlocalcontext_clean(KxActivationObject *self);


void
kxlocalcontext_init_extension() {
	kxobjectext_init(&kxlocalcontext_extension);
	kxlocalcontext_extension.type_name = "LocalContext";
	kxlocalcontext_extension.free = kxlocalcontext_free;
	kxlocalcontext_extension.mark = kxlocalcontext_mark;
	kxlocalcontext_extension.clean = kxlocalcontext_clean;
}


KxObject *
kxlocalcontext_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxlocalcontext_extension;
	object->data.ptr = NULL;

	kxlocalcontext_add_method_table(object);

	return object;
}

KxActivationObject *
kxlocalcontext_new(KxCore *core, struct KxActivation *activation)
{
	KxObject *proto = kxcore_get_basic_prototype(core,KXPROTO_LOCALCONTEXT);
	KxObject *self = kxobject_raw_clone(proto);

	activation->ref_count++;
	self->data.ptr = activation;

	return self;
}



static void
kxlocalcontext_free(KxActivationObject *self) 
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL)
		return;

	if (--activation->ref_count == 0)  {
		kxactivation_free(activation);
	}
}

static void
kxlocalcontext_clean(KxActivationObject *self) 
{
	kxlocalcontext_free(self);
	self->data.ptr = NULL;
}


static void
kxlocalcontext_mark(KxActivationObject *self) 
{
	KxActivation *activation = self->data.ptr;
	if (activation == NULL)
		return;
	if (--activation->ref_count == 0)  {
		kxactivation_free(activation);
	}
}

static KxObject *
kxlocalcontext_throw_invalid_activation(KxActivationObject *self)
{
	KxException *excp = kxexception_new_with_text(KXCORE, "Invalid activation");
	KXTHROW(excp);
}

static KxObject *
kxlocalcontext_codeblock(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;
	if (activation == NULL) {
		return kxlocalcontext_throw_invalid_activation(self);
	}

	KXRETURN(activation->codeblock);
}

static KxObject *
kxlocalcontext_get_local(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL) {
		return kxlocalcontext_throw_invalid_activation(self);
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
kxlocalcontext_put_local(KxActivationObject *self, KxMessage *message)
{
	KxActivation *activation = self->data.ptr;

	if (activation == NULL) {
		return kxlocalcontext_throw_invalid_activation(self);
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
kxlocalcontext_add_method_table(KxActivationObject *self)
{
	KxMethodTable table[] = {
		{"codeblock",0, kxlocalcontext_codeblock },
		{"local:",1, kxlocalcontext_get_local },
		{"local:put:",2, kxlocalcontext_put_local },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

