/*
   kxpool.c
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
#include <stdio.h>
#include <string.h>

#include "kxpool.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxcompile_utils.h"
#include "kxglobals.h"
//#include "kxstack.h"

KxObjectExtension kxpool_extension;



static void kxpool_free(KxPool *self);
static void kxpool_clean(KxPool *self);
static void kxpool_mark(KxPool *self);
static KxObject * kxpool_clone(KxPool *self);

void static kxpool_add_method_table(KxObject *self);



void kxpool_extension_init()
{
	kxobjectext_init(&kxpool_extension);
	kxpool_extension.type_name = "Pool";
	kxpool_extension.free = kxpool_free;
	kxpool_extension.clean = kxpool_clean;
	kxpool_extension.mark = kxpool_mark;
	kxpool_extension.clone = kxpool_clone;
}

static void 
kxpool_free(KxPool *self) 
{
	KxPoolData *data = KXPOOL_DATA(self);

	dictionary_foreach_key(data->paths,kxobject_ref_remove);
	dictionary_foreach_value(data->paths,kxobject_ref_remove);
	dictionary_foreach_value(data->objects,kxobject_ref_remove);

	dictionary_free(data->objects);
	dictionary_free(data->paths);
	free(data);
}

static void
kxpool_mark(KxPool *self) 
{
	KxPoolData *data = KXPOOL_DATA(self);
	dictionary_foreach_key(data->paths,(ListForeachFcn*) kxobject_mark);
	dictionary_foreach_value(data->paths,(ListForeachFcn*) kxobject_mark);
	dictionary_foreach_value(data->objects,(ListForeachFcn*) kxobject_mark);
}

static void
kxpool_clean(KxPool *self)
{
	// TODO
}

static KxObject *
kxpool_clone(KxPool *self) 
{
	KxObject *clone = kxobject_raw_clone(self);
	
	KxPoolData *data = malloc(sizeof(KxPoolData));
	ALLOCTEST(data);

	data->objects = dictionary_new();
	data->paths = dictionary_new();

	clone->data.ptr = data;
	return clone;
}

KxObject * 
kxpool_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxpool_extension;

	KxPoolData *data = malloc(sizeof(KxPoolData));
	ALLOCTEST(data);
	
	data->objects = dictionary_new();
	data->paths = dictionary_new();


	self->data.ptr = data;
	kxpool_add_method_table(self);
	return self;
}

static KxObject *
kxpool_add_file(KxPool *self, KxMessage *message)
{
	KxObject *param0 = message->params[0];
	KxObject *param1 = message->params[1];

	if (!IS_KXSYMBOL(param0)) {
		KXTHROW_EXCEPTION("First parameter must be symbol");
	}
	if (!IS_KXSTRING(param1)) {
		KXTHROW_EXCEPTION("Second parameter must be string");
	}

	KxPoolData *data = KXPOOL_DATA(self);
	REF_ADD(param0);
	REF_ADD(param1);
	dictionary_add(data->paths,param0,param1);

	KXRETURN(self);
}

static KxObject *
kxpool_message_hook(KxPool *self, KxMessage *message) 
{
	KxSymbol *name = message->params[0];
	KxPoolData *data = KXPOOL_DATA(self);
	KxObject *obj = dictionary_get(data->objects, name);
	if (obj) {
		KXRETURN(obj);
	} 


	KxObject *path = dictionary_get(data->paths, name);
	if (path == NULL) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Object '%s' not found in pool", name->data.ptr);
		KXTHROW(excp);
	}

	char *path_str = KXSTRING_VALUE(path);
	KxObject *object = kxcompile_do_file(path_str,
		KXCORE->lobby, message->message_name, kx_doc_flag);
	KXCHECK(object);

	dictionary_add(data->objects, name, object);

	KXRETURN(object);
}


static KxObject *
kxpool_as_string(KxPool *self, KxMessage *message)
{
	KxPoolData *data = KXPOOL_DATA(self);
	List *keys = dictionary_keys_as_list(data->objects);
	List *values = dictionary_values_as_list(data->objects);
	
	char *str = NULL;
	int len = 0;

	int t;
	for (t=0;t<keys->size;t++) {
		char tmp[250];
		snprintf(tmp,250,"%s:%i\n",((KxObject*)keys->items[t])->data.ptr, 
			((KxObject*)values->items[t])->ref_count);
		int old_len = len;
		len += strlen(tmp);
		str = realloc(str,len+1);
		ALLOCTEST(str);
		strcpy(str+old_len, tmp);
	}
	list_free(keys);
	list_free(values);
	if (str == NULL)
		str = strdup("");
	KxString *obj_str = KXSTRING(str);
	free(str);
	//RETURN(obj_str);
	return obj_str;
}

static KxObject *
kxpool_purge(KxPool *self, KxMessage *message)
{
	KxPoolData *data = KXPOOL_DATA(self);
	List *keys = dictionary_keys_as_list(data->objects);
	List *values = dictionary_values_as_list(data->objects);
	
	int t;
	for (t=0;t<values->size;t++) {
		KxObject *obj = values->items[t];
		if (obj->ref_count == 1) {
			dictionary_remove(data->objects, keys->items[t]);
			REF_REMOVE(obj);
		}
	}

	list_free(keys);
	list_free(values);
	
	KXRETURN(self);
}


void static
kxpool_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"add:file:",2, kxpool_add_file },
		{"doesNotUnderstand:parameters:",2, kxpool_message_hook },
		{"asString",0, kxpool_as_string},
		{"purge",0, kxpool_purge},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

