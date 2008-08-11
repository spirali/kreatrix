/*
   kxobject.c - Implementation of basic operations for KxObject
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

#include "kxobject.h"
#include "utils/utils.h"
#include "kxsymbol.h"
#include "kxstring.h"
#include "kxtypes.h"
#include "kxscopedblock.h"
#include "kxcharacter.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxglobals.h"
#include "kxobject_profile.h"

static KxParentSlot * 
kxparentslot_new(KxObject *parent) 
{
	KxParentSlot *newslot = kxmalloc(sizeof(KxParentSlot));
	ALLOCTEST(newslot);

	REF_ADD(parent);

	newslot->parent = parent;
	newslot->next = NULL;
	return newslot;
}

KxObject * 
kxobject_new(KxCore *core) 
{

	KxObject *object = kxcore_raw_object_get(core);

	#ifdef KX_MULTI_CORE
		object->core = self->core;
	#endif

	object->ref_count = 1;

	kxobject_slots_init(object);


	return object;
}

KxObject *
kxobject_new_from(KxObject *self) 
{
	KX_LOG_WRITE1("NO",
					self, 
					self->extension?self->extension->type_name:"");

	KxObject *object = kxobject_new(KXCORE);


	++(KXCORE->objects_count);
	object->extension = self->extension;

	object->profile = self->profile;

	KxObject *tmp = self->gc_next;
	self->gc_next = object;
	object->gc_prev = self;
	object->gc_next = tmp;
	tmp->gc_prev = object;

	return object;
}

void 
kxobject_ref_remove(void *self) 
{
	REF_REMOVE(((KxObject*)self));
}

void 
kxobject_ref_add(void *self) 
{
	REF_ADD(((KxObject*)self));
}



void
kxobject_free(KxObject *self) 
{
	KX_LOG_WRITE_ID_I("FO",self, kxobject_slots_count(self));

	/* When GC call this function, ref_count shouldn't be zero
	 * and when hashtable is unreferenced, another object can be deleted
	 * and change reference to this object and free would be called twice 
	 *
	 * ref_count = 0 prevent twice free this object
	 *
	 * ??? NOTE: Is this still necessary?? (sb)
	 */
	self->ref_count = 0; 
	
	

	// call finalize
	
	if (self->extension && self->extension->free) {
		self->extension->free(self);
	}	

	if (self->parent_slot.parent) {
		REF_REMOVE(self->parent_slot.parent);
	};

	KxParentSlot *pslot = self->parent_slot.next;
	while(pslot) {
		KxParentSlot *next = pslot->next;
		REF_REMOVE(pslot->parent);
		kxfree(pslot);
		pslot = next;
	}

	if (self->ptype == KXOBJECT_PROTOTYPE) {
		kxobject_profile_free(self->profile);
	}

	kxobject_slots_free(self);

	KxObject *next = self->gc_next;
	KxObject *prev = self->gc_prev;
	next->gc_prev = prev;
	prev->gc_next = next;
	KXCORE->objects_count--;
	kxcore_raw_object_return(KXCORE,self);
}

void 
kxobject_clean(KxObject *self) {
	if (self->extension && self->extension->clean)
		self->extension->clean(self);
	kxobject_slots_clean(self);
	kxobject_remove_all_parents(self);
}

KxObject *
kxobject_raw_clone(KxObject *self)
{
	
	KxObject *child = kxobject_new_from(self);
	
	//kxobject_add_parent(child, self);
	if (self->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_prototype(self);
	}

	REF_ADD(self);
	child->parent_slot.parent = self;

	return child;
}

void 
kxobject_dump(KxObject *self)
{
	if (self == NULL) {
		printf("<NULL>\n");
		return;
	}
	char *proto_tail = "";

	if (self->parent_slot.parent == KXCORE->base_object) {
			proto_tail = "(proto)";
	}

	if (IS_KXSYMBOL(self) || IS_KXSTRING(self)) {
		printf("(%p_%s:%i) {%p} %s %s\n", self, kxobject_raw_type_name(self), self->ref_count,self->data.ptr,(char*) self->data.ptr, proto_tail);
	} else if (IS_KXCHARACTER(self)) {
		printf("(%p_%s:%i) {%p} %c %s\n", self, kxobject_raw_type_name(self), self->ref_count,self->data.ptr, self->data.charval, proto_tail);
	} else if (IS_KXCODEBLOCK(self)) {
		KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
		if (data) {
			printf("(%p_%s:%i) {%p} %s:%i %s\n", self, kxobject_raw_type_name(self), self->ref_count, self->data.ptr,data->source_filename, data->message_linenumbers?data->message_linenumbers[0]:-1, proto_tail);
		} else {
			printf("(%p_%s:%i) {%p} %s\n", self, kxobject_raw_type_name(self), self->ref_count,self->data.ptr, proto_tail);
		}
	} else 
		printf("(%p_%s:%i) {%p} %s\n", self, kxobject_raw_type_name(self), self->ref_count,self->data.ptr, proto_tail);
}

void 
kxobject_remove_all_parents(KxObject *self)
{
	if (self->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_noninstance(self);
	}

	if (self->parent_slot.parent) {
		REF_REMOVE(self->parent_slot.parent);
		self->parent_slot.parent = NULL;
	};
	KxParentSlot *pslot = self->parent_slot.next;
	self->parent_slot.next = NULL;
	while(pslot) {
		KxParentSlot *next = pslot->next;
		REF_REMOVE(pslot->parent);
		kxfree(pslot);
		pslot = next;
	}
}

KxObject *
kxobject_send_message_init(KxObject *self) 
{
	KxMessage msg;
	msg.params_count = 0;
	msg.target = self;

	msg.message_name = KXCORE->dictionary[KXDICT_INIT];
	REF_ADD(msg.message_name);

	REF_ADD(msg.target);

	int slot_not_found;
	KxObject *obj = kxmessage_send_no_hook_and_exception(&msg, &slot_not_found);
	
	if (obj == NULL) {
		if (!slot_not_found) {
			REF_REMOVE(msg.target);
			return NULL;
		}
	} else {
		REF_REMOVE(obj);
	}
	return msg.target;


}

KxObject *
kxobject_clone(KxObject *self)
{
	

	KxObject *target;
	if (self->extension) {
		if (self->extension->clone) 
			target = self->extension->clone(self);
		else {
			KxObject *str = kxobject_type_name(self);
			KxException *excp = kxexception_new_with_text(KXCORE,
				"Object '%s' cannot be cloned", str->data.ptr);
			REF_REMOVE(str);
			KXTHROW(excp);
		}
	} else {
		 target = kxobject_raw_clone(self);
	}	
	return kxobject_send_message_init(target);
}

void
kxobject_copy_parents(KxObject *self, KxObject *copy) 
{
	if (self->parent_slot.parent) {
		REF_ADD(self->parent_slot.parent);
		copy->parent_slot.parent = self->parent_slot.parent;
	}


	KxParentSlot *ps = self->parent_slot.next;
	if (ps == NULL)
		return;

	KxParentSlot *slot = copy->parent_slot.next;
	KxParentSlot *cs = NULL;
	while(slot) {
		cs = slot;
		slot = slot->next;
	}

	if (cs == NULL) {
		cs = kxparentslot_new(ps->parent);
		copy->parent_slot.next = cs;
		ps = ps->next;
	}
	while(ps) {
		KxParentSlot *np = kxparentslot_new(ps->parent);
		cs->next = np;
		cs = np;
		ps = ps->next;
	}
}



KxObject *
kxobject_raw_copy(KxObject *self) 
{
	KxObject *copy = kxobject_new_from(self);
	kxobject_copy_parents(self, copy);
	kxobject_slots_copy(self, copy);
	return copy;
}


void
kxobject_set_parent(KxObject *self, KxObject *parent)
{
	if (self->parent_slot.parent) {
		kxobject_remove_all_parents(self);
	}
	self->parent_slot.parent = parent;
	REF_ADD(parent);

	if (parent->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_prototype(parent);
	}
}


void
kxobject_add_parent(KxObject *self, KxObject *parent) 
{
	if (parent->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_prototype(parent);
	}

	if (self->parent_slot.parent == NULL) {
		REF_ADD(parent);
		self->parent_slot.parent = parent;
		return;
	}
	KxParentSlot *pslot = &self->parent_slot;
	
	KxParentSlot *prev = NULL;
	while (pslot) {
		prev = pslot;
		pslot = pslot->next;
	}

	KxParentSlot *slot = kxparentslot_new(parent);

	prev->next =  slot;
}

void
kxobject_remove_parent(KxObject *self, KxObject *parent) 
{
	if (self->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_noninstance(self);
	}

	if (self->parent_slot.parent == parent) {
		REF_REMOVE(parent);
		if (self->parent_slot.next == NULL) {
			self->parent_slot.parent = NULL;
			return;
		}
		KxParentSlot *p = self->parent_slot.next;
		self->parent_slot.parent = p->parent;
		self->parent_slot.next = p->next;
		kxfree(p);
		return;
	}

	KxParentSlot *pslot = self->parent_slot.next;
	
	KxParentSlot *prev = &self->parent_slot;
	while (pslot) {
		if (pslot->parent == parent) {
			prev->next = pslot->next;
			REF_REMOVE(parent);
			kxfree(pslot);
			return;
		}
		prev = pslot;
		pslot = pslot->next;
	}

}


void kxobject_insert_parent(KxObject *self, KxObject *parent)
{
	if (parent->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_prototype(parent);
	}

	if (self->parent_slot.parent) {
		KxObject *obj = self->parent_slot.parent;
		KxParentSlot *pslot = kxparentslot_new(obj);
		REF_REMOVE(obj);
		pslot->next = self->parent_slot.next;
		self->parent_slot.next = pslot;
	}
	REF_ADD(parent);
	self->parent_slot.parent = parent;

}

void 
kxobject_set_slot(KxObject *self, KxSymbol *key, KxObject *value) 
{
	REF_ADD(value);

	KxSlot *slot = kxobject_slot_find(self, key);
	if (slot) {
		REF_REMOVE(slot->value);
		slot->value = value;
		return;
	}
	REF_ADD(key); // if slot not exist, reference to key must be added
	kxobject_slot_add(self,key,value,0);

}

void 
kxobject_set_slot_with_flags(KxObject *self, KxSymbol *key, KxObject *value, int flags) 
{
	REF_ADD(value);

	KxSlot *slot = kxobject_slot_find(self, key);
	if (slot) {
		REF_REMOVE(slot->value);
		slot->value = value;
		slot->flags = flags;
		return;
	}
	REF_ADD(key); // if slot not exist, reference to key must be added


	kxobject_slot_add(self,key,value,flags);

}


// Same as kxobject_set_slot, but adopt reference on key
void 
kxobject_set_slot_no_ref(KxObject *self, KxSymbol *key, KxObject *value) 
{
	REF_ADD(value);

	KxSlot *slot = kxobject_slot_find(self, key);
	if (slot) {
		REF_REMOVE(key);
		REF_REMOVE(slot->value);
		slot->value = value;
		return;
	}
	kxobject_slot_add(self,key,value,0);

}


// Same as kxobject_set_slot, but adopt references on value and key
void 
kxobject_set_slot_no_ref2(KxObject *self, KxSymbol *key, KxObject *value) 
{
	KxSlot *slot = kxobject_slot_find(self, key);
	if (slot) {
		REF_REMOVE(key);
		REF_REMOVE(slot->value);
		slot->value = value;
		return;
	}
	kxobject_slot_add(self,key,value,0);

}


int
kxobject_update_slot(KxObject *self, KxSymbol *key, KxObject *value)
{
	KxSlot *slot = kxobject_slot_find(self, key);

	if (slot) {
		REF_REMOVE(slot->value);
		REF_ADD(value);
		slot->value = value;
		return 1;
	}

	kxobject_recursive_mark_set(self);
	
	KxParentSlot *pslot = &self->parent_slot;
	
	if (pslot->parent) {
		 do {

			if (!(kxobject_recursive_mark_test(pslot->parent))) {
				if (kxobject_update_slot(pslot->parent, key,value)) {
					kxobject_recursive_mark_reset(self);
					return 1;
				}
			}
	
			pslot = pslot->next;
		} while (pslot);
	}

	kxobject_recursive_mark_reset(self);
	return 0;
}

int
kxobject_update_slot_flags(KxObject *self, KxSymbol *key, int flags)
{
	KxSlot *slot = kxobject_slot_find(self, key);

	if (slot) {
		slot->flags = flags;
		return 1;
	}

	kxobject_recursive_mark_set(self);
	
	KxParentSlot *pslot = &self->parent_slot;

	if (pslot->parent) {
		do {

			if (!(kxobject_recursive_mark_test(pslot->parent))) {
				if (kxobject_update_slot_flags(pslot->parent, key,flags)) {
					kxobject_recursive_mark_reset(self);
					return 1;
				}
			}

			pslot = pslot->next;
		} while (pslot);
	}

	kxobject_recursive_mark_reset(self);
	return 0;
}

/**
	Search slot in self-object, NOT in parents
*/
KxObject *
kxobject_get_slot(KxObject *self, KxSymbol *key)
{
	KxSlot *slot = kxobject_slot_find(self, key);
	return slot?slot->value:NULL;
}

KxObject *
kxobject_get_slot_with_flags(KxObject *self, KxSymbol *key, int *flags)
{
	KxSlot *slot = kxobject_slot_find(self, key);
	if (slot) {
		*flags = slot->flags;
		return slot->value;
	} else 
		return NULL;
}

/**
	Search slot in self-object, AND in parents
*/
KxObject *
kxobject_find_slot(KxObject *self, KxSymbol *key) 
{
	KxObject *obj;
	int flags;
	return kxobject_find_slot_and_holder(self, key, &obj, &flags);
}


static KxObject 
*kxobject_slot_deep_search(KxObject *self, KxSlot *slot)
{
	KxSlot *s = kxobject_slot_find(self, slot->key);
	if (s) {
		slot->flags = s->flags;
		slot->value = s->value;
		return self;
	}

	kxobject_recursive_mark_set(self);
	
	if (self->parent_slot.parent) {
		KxParentSlot *pslot = &self->parent_slot;
		do {

			if (!(kxobject_recursive_mark_test(pslot->parent))) {
				KxObject *object = kxobject_slot_deep_search(pslot->parent, slot);
				if (object) {
					kxobject_recursive_mark_reset(self);
					return object;
				}
			}

			pslot = pslot->next;
		} while(pslot);
	}

	kxobject_recursive_mark_reset(self);
	return NULL;
}

KxObject *
kxobject_find_slot_and_holder(KxObject *self, KxSymbol *key, KxObject **slot_holder, int *flags) 
{
	KxSlot slot;
	slot.key = key;
	*slot_holder = kxobject_slot_deep_search(self, &slot);
	if (*slot_holder) {
		*flags = slot.flags;
		return slot.value;
	}
	return NULL;
}


// Same as find_slot but not search slot in self
KxObject *
kxobject_find_slot_in_ancestors(KxObject *self, KxSymbol *key, KxObject **slot_holder, int *flags) 
{
	kxobject_recursive_mark_set(self);
	
	KxParentSlot *pslot = &self->parent_slot;

	if (pslot->parent) {
		do {

			if (!(kxobject_recursive_mark_test(pslot->parent))) {
				KxObject *object = kxobject_find_slot_and_holder(pslot->parent, key, slot_holder, flags);
				if (object) {
					kxobject_recursive_mark_reset(self);
					return object;
				}
			}

			pslot = pslot->next;
		} while(pslot);
	}

	kxobject_recursive_mark_reset(self);
	return NULL;
}


int
kxobject_is_kind_of(KxObject *self, KxObject *test) 
{
	
	if (self == test)
		return 1;

	kxobject_recursive_mark_set(self);
	
	KxParentSlot *pslot = &self->parent_slot;

	if (pslot->parent) {
		do {
			if (!(kxobject_recursive_mark_test(pslot->parent))) {
				if (kxobject_is_kind_of(pslot->parent, test)) {
					kxobject_recursive_mark_reset(self);
					return 1;
				}
			}
			pslot = pslot->next;
		} while(pslot);
	}

	kxobject_recursive_mark_reset(self);
	return 0;
}

void 
kxobject_set_slot_cfunction(KxObject *self, KxSymbol *key, int params_count, KxExternFunction *function) 
{
	KxObject *prototype = kxcore_get_basic_prototype(KXCORE,KXPROTO_CFUNCTION);
	KxCFunction *cfunction = kxcfuntion_clone_with(prototype, self->extension, params_count, function);
	kxobject_set_slot(self,key,cfunction);
	REF_REMOVE(cfunction);
}


void
kxobject_add_methods(KxObject *self, KxMethodTable *table)
{
	KxMethodTable *item = table;
	while(item->message) {
		KxSymbol *message = kxcore_get_symbol(KXCORE,item->message);
		kxobject_set_slot_cfunction(self,message, item->params_count, item->cfunction);
		item++;
		REF_REMOVE(message);
	}
}

char *
kxobject_raw_type_name(KxObject *self)
{
	if (self->extension == NULL) {
		return "Object";
	}
	if (self->extension->type_name == NULL) {
		return "<NULL>";
	}
	return self->extension->type_name;
}

KxObject *
kxobject_send_unary_message(KxObject *self, KxSymbol *message_name)
{
		KxMessage msg;
		kxmessage_init(&msg, self, 0, message_name);
		return kxmessage_send(&msg);
}


KxString *
kxobject_type_name(KxObject *self)
{
	KxObject * obj = kxobject_send_unary_message(self, KXCORE->dictionary[KXDICT_TYPE]);

	if (obj) {
		if (IS_KXSTRING(obj)) 
			return obj;
		REF_REMOVE(obj);
		return KXSTRING("(type_is_not_string)");
	}
	KxReturn ret = kxstack_get_return_state(KXSTACK);
	switch(ret) {
		case RET_THROW: {
			KxObject *obj = kxstack_catch_thrown_object(KXSTACK);
			REF_REMOVE(obj);
			return KXSTRING("(type call thrown exception)");
		}
		default:
			fprintf(stderr,"kxobject_type_name: unexpected return");
			exit(-1);
	}
}

KxObject * 
kxobject_activate(KxObject *self, KxObject *target, List *params_list)
{

	if (!self->extension || !self->extension->activate)
	{
		KXRETURN(self);
	}

	KxMessage message;
	message.message_name = NULL;

	if (params_list) {
		int t;

		message.params_count = params_list->size;
		for (t=0;t<params_list->size;t++) {
			message.params[t] = params_list->items[t];
		}
	} else 
		message.params_count = 0;


	KxObject * ret = self->extension->activate(self, target, &message);

	return ret;
}

KxObject * 
kxobject_activate_with_message(KxObject *self, KxMessage *message)
{
	if (!self->extension || !self->extension->activate)
	{
		KXRETURN(self);
	}

	KxObject * ret = self->extension->activate(self, message->target, message);

	return ret;
}



void 
kxobject_mark(KxObject *self) 
{
	if (kxobject_flag_test(self, KXOBJECT_FLAG_GC)) {
 		return;
	}

	kxobject_flag_set(self, KXOBJECT_FLAG_GC);
	
	if (self->parent_slot.parent && !kxobject_flag_test(self->parent_slot.parent, KXOBJECT_FLAG_GC))
	{
		kxobject_mark(self->parent_slot.parent);	
	}
	
	KxParentSlot *pslot = self->parent_slot.next;
	while(pslot) {
		if (!kxobject_flag_test(pslot->parent, KXOBJECT_FLAG_GC))
			kxobject_mark(pslot->parent);
		pslot = pslot->next;
	}

	if (self->extension && self->extension->mark) {
		self->extension->mark(self);
	}

	kxobject_slots_mark(self);
}

static KxObject * 
kxobject_send_value_message(KxObject *self, KxMessage *message)
{
	switch(message->params_count) {
		case 0: message->message_name = KXCORE->dictionary[KXDICT_VALUE]; break;
		case 1: message->message_name = KXCORE->dictionary[KXDICT_VALUE1]; break;
		case 2: message->message_name = KXCORE->dictionary[KXDICT_VALUE2]; break;
		case 3: message->message_name = KXCORE->dictionary[KXDICT_VALUE3]; break;
		default: message->message_name = KXCORE->dictionary[KXDICT_VALUE_WITH_LIST]; break;
	}
	REF_ADD(message->message_name);
	REF_ADD(message->target);
	int t;
	for (t=0;t<message->params_count;t++) {
		REF_ADD(message->params[t]);
	}
	return kxmessage_send(message);
}

void
kxobject_set_type(KxObject *self, char *type)
{
	KxString *str = KXSTRING(type);
	kxobject_set_slot(self,KXCORE->dictionary[KXDICT_TYPE],str);
	REF_REMOVE(str);
}

/**
	Message attrs: params_count, params,  and stack must be filled, 
	no reference modified, caller must protect reference
*/
KxObject *
kxobject_evaluate_block(KxObject *self, KxMessage *message) 
{
	
	message->message_name = NULL;
	if (IS_KXSCOPEDBLOCK(self)) {
		return kxscopedblock_run(self, message);
	}
	
	KxMessage msg;
	msg.target = self;
	msg.params_count = message->params_count;
	int t;
	for (t=0;t<msg.params_count;t++) {
		msg.params[t] = message->params[t];
	}
	return kxobject_send_value_message(self, &msg);
}

KxObject *
kxobject_evaluate_block_simple(KxObject *self) 
{
	KxMessage msg;
	msg.target = self;
	msg.message_name = NULL;
	msg.params_count = 0;
	if (IS_KXSCOPEDBLOCK(self)) {
		return kxscopedblock_run(self, &msg);
	}
	
	return kxobject_send_value_message(self, &msg);
}

KxObject *
kxobject_find_object_by_path(KxObject *self, char **path, int path_count) 
{
	KxObject *obj = self;
	int t;
	for (t=0;t<path_count;t++) {
	    KxSymbol *symbol = KXSYMBOL(path[t]);
		obj = kxobject_find_slot(obj,symbol);
		REF_REMOVE(symbol);
		if (obj == NULL)
			return NULL;
	}
	return obj;
}

KxObject *
kxobject_secured_clone_object_by_path(KxObject *self, char **path, int path_count) 
{
	KxObject *proto = kxobject_find_object_by_path(self, path, path_count);
	if (proto == NULL) {
		fprintf(stderr,"Fatal error: kxobject_secured_clone_object_by_path: proto not found\n");
		int t;
		for (t=0;t<path_count;t++) 
			fprintf(stderr,"Path: %s\n", path[t]);
		exit(-1);
	}
	KxObject *obj = kxobject_clone(proto);
	if (obj == NULL) {
		fprintf(stderr,"Fatal error: kxobject_secured_clone_object_by_path: proto cannot be cloned\n");
		int t;
		for (t=0;t<path_count;t++) 
			fprintf(stderr,"Path: %s\n", path[t]);
		exit(-1);
	}

	return obj;
}

KxObject *
kxobject_compare_exception(KxObject *self, KxObject *object)
{
	KxObject *obj1 = kxobject_type_name(self);
	KxObject *obj2 = kxobject_type_name(object);

	KxException *excp = kxexception_new_with_text(KXCORE,"%s is not comparable to %s",
		obj1->data.ptr, obj2->data.ptr);
	
	REF_REMOVE(obj1);
	REF_REMOVE(obj2);
	return excp;
}


/* Get hash of object, 
 * returns 1 when succed, 0 on error (return_state set in stack) */
int 
kxobject_get_hash(KxObject *self, unsigned long *hash)
{
	KxObject *ret = kxobject_send_unary_message(self, KXCORE->dictionary[KXDICT_HASH]);
	if (ret == NULL)
		return 0;
	if (IS_KXINTEGER(ret)) {
		*hash = KXINTEGER_VALUE(ret);
		REF_REMOVE(ret);
		return 1;
	}
	KxException *excp = kxexception_new_with_text(KXCORE,
		"message 'hash' returned '%s', but integer is expected",
		kxobject_type_name(ret));

	REF_REMOVE(ret);
	kxstack_throw_object(KXSTACK, excp);
	return 0;
}

// TODO: Change integer & string type error to this function
// TODO: Use TypeException instead Exception
KxObject *
kxobject_type_error(
	KxObject *self, KxObjectExtension *extension, int param_id)
{
	KxString *type_name = kxobject_type_name(self);

	char msg[250];

	snprintf(msg, 250,
		"'%s' is expected as %i. parameter, not '%s'", 
		extension->type_name, 
		param_id + 1,
		KXSTRING_VALUE(type_name));

	KxException *exception = kxcore_clone_registered_exception_text
		(KXCORE,"vm","TypeError",msg);
	
	REF_REMOVE(type_name);
	KXTHROW(exception);
}

KxObject *
kxobject_need_boolean(KxObject *self)
{
	KxString *type_name = kxobject_type_name(self);
	
	char msg[250];
	snprintf(msg, 250, 
		"Object true or false is expected as parameter, not '%s'", 
		KXSTRING_VALUE(type_name));
	
	KxException *exception = kxcore_clone_registered_exception_text
		(KXCORE,"vm","TypeError",msg);
	
	REF_REMOVE(type_name);
	KXTHROW(exception);
}

int
kxobject_check_type(KxObject *self, KxObjectExtension *extension)
{
	KxObjectExtension *ext = self->extension;
	while(ext) {
		if (ext == extension) 
			return 1;
		ext = ext->parent;
	};
	return 0;
}

void
kxobject_set_as_prototype(KxObject *self)
{
	self->ptype = KXOBJECT_PROTOTYPE;
	self->profile = kxobject_profile_new_for_child(self->profile, self);
}

void
kxobject_set_as_noninstance(KxObject *self)
{
	self->ptype = KXOBJECT_NONINSTANCE;
	self->profile = NULL;
}
