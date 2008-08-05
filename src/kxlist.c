/*
   kxlist.c
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

#include "kxlist.h"
#include "kxobject.h"
#include "utils/list.h"
#include "utils/utils.h"
#include "kxinteger.h"
#include "kxexception.h"

/*KXobject Base List
 [Data structures] 
*/

KxObjectExtension kxlist_extension;

static void kxlist_free(KxList *self);

static void kxlist_add_methods_table(KxList *self);
static void kxlist_mark(KxList *self);
static KxList * kxlist_clone(KxList *self);
static void kxlist_clean(KxList *self);

void
kxlist_init_extension() 
{
	kxobjectext_init(&kxlist_extension);
	kxlist_extension.type_name = "List";
	kxlist_extension.free = kxlist_free;
	kxlist_extension.mark = kxlist_mark;
	kxlist_extension.clone = kxlist_clone;
	kxlist_extension.clean = kxlist_clean;

}

KxList *
kxlist_new_prototype(KxCore *core) 
{
	KxList *self = kxcore_clone_base_object(core);
	self->extension = &kxlist_extension;
	self->data.ptr = list_new();

	kxlist_add_methods_table(self);
	return self;
}



static void 
kxlist_free(KxList *self) 
{
	List *list = self->data.ptr;
	int t;
	for (t=0;t<list->size;t++) {
		REF_REMOVE((KxObject*)(list->items[t]));
	}
	list_free(self->data.ptr);
}


static void
kxlist_clean(KxList *self) 
{
	List *list = self->data.ptr;
	int t;
	for (t=0;t<list->size;t++) {
		REF_REMOVE((KxObject*)(list->items[t]));
	}
	list_set_size(self->data.ptr,0);

}

static KxObject *
kxlist_copy(KxList *self, KxMessage *message)
{
	KxObject *copy = kxobject_raw_copy(self);
	List *list = (List *) self->data.ptr;
	list_foreach(list, kxobject_ref_add);
	copy->data.ptr = list_copy(list);
	return copy;
}

static KxObject *
kxlist_msg_clean(KxList *self, KxMessage *message) 
{
	kxlist_clean(self);
	KXRETURN(self);
}

static void
kxlist_mark(KxList *self)
{
	list_foreach(self->data.ptr, (ListForeachFcn*)kxobject_mark);
}


/**
 *  Convert KxList to List which contains cstrings, 
 *  If item in KxList isn't KxString, item is skipped
 */ 
List *
kxlist_to_list_of_cstrings(KxList *self)
{
	List *list = KXLIST_DATA(self);
	List *out = list_new_size(list->size);

	int t;
	int count = 0;
	for (t=0;t<list->size;t++) {
		KxObject *obj = list->items[t];
		if (!IS_KXSTRING(obj)) {
			continue;
		}
		out->items[count] = KXSTRING_TO_CSTRING(obj);
		count++;
	}
	if (count != list->size)
		list_set_size(out,count);
	return out;
}

KxList *
kxlist_new_from_list_of_cstrings(KxCore *core, List *list)
{
	List *out = list_new_size(list->size);

	int t;
	for (t=0;t<list->size;t++) {
		char *str = list->items[t];
		out->items[t] = kxstring_new_with(core,str);
	}
	return kxlist_new_with(core,out);
}


static KxObject *
kxlist_add(KxList *self, KxMessage *message)
{
	KxObject *obj = message->params[0];
	REF_ADD(obj);
	list_append(self->data.ptr, obj);
	KXRETURN(self);
}

/*KXdoc at: index 
  Returns object in list at index-th position.
*/
static KxObject *
kxlist_at(KxList *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	List *list = self->data.ptr;
	
	if (param < 0) {
		param += list->size;
	}

	if (param<0 || param >= list->size) {
		KxException *exp = kxexception_new_with_message(KXCORE,KXSTRING("Index out of range"));
		KXTHROW(exp);
	}

	KxObject *obj = list->items[param];
	KXRETURN(obj);
}

static KxObject *
kxlist_at_put(KxList *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	List *list = self->data.ptr;
	
	if (param < 0) {
		param += list->size;
	}

	if (param<0 || param >= list->size) {
		KxException *exp = kxexception_new_with_message(KXCORE,KXSTRING("Index out of range"));
		KXTHROW(exp);
	}

	KxObject *obj = list->items[param];
	list->items[param] = message->params[1];
	REF_ADD(message->params[1]);
	REF_REMOVE(obj);

	KXRETURN(self);
}

static KxObject *
kxlist_foreach(KxList *self, KxMessage *message)
{
	KxObject *block = message->params[0];
	List *list =  self->data.ptr;

	if (list->size == 0) {
		KXRETURN(KXCORE->object_nil);	
	}

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;


	int t;
	for (t=0;t<list->size-1;t++) {
		msg.params[0] = list->items[t];
		KxObject * obj = kxobject_evaluate_block(block,&msg);
		KXCHECK(obj);
		REF_REMOVE(obj);
	}
	msg.params[0] = list->items[t];
	return kxobject_evaluate_block(block,&msg);
}

static KxObject *
kxlist_reverse_foreach(KxList *self, KxMessage *message)
{
	KxObject *block = message->params[0];
	List *list =  self->data.ptr;

	if (list->size == 0) {
		KXRETURN(KXCORE->object_nil);	
	}

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;


	int t;
	for (t=list->size-1;t>=1;t--) {
		msg.params[0] = list->items[t];
		KxObject * obj = kxobject_evaluate_block(block,&msg);
		KXCHECK(obj);
		REF_REMOVE(obj);
	}
	msg.params[0] = list->items[0];
	return kxobject_evaluate_block(block,&msg);
}

static KxObject *
kxlist_size(KxList *self, KxMessage *message)
{
	List *list =  self->data.ptr;
	return KXINTEGER(list->size);
}

static KxObject *
kxlist_set_size(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	int size = list->size;
	KXPARAM_TO_LONG(param,0);
	if (param < 0) {
		KXTHROW_EXCEPTION("Size of list must be positive integer");
	}

	if (size > param) {
		int t;
		for (t=0;t<size;t++) {
			REF_REMOVE((KxObject*) list->items[t]);
		}
	}
	list_set_size(list,param);

	if (size < param) {
		int t;
		for (t=0;t<param;t++) {
			list->items[t] = KXCORE->object_nil;
			REF_ADD(KXCORE->object_nil);
		}

	}
	KXRETURN(self);
}

static KxObject *
kxlist_remove_at(KxList *self, KxMessage *message)
{
	List *list =  self->data.ptr;
	int size = list->size;
	KXPARAM_TO_LONG(param, 0);

	if (param < 0)
		param += size;

	if (param<0 || param >= size) {
		KxException *exp = kxexception_new_with_message(KXCORE,KXSTRING("Index out of range"));
		KXTHROW(exp);
	}
	REF_REMOVE((KxObject*)list->items[param]);
	list_remove(list, param);

	KXRETURN(self);
}

static KxObject *
kxlist_join(KxList *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (!IS_KXLIST(param)) {
		KxException *excp = kxexception_new_with_text(KXCORE,"List expected");
		KXTHROW(excp);
	}


	List *list = self->data.ptr;
	List *list2 = param->data.ptr;

	list_foreach(list2, kxobject_ref_add);

	list_join(list, list2);

	
	KXRETURN(self);
}



KxList * 
kxlist_clone(KxList *self) 
{
	KxObject *clone = kxobject_raw_clone(self);

	List *list = self->data.ptr;
	List *out = list_copy(list);
	int t;
	for (t=0;t<list->size;t++) {
		REF_ADD((KxObject*)out->items[t]);
	}
	clone->data.ptr = out;
	return clone;
}

 KxList * 
kxlist_new_with(KxCore *core, List *list) 
{
	KxObject *clone = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_LIST));
	clone->data.ptr = list;
	return clone;
}

static KxObject *
kxlist_pop(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	if (list->size == 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"List is empty");
		KXTHROW(excp);
	}
	KxObject *obj = ((KxObject*)list->items[list->size-1]);
	list_pop(list);
	return obj;
}

static KxObject *
kxlist_last(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	if (list->size == 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"List is empty");
		KXTHROW(excp);
	}
	KXRETURN((KxObject*)list->items[list->size-1]);
}

static KxObject *
kxlist_first(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	if (list->size == 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"List is empty");
		KXTHROW(excp);
	}
	KXRETURN((KxObject*)list->items[0]);
}


// Quick sort - recursive function,  1 - ok, 0 - non ok KxReturn from compare_block eval
static int
kxlist_sort_helper(KxObject **items, int begin, int end, KxObject *compare_block, KxMessage *msg)
{
	KxObject *pivot = items[begin];
	int begin_hold = begin;
	int end_hold = end;
	KxObject *obj_true = KXCORE_FROM(pivot)->object_true;
	while (begin < end) {
	/*	while ((numbers[begin] >= pivot) && (begin < end))
			end--;*/
		msg->params[1] = pivot;
		while(begin < end) {
			msg->params[0] = items[end];
			KxObject *ret = kxobject_evaluate_block(compare_block, msg);
			if (ret == NULL)
				return 0;
			REF_REMOVE(ret);
			if (ret == obj_true)
				break;
			end--;
		}
    	if (begin != end)
	    {
	      items[begin] = items[end];
	      begin++;
	    }

	/*    while ((pivot >= numbers[begin]) && (begin < end))
		      begin++;*/

		msg->params[0] = pivot;
		while(begin < end) {
			msg->params[1] = items[begin];
			KxObject *ret = kxobject_evaluate_block(compare_block, msg);
			if (ret == NULL)
				return 0;
			REF_REMOVE(ret);
			if (ret == obj_true)
				break;
			begin++;
		}


	    if (begin != end)
	    {
	      items[end] = items[begin];
	      end--;
	    }
 	}
	items[begin] = pivot;
	if (begin_hold < begin)
		if (!kxlist_sort_helper(items, begin_hold, begin-1, compare_block, msg))
			return 0;
	if (end_hold > begin)
	    if (!kxlist_sort_helper(items, begin+1, end_hold, compare_block, msg))
			return 0;
	
	return 1;
}



static KxObject *
kxlist_sort(KxList *self, KxMessage *message) 
{
	KxMessage msg;
	msg.params_count = 2;
	List *list = self->data.ptr;
	if (list->size == 0) {
		KXRETURN(self);
	}
    if (!kxlist_sort_helper((KxObject **) list->items, 0, list->size-1, message->params[0], &msg))
		return NULL;
	KXRETURN(self);
}

static KxObject *
kxlist_insert(KxList *self, KxMessage *message) 
{
	KxObject *obj = message->params[0];
	REF_ADD(obj);
	list_insert_at(self->data.ptr, obj,0);
	KXRETURN(self);
}

// index = 0..size-1
static KxObject *
kxlist_insert_at(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	KXPARAM_TO_LONG(index,1);
	if (index < 0 || index > list->size) {
		KXTHROW_EXCEPTION("Index out of range");
	}
	KxObject *obj = message->params[0];
	REF_ADD(obj);
	list_insert_at(list, obj,index);
	KXRETURN(self);
}


static KxObject *
kxlist_rest(KxList *self, KxMessage *message) 
{
	List *list = self->data.ptr;
	
	int size = list->size;
	if (size > 0) 
		size--;
	
	List *new = list_new_size(size);
	
	int t;
	for (t=0; t<size; t++) {
		KxObject *obj = (KxObject *) list->items[t+1];
		new->items[t] = obj;
		REF_ADD(obj);
	}

	return KXLIST(new);
}

static KxObject *
kxlist_eq(KxList *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (IS_KXLIST(param)) {
		List *list = self->data.ptr;
		List *list2 = param->data.ptr;
		
		int ls = list->size;

		if (list2->size != ls)
			KXRETURN(KXCORE->object_false);

		KxMessage msg;
		int t;
		for (t=0;t<ls;t++) {
			kxmessage_init(&msg, list->items[t], 1, KXCORE->dictionary[KXDICT_EQ]);
			msg.params[0] = list2->items[t];
			REF_ADD(msg.params[0]);
			KxObject *obj = kxmessage_send(&msg);
			KXCHECK(obj);
			REF_REMOVE(obj);
			if (obj == KXCORE->object_false) {
				KXRETURN(KXCORE->object_false);
			}
		}
		KXRETURN(KXCORE->object_true);

	} else 
		KXRETURN(KXCORE->object_false);


}

static KxObject *
kxlist_as_string(KxList *self, KxMessage *message)
{
	List *list = self->data.ptr;
	char *strings[list->size];
	KxObject *tmp[list->size];

	int len = 0;
	int t;
	for (t=0;t<list->size;t++) {
		KxObject *obj = (KxObject *) list->items[t];
		if (IS_KXSTRING(obj)) {
			strings[t] = KXSTRING_VALUE(obj);
			tmp[t] = NULL;
		} else {
			KxObject *str = kxobject_send_unary_message(obj, KXCORE->dictionary[KXDICT_AS_STRING]);

			if (str == NULL) {
				// Send failed, free all objects and return NULL
				int s;
				for (s=0;s<t;s++) {
					if (tmp[s])
						REF_REMOVE(tmp[s]);
				}
				return NULL;
			}
			tmp[t] = str;
			if (IS_KXSTRING(str)) {
				strings[t] = KXSTRING_VALUE(str);
			} else {
				strings[t] = "";
			}
		}
		len += strlen(strings[t]);
	};

	char *string = kxmalloc(len+1);
	char *pos = string;
	ALLOCTEST(string);
	for (t=0;t<list->size;t++) {
		char *src = strings[t];
		while( *src != 0 ) {
			*pos = *src;
			++src;
			++pos;
		}
	}
	*pos = 0;
	for (t=0;t<list->size;t++) {
		if (tmp[t])
			REF_REMOVE(tmp[t]);
	}
	return kxstring_from_cstring(KXCORE,string);

}

static void
kxlist_add_methods_table(KxList *self) 
{
	KxMethodTable table[] = {
		{"add:", 1, kxlist_add },
		{"at:", 1, kxlist_at },
		{"at:put:", 2, kxlist_at_put },
		{"insert:", 1, kxlist_insert },
		{"insert:at:", 2, kxlist_insert_at },
		{"==", 1, kxlist_eq },
		{"removeAt:",1, kxlist_remove_at},
		{"size",0, kxlist_size},
		{"foreach:", 1, kxlist_foreach },
		{"asString",0, kxlist_as_string},
		{"copy",0, kxlist_copy},
		{"size:",1, kxlist_set_size},
		{"reverseForeach:", 1, kxlist_reverse_foreach },
		{"join:", 1, kxlist_join },
		{"pop", 0, kxlist_pop },
		{"last", 0, kxlist_last },
		{"first", 0, kxlist_first },
		{"sort:", 1, kxlist_sort },
		{"rest", 0, kxlist_rest },
		{"clean", 0, kxlist_msg_clean },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);

}
