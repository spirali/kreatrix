/*
   kxassociation.c
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
	
#include "kxassociation.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxcore.h"

KxObjectExtension kxassociation_extension;

static void kxassociation_free(KxAssociation *self);
static void kxassociation_mark(KxAssociation *self);
static KxAssociation * kxassociation_clone(KxAssociation *self);
static void kxassociation_add_method_table(KxAssociation *self);


KxAssociation *
kxassociation_new_prototype(KxCore *core)
{
	KxObject *object = kxcore_clone_base_object(core);
	object->extension = &kxassociation_extension;

	object->data.data2.ptr1 = core->object_nil;
	object->data.data2.ptr2 = core->object_nil;
	REF_ADD2(core->object_nil);

	kxassociation_add_method_table(object);

	return object;

}

void
kxassociation_init_extension() 
{
	kxobjectext_init(&kxassociation_extension);
	kxassociation_extension.type_name = "Association";
	kxassociation_extension.free = kxassociation_free;
	kxassociation_extension.mark = kxassociation_mark;
	kxassociation_extension.clone = kxassociation_clone;
}

static void 
kxassociation_free(KxAssociation *self) 
{
	REF_REMOVE(KXASSOCIATION_GET_KEY(self));
	REF_REMOVE(KXASSOCIATION_GET_VALUE(self));
}

static void 
kxassociation_mark(KxAssociation *self) 
{
	kxobject_mark(KXASSOCIATION_GET_KEY(self));
	kxobject_mark(KXASSOCIATION_GET_VALUE(self));
}

static KxAssociation * 
kxassociation_clone(KxAssociation *self) 
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.data2.ptr1 = KXASSOCIATION_GET_KEY(self);
	REF_ADD(KXASSOCIATION_GET_KEY(clone));
	clone->data.data2.ptr2 = KXASSOCIATION_GET_VALUE(self);
	REF_ADD(KXASSOCIATION_GET_VALUE(clone));
	return clone;
}

static KxObject *
kxassociation_key(KxAssociation *self, KxMessage *message)
{
	KXRETURN(KXASSOCIATION_GET_KEY(self));
}

static KxObject *
kxassociation_value(KxAssociation *self, KxMessage *message)
{
	KXRETURN(KXASSOCIATION_GET_VALUE(self));
}

static KxObject *
kxassociation_set_key(KxAssociation *self, KxMessage *message)
{
	REF_REMOVE(KXASSOCIATION_GET_KEY(self));
	self->data.data2.ptr1 = message->params[0];
	REF_ADD(message->params[0]);
	KXRETURN(self);
}

static KxObject *
kxassociation_set_value(KxAssociation *self, KxMessage *message)
{
	REF_REMOVE(KXASSOCIATION_GET_VALUE(self));
	self->data.data2.ptr2 = message->params[0];
	REF_ADD(message->params[0]);
	KXRETURN(self);
}

static void 
kxassociation_add_method_table(KxAssociation *self)
{
	KxMethodTable table[] = {
		{"key", 0, kxassociation_key},
		{"value", 0, kxassociation_value},
		{"key:", 1, kxassociation_set_key},
		{"value:", 1, kxassociation_set_value},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}


