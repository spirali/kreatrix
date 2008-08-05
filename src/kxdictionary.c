/*
   kxdictionary.c
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
	
/*KXobject Base Dictionary
 [Data structures] 
*/

#include <stdlib.h>
#include <stdio.h>

#include "kxdictionary.h"

#include "kxdictionary_utils.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxlist.h"

KxObjectExtension kxdictionary_extension;

static void kxdictionary_free(KxDictionary *self);
static void kxdictionary_mark(KxDictionary *self);
static KxDictionary * kxdictionary_clone(KxDictionary *self);
static void kxdictionary_clean(KxDictionary *self);


static void kxdictionary_add_methods_table(KxDictionary *self);

void
kxdictionary_init_extension() 
{
	kxobjectext_init(&kxdictionary_extension);
	kxdictionary_extension.type_name = "Dictionary";
	kxdictionary_extension.free = kxdictionary_free;
	kxdictionary_extension.mark = kxdictionary_mark;
	kxdictionary_extension.clone = kxdictionary_clone;
	kxdictionary_extension.clean  = kxdictionary_clean;
}

KxDictionary *
kxdictionary_new_prototype(KxCore *core) 
{
	KxDictionary *self = kxcore_clone_base_object(core);
	self->extension = &kxdictionary_extension;
	self->data.ptr = kxbasedictionary_new();

	kxdictionary_add_methods_table(self);
	return self;
}



static void 
kxdictionary_free(KxDictionary *self) 
{
	kxbasedictionary_free(self->data.ptr);
}


static void
kxdictionary_clean(KxDictionary *self) {
	kxbasedictionary_clean(self->data.ptr);
}

static void
kxdictionary_mark(KxDictionary *self)
{
	kxbasedictionary_mark(self->data.ptr);
}

static KxObject *
kxdictionary_add(KxDictionary *self, KxMessage *message)
{

	if (!kxbasedictionary_add(self->data.ptr, message->params[0]))
		return NULL;
	KXRETURN(self);
}

static KxObject *
kxdictionary_association_at(KxDictionary *self, KxMessage *message)
{
	KxObject *obj =  kxbasedictionary_at(self->data.ptr, message->params[0]);

	KXCHECK(obj);

	KXRETURN(obj);
}

static KxObject *
kxdictionary_foreach(KxDictionary *self, KxMessage *message)
{

	KxObject *block = message->params[0];
	KxBaseDictionary *dict = self->data.ptr;

	if (dict->items_count == 0) {
		KXRETURN(KXCORE->object_nil);	
	}

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;

	int t;
	for (t=0;t<dict->array_size;t++) {
		if (dict->array[t]) {
			msg.params[0] = dict->array[t];
			KxObject * obj = kxobject_evaluate_block(block,&msg);
			KXCHECK(obj);
			REF_REMOVE(obj);
		}
	}
	KXRETURN(KXCORE->object_nil);

	KXTHROW_EXCEPTION("Not implemented");
}

static KxObject *
kxdictionary_size(KxDictionary *self, KxMessage *message)
{
	KxBaseDictionary *dictionary =  self->data.ptr;
	return KXINTEGER(dictionary->items_count);
}


static KxDictionary * 
kxdictionary_clone(KxDictionary *self) 
{
	KxObject *clone = kxobject_raw_clone(self);

	KxBaseDictionary *dictionary = self->data.ptr;
	KxBaseDictionary *out = kxbasedictionary_copy(dictionary);
	clone->data.ptr= out;
	return clone;
}

static KxObject * 
kxdictionary_remove(KxDictionary *self, KxMessage *message) 
{
	int r = kxbasedictionary_remove(self->data.ptr, message->params[0]);
	if (r == -1)
		return NULL;

	if (r == 0) {
		KXTHROW_EXCEPTION("Element not found in set.");
	}
	KXRETURN(self);
}

static KxObject * 
kxdictionary_remove_ifabsent(KxDictionary *self, KxMessage *message) 
{
	int r = kxbasedictionary_remove(self->data.ptr, message->params[0]);
	if (r == -1)
		return NULL;

	if (r == 0) {
		return kxobject_evaluate_block_simple(message->params[1]);
	}
	KXRETURN(self);
}



static void
kxdictionary_add_methods_table(KxDictionary *self) 
{
	KxMethodTable table[] = {
		{"associationAt:", 1, kxdictionary_association_at },
		{"add:", 1, kxdictionary_add },
		{"size",0, kxdictionary_size},
		{"foreach:", 1, kxdictionary_foreach },
		{"remove:", 1, kxdictionary_remove },
		{"remove:ifAbsent:", 2, kxdictionary_remove_ifabsent },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);

}
