/*
   kxstack.c
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

#include "kxstack.h"
#include "utils/utils.h"
#include "kxobject.h"
#include "kxsymbol.h"
#include "kxlist.h"
#include "kxmessage.h"
#include "kxactivation.h"
#include "kxstring.h"
#include "kxglobals.h"

KxStack * 
kxstack_new() 
{
	KxStack *stack = kxcalloc(1,sizeof(KxStack));
	ALLOCTEST(stack);

	return stack;
}

void
kxstack_free(KxStack *self) 
{
	
	#ifdef KX_THREADS_SUPPORT
	if (self->thread)
		REF_REMOVE(self->thread);
	#endif // KX_THREADS_SUPPORT

	if (self->return_object) {
		REF_REMOVE(self->return_object);
	}

	if (self->throw_trace)
		list_free(self->throw_trace);


	kxfree(self);


}

KxReturn
kxstack_push_object(KxStack *self, KxObject *object) 
{
	if (self->objects_count == KXSTACK_ACTIVATIONS_SIZE) {
		fprintf(stderr,"Fatal error: kxstack_push_object: Stack is full\n");
		exit(-1);
		// TODO: Exception
		return RET_THROW;
	}
	REF_ADD(object);
	self->objects[self->objects_count++] = object;
	return RET_OK;
}

void
kxstack_pop_object(KxStack *self) 
{
	/*if (!self->objects_count) {
		fprintf(stderr,"Fatal error: kxstack_pop_object: Stack is empty\n");
		exit(-1);
	}*/
	KxObject *object = self->objects[--self->objects_count];
	REF_REMOVE(object);
}

KxReturn
kxstack_push_activation(KxStack *self, struct KxActivation *activation) 
{
	if (self->activations_count == KXSTACK_ACTIVATIONS_SIZE) {
		
		if (kx_verbose) { 
			kxstack_dump_messages(self);	
		}

		fprintf(stderr,
			"Fatal error: kxstack_push_activation: Stack is full\n");

		
		abort();

		// TODO: Exception
		return RET_THROW;
	}
	self->activations[self->activations_count++] = activation;
	return RET_OK;
}

void
kxstack_pop_activation(KxStack *self) 
{
	/*if (!self->activations_count) {
		fprintf(stderr,"Fatal error: kxstack_pop_activation: Stack is empty\n");
		abort();
	}*/
   self->activations_count--;
}

void 
kxstack_throw_object(KxStack *self, KxObject *object)
{
	//REF_ADD(object);

	/*if (self->) {
		KxObject *obj = kxstack_catch_thrown_object(self);
		REF_REMOVE(obj);
	}*/
	kxstack_set_return_object(self, object);
	self->return_state = RET_THROW;
	self->throw_trace = list_new();
}

KxObject *
kxstack_get_thrown_object(KxStack *self)
{
	return self->return_object;
}


KxObject*
kxstack_catch_thrown_object(KxStack *self) {
	KxObject *obj = kxstack_get_and_reset_return_object(self);

	if (self->throw_trace) {
		if (obj) {
			KxCore *core = KXCORE_FROM(obj);

			kxcore_create_and_switch_to_stack(core);

			KxList *list = kxlist_new_from_list_of_cstrings(KXCORE_FROM(obj),self->throw_trace);
			KxSymbol *symbol = kxcore_get_symbol(core,"__trace");

			kxobject_set_slot_no_ref2(obj, symbol, list);

			kxcore_free_actual_and_switch_to_stack(core, self);
		}

		list_free_all(self->throw_trace);
		self->throw_trace = NULL;
	}

	return obj;
}

void
kxstack_mark(KxStack *self) 
{
	#ifdef KX_THREADS_SUPPORT
	if (self->thread)
		kxobject_mark(self->thread);
	#endif //KX_THREADS_SUPPORT

	//kxstack_dump_messages(self);
	if (self->return_object)
		kxobject_mark(self->return_object);

	int t;
	
	/* DEBUG*/
	/* for (t=0;t<self->activations_count;t++) {
	 	printf("dump -- ");
		kxobject_dump(self->activations[t]);
	}*/

	for (t=0;t<self->activations_count;t++) {
		kxactivation_mark(self->activations[t]);
	}

	for (t=0;t<self->objects_count;t++) {
		kxobject_mark(self->objects[t]);
	}


	for (t=0;t<self->messages_count;t++) {
		kxmessage_mark(self->messages[t]);
	}
}

void
kxstack_throw_trace( KxStack *self, 
	char *source_filename, 
	int lineno, 
	char *object_type, 
	KxSymbol *message_name )
{
	char *msg_name = message_name?KXSYMBOL_AS_CSTRING(message_name):"<unkwnon>";

	char tmp[250];
	char tmp2[250];
	snprintf(tmp2, 250,"%s:%i", source_filename, lineno);
	snprintf(tmp,250,"%-17s Target: %-14s Message: %s", 
		tmp2, object_type, msg_name);
	list_append(self->throw_trace,strdup(tmp));
}

void 
kxstack_print_trace(KxStack *self) 
{
	list_foreach(self->throw_trace, (ListForeachFcn*)puts);
}

void kxstack_push_message (KxStack *self, KxMessage *message)
{
	if (self->messages_count == KXSTACK_MESSAGES_SIZE) {
		fprintf(stderr,"Fatal error: kxstack_push_message: Stack is full\n");
		exit(-1);
	}
	self->messages[self->messages_count++] = message;
}

void kxstack_drop_message (KxStack *self) 
{
/*	if (!self->messages_count) {
		fprintf(stderr,"Fatal error: kxstack_pop_message: Stack is empty\n");
		exit(-1);
	}*/
	--self->messages_count;
}

KxReturn 
kxstack_get_return_state(KxStack *self) 
{
	return self->return_state;
}

void 
kxstack_set_return_state(KxStack *self, KxReturn state) 
{
	self->return_state = state;
}



KxObject *kxstack_get_and_reset_return_object(KxStack *self) 
{
	if (self->return_object) {
		KxObject * obj = self->return_object;
		self->return_object = NULL;
		return obj;
	} else 
		return NULL;
}

void kxstack_set_return_object(KxStack *self, KxObject *object)
{
	self->return_object = object;
}

void kxstack_dump_messages(KxStack *self) 
{
	int t;
	for (t=0;t<self->messages_count;t++) {
		kxmessage_dump(self->messages[t]);
	/*	if (self->messages[t]->message_name) {
			printf("MSG::%s\n", (char*)self->messages[t]->message_name->data.ptr);
		} else {
			printf("MSG::---\n");
		}*/
	}

}


void
kxstack_set_long_return_to_activation(KxStack *stack, struct KxActivation *activation)
{
	stack->long_return_to_activation = activation;
}

struct KxActivation *
kxstack_get_long_return_to_activation(KxStack *stack)
{
	return stack->long_return_to_activation;
}

void kxstack_dump_throw(KxStack *self) {
	if (kxstack_get_return_state(self) == RET_THROW) {
		printf("\nUncatched throw\nTrace:\n");
		kxstack_print_trace(self);
		KxObject *obj = kxstack_catch_thrown_object(self);
	
		if (!obj) {
			fprintf(stderr,"Internal error: kxstack_catch_thrown_object returned NULL\n");
				exit(-1);
		}

		KxString *str = kxobject_type_name(obj);
		printf("Thrown object: %s\n", (char*)str->data.ptr);
		REF_REMOVE(str);

		// TODO: Normal message call
		KxMessage msg;
		kxmessage_init(&msg, obj, 0, KXCORE_FROM(obj)->dictionary[KXDICT_MESSAGE]);
		REF_REMOVE(obj);
		
		KxObject *mesg_obj = kxmessage_send(&msg);

		if (mesg_obj) {
			if (IS_KXSTRING(mesg_obj))
				printf("Message: %s\n",(char*)mesg_obj->data.ptr);
			else
				printf("Message of thrown object hasn't returned String\n");

			REF_REMOVE(mesg_obj);
		} else {
			printf("No message slot in thrown object\n");
		}
	}	
}

#ifdef KX_THREADS_SUPPORT
void
kxstack_set_thread(KxStack *stack, KxObject *thread)
{
	stack->thread = thread;
	REF_ADD(thread);
}
#endif // KX_THREADS_SUPPORT
