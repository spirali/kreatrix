/*
   kxcfunction.c
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
#include <stdlib.h>

#include "kxcfunction.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxexception.h"

KxObjectExtension kxcfunction_extension;
static KxObject * kxcfunction_activate(KxCFunction *self, KxObject *target, KxMessage *message);
static void kxcfunction_free(KxCFunction *self);
static void kxcfunction_mark(KxCFunction *self);

void 
kxcfunction_init_extenstion()
{
	kxobjectext_init(&kxcfunction_extension);
	kxcfunction_extension.type_name = "CFunction";
	kxcfunction_extension.activate = kxcfunction_activate;
	kxcfunction_extension.free = kxcfunction_free;
	kxcfunction_extension.mark = kxcfunction_mark;
	kxcfunction_extension.is_immutable = 1;
}

static void
kxcfunction_free(KxCFunction *self) 
{
	KxCFunctionData *data = self->data.ptr;
	if (data->objects) {
		KxObject **obj = data->objects;
		while (*obj) {
			REF_REMOVE(*obj);
			obj++;
		}
		kxfree(data->objects);
	}
	kxfree(data);
}

static void
kxcfunction_mark(KxCFunction *self) 
{
	KxCFunctionData *data = self->data.ptr;
	if (data->objects) {
		KxObject **obj = data->objects;
		while (*obj) {
			kxobject_mark(*obj);
			obj++;
		}
	}
}

KxObject *
kxcfunction_returnself(KxObject *self, KxMessage *message) 
{
	REF_ADD(self);
	return self;
}


KxObject * 
kxcfunction_new_prototype(KxCore *core)
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxcfunction_extension;
	
	KxCFunctionData *data = kxmalloc(sizeof(KxCFunctionData));
	ALLOCTEST(data);

	data->type_extension = NULL;
	data->params_count = 0;
	data->cfunction = kxcfunction_returnself;
	data->objects = NULL;

	object->data.ptr = data;


	return object;
}


KxCFunction * 
kxcfuntion_clone_with(KxCFunction *self, KxObjectExtension *type_ext, int params_count, KxExternFunction *cfunction) 
{
	KxCFunction *child = kxobject_raw_clone(self);
	
	KxCFunctionData *data = kxmalloc(sizeof(KxCFunctionData));
	ALLOCTEST(data);

	data->type_extension = type_ext;
	data->params_count = params_count;
	data->cfunction = cfunction;
	data->objects = NULL;

	child->data.ptr = data;
	return child;
}


static KxObject *
kxcfunction_activate(KxCFunction *self, KxObject *target, KxMessage *message) 
{
	KxCFunctionData *data = self->data.ptr;
	
	if (data->type_extension && data->type_extension != target->extension) {
		KxException *excp = kxexception_new_with_text(KXCORE,"CFunction type error: %s expected\n",
			data->type_extension->type_name);
		KXTHROW(excp);
	}

	if (data->params_count != message->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"CFunction type error: invalid count of parameters %i (%i expected)\n",
			message->params_count, data->params_count);
		KXTHROW(excp);
	}

	KxStack *stack = KXSTACK;
	KxCFunction *old = stack->running_cfunction;
	stack->running_cfunction = self;
	KxObject *obj = data->cfunction(target, message);
	stack->running_cfunction = old;
	return obj;
}

/** Creates symbol frame with one symbol */
void 
kxcfunction_objects_set_one(KxCFunction *self, KxObject *object)
{
	KxCFunctionData *data = self->data.ptr;
	
	KxSymbol **objs = kxmalloc(sizeof(KxSymbol *) * 2);
	ALLOCTEST(objs);

	data->objects = objs;
	
	REF_ADD(object);
	objs[0] = object;
	objs[1] = NULL;
}


/** Creates symbol frame from list of symbols */
void 
kxcfunction_objects_set_array(KxCFunction *self, KxObject **array, int size) 
{
	KxCFunctionData *data = self->data.ptr;
	
	KxSymbol **objs = kxmalloc(sizeof(KxSymbol *) * (size+1));
	ALLOCTEST(objs);

	data->objects = objs;
	
	objs[size] = NULL;
	
	int t;
	for (t=0;t<size;t++) {
		REF_ADD(array[t]);
		objs[t] = array[t];
	}

}

KxObject * 
kxcfunction_objects_find_by_extension(KxCFunction *self, KxObjectExtension *extension)
{
	KxCFunctionData *data = self->data.ptr;

	if (data->objects == NULL) {
		return NULL;
	}

	KxObject **obj = data->objects;

	do {
		if ((*obj)->extension == extension)
			return *obj;
		obj++;
	} while(*obj);

	return NULL;
}

void 
kxcfunction_objects_add_object(KxCFunction *self, KxObject *object)
{
	KxCFunctionData *data = self->data.ptr;
	
	if (data->objects == NULL) {
		kxcfunction_objects_set_one(self, object);
		return;
	}
	
	int size = 2;
	KxObject **obj = data->objects;

	do {
		size++;
		obj++;
	} while(*obj);

	data->objects = kxrealloc(data->objects, sizeof(KxObject *) * size);
	ALLOCTEST(data->objects);
	data->objects[size - 1] = NULL;
	REF_ADD(object);
	data->objects[size - 2] = object;
}

void 
kxcfunction_objects_remove_object(KxCFunction *self, KxObject *object)
{
	KxCFunctionData *data = self->data.ptr;
	
	if (data->objects == NULL) {
		return;
	}

	if (data->objects[0] == object && data->objects[1] == NULL) {
		REF_REMOVE(object);
		kxfree(data->objects);
		data->objects = NULL;
		return;
	}

	KxObject **objs = data->objects;

	do {
		if ((*objs) == object) {
			REF_REMOVE(object);
			do {
				*objs = *(objs + 1);
				objs++;
			} while(*objs);
			return;
		}
		objs++;
	} while(*objs);
}
