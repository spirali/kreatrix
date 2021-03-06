/*
   kxmessage.c
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
#include "../kxconfig.h"

#ifdef KX_THREADS_SUPPORT
#include <sched.h>
#endif // KX_THREADS_SUPPORT

#include "kxmessage.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxexception.h"
#include "kxlist.h"

//static void kxmessage_dump(KxMessage *self);


void kxmessage_init(KxMessage *self, KxObject *target, int params_count, KxSymbol * message_name) {
	
	REF_ADD(message_name);
	self->message_name = message_name;

	REF_ADD(target);
	self->target = target;
	self->params_count = params_count;
}

KxList *
kxmessage_params_to_list(KxMessage *self) 
{
	int t;
	List *list = list_new_size(self->params_count);
	for (t=0;t<self->params_count;t++) {
		// no REF_ADD
		list->items[t] = self->params[t];
	}
	KxCore *core = KXCORE_FROM(self->target);
	return kxlist_new_with(core,list);
}

KxObject * 
kxmessage_send_to_message_hook(KxMessage *self, KxObject *message_hook)
{
	KxList *list = kxmessage_params_to_list(self);
	self->params_count = 2;
	self->params[0] = self->message_name;
	self->params[1] = list;

	KxCore *core = KXCORE_FROM(self->target);
	self->message_name = core->dictionary[KXDICT_DNU];
	
	KxObject * retobj;
	if (!message_hook->extension || !message_hook->extension->activate) {
			retobj = message_hook;
			REF_ADD(retobj);
	} else {
		KxStack *stack = KXSTACK_FROM(self->target);
		kxstack_push_message(stack, self);
		retobj =  message_hook->extension->activate(message_hook, self->target, self);
		kxstack_drop_message(stack);
	}

	REF_REMOVE(list);
	REF_REMOVE(self->params[0]);
	REF_REMOVE(self->target);
	return retobj;
}

static void
kxmessage_throw_slot_not_found(KxCore *core, char *slot_name)
{
		KxException *excp = kxcore_clone_registered_exception_text(core, "vm", "DoesNotUnderstand","Slot '%s' not found.", slot_name);
		kxstack_throw_object(core->stack, excp);
}

KxObject *
kxmessage_send(KxMessage *self) 
{
	int flags;
	KxObject *object = kxobject_find_slot_and_holder(self->target,self->message_name, &self->slot_holder, &flags);

	int params_count = self->params_count;
	
	KxObject * retobj;
	
	if (!object) {

		KxCore *core = KXCORE_FROM(self->target);
		KxObject *hook = kxobject_find_slot_and_holder(self->target,
			core->dictionary[KXDICT_DNU], &self->slot_holder, &flags);
		
		if (hook) {
			return kxmessage_send_to_message_hook(self, hook);
		}
		kxmessage_throw_slot_not_found(core, self->message_name->data.ptr);
		retobj = NULL;
	} else {
		if (!object->extension || !object->extension->activate || (flags & KXOBJECT_SLOTFLAG_FREEZE)) {
	
			retobj = object;
			REF_ADD(retobj);
		} else  {
			KxStack *stack = KXSTACK_FROM(self->target);
			kxstack_push_message(stack,self);

			#ifdef KX_THREADS_SUPPORT
			if (KXCORE_FROM(self->target)->gil) {
				KxCore *core = KXCORE_FROM(object);
				if (--core->yield_counter == 0) {
					core->yield_counter = core->yield_interval;
					KxStack *stack = core->stack;
					core->gil_unlock(core);
					sched_yield();
					core->gil_lock(core);
					kxcore_switch_to_stack(core, stack);
				}
			}
			#endif


			retobj =  object->extension->activate(object, 
				self->target, self);
			kxstack_drop_message(stack);
		}
	}
	REF_REMOVE(self->target);
	REF_REMOVE(self->message_name);
	int t;
	for (t=0;t<params_count;t++) {
		REF_REMOVE(self->params[t]);
	}
	return retobj;
}

KxObject *
kxmessage_resend(KxMessage *self, KxObject *start_of_lookup) 
{
	int flags;
	KxObject *object = kxobject_find_slot_in_ancestors(start_of_lookup,self->message_name, &self->slot_holder, &flags);

	int params_count = self->params_count;
	
	KxObject * retobj;
	
	if (!object) {

		KxCore *core = KXCORE_FROM(self->target);
		KxObject *hook = kxobject_find_slot_in_ancestors(start_of_lookup,
			core->dictionary[KXDICT_DNU], &self->slot_holder, &flags);
		
		if (hook) {
			return kxmessage_send_to_message_hook(self, hook);
		}
		kxmessage_throw_slot_not_found(core, (char*) self->message_name->data.ptr);
		
		retobj = NULL;
	} else {
		if (!object->extension || !object->extension->activate  || (flags & KXOBJECT_SLOTFLAG_FREEZE)) {
			retobj = object;
			REF_ADD(retobj);
		} else  {
			KxStack *stack = KXSTACK_FROM(self->target);
			kxstack_push_message(stack,self);
			retobj =  object->extension->activate(object, 
				self->target, self);
			kxstack_drop_message(stack);
		}
	}
	REF_REMOVE(self->target);
	REF_REMOVE(self->message_name);
	int t;
	for (t=0;t<params_count;t++) {
		REF_REMOVE(self->params[t]);
	}
	return retobj;
}



KxObject *
kxmessage_send_no_hook_and_exception(KxMessage *self, int *slot_not_found) 
{
	int flags;
	KxObject *object = kxobject_find_slot_and_holder(self->target,self->message_name, &self->slot_holder, &flags);

	int params_count = self->params_count;
	
	KxObject * retobj;
	
	if (!object) {
		*slot_not_found = 1;
		retobj = NULL;
	} else {
		*slot_not_found = 0;
		if (!object->extension || !object->extension->activate || (flags & KXOBJECT_SLOTFLAG_FREEZE)) {
			retobj = object;
			REF_ADD(retobj);
		} else  {
			KxStack *stack = KXSTACK_FROM(self->target);
			kxstack_push_message(stack,self);
			retobj =  object->extension->activate(object, 
				self->target, self);
			kxstack_drop_message(stack);
		}
	}
	REF_REMOVE(self->target);
	REF_REMOVE(self->message_name);
	int t;
	for (t=0;t<params_count;t++) {
		REF_REMOVE(self->params[t]);
	}
	return retobj;
}

KxMessage 
*kxmessage_new(KxSymbol *message_name, KxObject *target, KxObject *slot_holder) {
	KxMessage *self = kxcalloc(1,sizeof(KxMessage));
	ALLOCTEST(self);
	
	REF_ADD(message_name);
	self->params_count = 0;
	self->target = target;
	self->message_name = message_name;
	self->slot_holder = slot_holder;
	return self;
}

void
kxmessage_free(KxMessage *self) 
{
	REF_REMOVE(self->message_name);
	kxfree(self);
}

void
kxmessage_mark(KxMessage *self) 
{

	kxobject_mark(self->target);
	kxobject_mark(self->slot_holder);
	kxobject_mark(self->message_name);
	int t;
	for (t=0;t<self->params_count;t++) {
		kxobject_mark(self->params[t]);
	}
}

/*static void
kxmessage_dump(KxMessage *self)
{
	printf("--MESSAGE--\n");
	printf("message_name:"); kxobject_dump(self->message_name);
	printf("target:"); kxobject_dump(self->target);
	printf("slot_holder:"); kxobject_dump(self->slot_holder);
	int t;
	for (t=0;t<self->params_count;t++) {
		printf("param:"); kxobject_dump(self->params[t]);
	}
	printf("--END OF MESSAGE--\n");
}*/

void
kxmessage_dump(KxMessage *self)
{
	printf("%s target:", (char*)self->message_name->data.ptr);
	kxobject_dump(self->target);
}
