/*
   kxthread.c
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
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "kxthread.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxglobals.h"

KxObjectExtension kxthread_extension;

KxObject *kxthread_clone(KxThread *self);

void static kxthread_add_method_table(KxObject *self);

void kxthread_free(KxThread *self) {
	KxThreadData *data = KXTHREAD_DATA(self);

	if (data->return_value) {
		REF_REMOVE(data->return_value);
	}

	free(data);
}

void kxthread_mark(KxThread *self) {
	KxThreadData *data = KXTHREAD_DATA(self);
	if (data->return_value) {
		kxobject_mark(data->return_value);
	}
}

void kxthread_clean(KxThread *self) {
	KxThreadData *data = KXTHREAD_DATA(self);
	if (data->return_value) {
		REF_REMOVE(data->return_value);
		data->return_value = NULL;
	}
}



void kxthread_extension_init() {
	kxobjectext_init(&kxthread_extension);
	kxthread_extension.type_name = "Thread";
	kxthread_extension.free = kxthread_free;
	kxthread_extension.mark = kxthread_mark;
	kxthread_extension.clean = kxthread_clean;
	kxthread_extension.clone = kxthread_clone;
}

static KxThreadData * 
kxthread_new_data()
{
	KxThreadData *data = calloc(sizeof(KxThreadData), 1);
	ALLOCTEST(data);

	return data;
}

KxObject *
kxthread_clone(KxThread *self)
{
	KxThread *clone = kxobject_raw_clone(self);
	clone->data.ptr = kxthread_new_data();
	return clone;
}

KxObject * 
kxthread_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxthread_extension;
	self->data.ptr = kxthread_new_data();
	kxthread_add_method_table(self);
	return self;
}

static void *
thread_body(KxStack *stack)
{
	KxThread *self = stack->thread;
	KxCore *core = KXCORE;
	
	core->gil_lock(core);
	if (kx_verbose) 
		printf("thread start\n");

	kxcore_switch_to_stack(core,stack);

	KxMessage msg;
	KxSymbol *symbol = KXSYMBOL("run");
	kxmessage_init(&msg, self, 0, symbol);
	KxObject *ret = kxmessage_send(&msg);
	REF_REMOVE(symbol);
	if (ret == NULL) {
		KxReturn state = kxstack_get_return_state(stack);
		if (state == RET_THROW) {
			kxstack_dump_throw(stack);

			ret = core->object_nil;
			REF_ADD(ret);
		} else if (state == RET_EXIT) {
			ret = kxstack_get_and_reset_return_object(stack);
		} else 	{
			printf("Internal error: thread_body: Invalid return");
			abort();
		}

	}

	KxThreadData *data = KXTHREAD_DATA(self);
	if (data->state == KXTHREAD_STATE_RUNNING_DETACHED) {
		REF_REMOVE(ret);
		data->state = KXTHREAD_STATE_STOPPED;
	} else {
		data->return_value = ret;
		data->state = KXTHREAD_STATE_WAITING_FOR_JOIN;
	}
	
	kxcore_unregister_stack(core, stack);
	kxstack_free(stack);
	core->gil_unlock(core);
	if (kx_verbose) 
		printf("thread exit\n");

	pthread_exit(NULL);
}

kxthread_start_thread(KxThread *self, int detach) 
{
	KxThreadData *data = KXTHREAD_DATA(self);

	KxStack *stack = kxstack_new();
	kxstack_set_thread(stack, self);
	kxcore_register_stack(KXCORE,stack);


	pthread_attr_t a;
	pthread_attr_t *attr = NULL;
	
	if (detach) {
		attr = &a;
		pthread_attr_init(attr);
		pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
		data->state = KXTHREAD_STATE_RUNNING_DETACHED;
	} else {
		data->state = KXTHREAD_STATE_RUNNING_JOINABLE;
	}
	int r = pthread_create(&data->pthread, 
						   attr, 
						   (void*(*)(void*))thread_body, 
						   stack);

	if (attr) {
		pthread_attr_destroy(attr);
	}
}

static KxObject *
kxthread_start(KxThread *self, KxMessage *message) 
{
	KxThreadData *data = KXTHREAD_DATA(self);
	if (data->state != KXTHREAD_STATE_INIT && data->state != KXTHREAD_STATE_STOPPED) {
		KXTHROW_EXCEPTION("Thread is already running");
	}

	if (data->state == KXTHREAD_STATE_STOPPED) {
		
	}
	if (kxthread_start_thread(self,0)) {
		KxException *excp = 
			kxexception_new_with_text(KXCORE,"Creating new thread failed");
		KXTHROW(excp);
	}
	KXRETURN(self);
}

static KxObject *
kxthread_start_detached(KxThread *self, KxMessage *message) 
{
	KxThreadData *data = KXTHREAD_DATA(self);
	if (data->state != KXTHREAD_STATE_INIT && data->state != KXTHREAD_STATE_STOPPED) {
		KXTHROW_EXCEPTION("Thread is already running");
	}


	if (kxthread_start_thread(self,1)) {
		KxException *excp = 
			kxexception_new_with_text(KXCORE,"Creating new thread failed");
		KXTHROW(excp);
	}
	KXRETURN(self);
}


static KxObject *
kxthread_join(KxThread *self, KxMessage *message) 
{
	KxThreadData *data = KXTHREAD_DATA(self);

	if (data->state == KXTHREAD_STATE_RUNNING_DETACHED) {
		KXTHROW_EXCEPTION("Thread is detached");
	}

	if (data->state == KXTHREAD_STATE_INIT || data->state == KXTHREAD_STATE_STOPPED) {
		KXTHROW_EXCEPTION("Thread isn't running");
	}

	KX_THREADS_BEGIN
		void *status;
		pthread_join(data->pthread, &status);
	KX_THREADS_END
	data->state = KXTHREAD_STATE_STOPPED;
	KxObject *obj = data->return_value;
	if (obj) {
		data->return_value = NULL;
		return obj;
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxthread_is_alive(KxThread *self, KxMessage *message) 
{
	KxThreadData *data = KXTHREAD_DATA(self);
	KXRETURN_BOOLEAN(
		(data->state == KXTHREAD_STATE_RUNNING_JOINABLE || 
		data->state == KXTHREAD_STATE_RUNNING_DETACHED));
}



void static
kxthread_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"start",0, kxthread_start },
		{"startDetached",0, kxthread_start_detached },
		{"join",0, kxthread_join },
		{"isAlive",0, kxthread_is_alive },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
