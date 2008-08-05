/*
   kxbytearray.c
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
#include <string.h>

#include "kxbytearray.h"
#include "kxobject.h"
#include "kxinteger.h"
#include "kxexception.h"
#include "kxcharacter.h"

KxObjectExtension kxbytearray_extension;

static void kxbytearray_free(KxByteArray *self);
static void kxbytearray_add_methods_table(KxByteArray *self);
static KxByteArray * kxbytearray_clone(KxByteArray *self);

void
kxbytearray_init_extension() 
{
	kxobjectext_init(&kxbytearray_extension);
	kxbytearray_extension.type_name = "ByteArray";
	kxbytearray_extension.free = kxbytearray_free;
	kxbytearray_extension.clone = kxbytearray_clone;

}

KxByteArray *
kxbytearray_new_prototype(KxCore *core) 
{
	KxByteArray *self = kxcore_clone_base_object(core);
	self->extension = &kxbytearray_extension;
	self->data.ptr = bytearray_new();

	kxbytearray_add_methods_table(self);
	return self;
}


KxByteArray *
kxbytearray_new_with(KxCore *core, ByteArray *bytearray)
{
	KxByteArray *clone = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_BYTEARRAY));
	clone->data.ptr = bytearray;
	return clone;
}



static void 
kxbytearray_free(KxByteArray *self) 
{
	bytearray_free(self->data.ptr);
}

static KxByteArray * 
kxbytearray_clone(KxByteArray *self)
{
	KxByteArray *clone = kxobject_raw_clone(self);
	clone->data.ptr = bytearray_copy((ByteArray*)self->data.ptr);
	return clone;
}


static KxObject *
kxbytearray_size(KxByteArray *self, KxMessage *message)
{
	ByteArray *data = self->data.ptr;
	return KXINTEGER(data->size);
}

static KxObject *
kxbytearray_at(KxByteArray *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	ByteArray *data = self->data.ptr;
	if (param < 0 || param >= data->size) {
		KXTHROW_EXCEPTION("Index out of bounds");
	}
    return KXINTEGER(bytearray_at(data,param));
}

static KxObject *
kxbytearray_at_put(KxByteArray *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	KXPARAM_TO_LONG(value,1);
	ByteArray *data = self->data.ptr;

	if (param < 0 || param >= data->size) {
		KXTHROW_EXCEPTION("Index out of bounds");
	}

    bytearray_at_put(data,param, value);
	KXRETURN(self);
}


static KxObject *
kxbytearray_as_string(KxByteArray *self, KxMessage *message)
{
	ByteArray *data = self->data.ptr;
	if (data->size > 0 && data->array[data->size-1] == 0) {
		return KXSTRING(data->array);
	}
	char str[data->size+1];
	str[data->size] = 0;
	memcpy(str,data->array, data->size);
	return KXSTRING(str);
}

static KxObject *
kxbytearray_as_int32(KxByteArray *self, KxMessage *message)
{
	ByteArray *data = self->data.ptr;
	return KXINTEGER(bytearray_as_int32(data,0));
}

static KxObject *
kxbytearray_set_size(KxByteArray *self, KxMessage *message)
{
	KXPARAM_TO_LONG(newsize, 0);
	ByteArray *data = self->data.ptr;
	bytearray_set_size(data, newsize);
	KXRETURN(self);
}

static KxObject *
kxbytearray_cut_line(KxByteArray *self, KxMessage *message)
{
	ByteArray *data = KXBYTEARRAY_DATA(self);
	char *array = data->array;
	int size = data->size;
	int t;
	for (t=0;t<size;t++) {
		if (array[t] == '\n') {
			break;
		}
	}
	if (t == data->size) {
		KXRETURN(KXCORE->object_nil);
	}
	ByteArray *ba = bytearray_new_from_data(array+t+1, size - t -1);
	self->data.ptr = ba;

	data->array = NULL;
	bytearray_free(data);

	char *str = kxrealloc(array, t + 2);
	ALLOCTEST(str);
	str[t+1] = 0;


	return kxstring_from_cstring(KXCORE,str);
}

static KxObject *
kxbytearray_contains_byte(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_CHAR(c, 0);
	ByteArray *data = KXBYTEARRAY_DATA(self);
	char *array = data->array;
	int t;
	for (t=0;t<data->size;t++) {
		if (array[t] == c) {
			KXRETURN(KXCORE->object_true);	
		}
	}
	KXRETURN(KXCORE->object_false);	
}

static KxObject * 
kxbytearray_add(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_BYTEARRAY(param, 0);
	ByteArray *data = KXBYTEARRAY_DATA(self);
	bytearray_append(data, param->array, param->size);
	KXRETURN(self);
}

static void kxbytearray_add_methods_table(KxByteArray *self) 
{
	KxMethodTable table[] = {
		{"size",0,kxbytearray_size},
		{"at:", 1, kxbytearray_at },
		{"at:put:", 2, kxbytearray_at_put },
		{"asString", 0, kxbytearray_as_string },
		{"asInt32", 0, kxbytearray_as_int32},
		{"add:", 1,  kxbytearray_add },
		{"asByteArray", 0, kxcfunction_returnself},
		{"size:",1,kxbytearray_set_size},
		{"cutLine",0,kxbytearray_cut_line},
		{"containsByte:",1,kxbytearray_contains_byte},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

void 
kxbytearray_type_error(KxCore *core)
{
	KxException *excp = kxexception_new_with_text(
		core,"ByteArray expected as parameter");
	kxstack_throw_object(core->stack, excp);
}
