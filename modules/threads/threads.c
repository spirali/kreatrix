/*
   threads.c
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
#include <pthread.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sched.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxthread.h"
#include "kxmutex.h"
#include "kxcore.h"
#include "kxinteger.h"
#include "kxglobals.h"
#include "kxexception.h"

void static kxthreads_add_method_table(KxObject *self);


static void
gil_lock(KxCore *core) 
{
	pthread_mutex_lock(core->gil);
}

static void
gil_unlock(KxCore *core) 
{
	pthread_mutex_unlock(core->gil);
}


static void
gil_init(KxCore *core) 
{
	core->gil_lock = gil_lock;
	core->gil_unlock = gil_unlock;
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	ALLOCTEST(mutex);
	pthread_mutex_init(mutex, NULL);
	core->gil = mutex;
}

static void
gil_unload(KxCore *core) 
{
	//gil_unlock(core);
	core->gil_lock = NULL;
	core->gil_unlock = NULL;
	/*if (pthread_mutex_destroy(core->gil)) {
		printf("Internal error: gil_unload FAILED\n");
	}*/
	free(core->gil);
	core->gil = NULL;
}


KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	if (!kx_threads_support) {
		KXTHROW_EXCEPTION("VM is not compiled with threads support");
	}
	gil_init(KXCORE);
	gil_lock(KXCORE);

	kxthread_extension_init();
	kxmutex_extension_init();

	KxThread *kxthread_prototype = kxthread_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxthread_prototype);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Thread"),kxthread_prototype);
	
	KxMutex *kxmutex_prototype = kxmutex_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxmutex_prototype);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Mutex"),kxmutex_prototype);

	kxthreads_add_method_table(self);

	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxmutex_extension);
	kxcore_remove_prototype(KXCORE, &kxthread_extension);
	gil_unload(KXCORE);
}

static KxObject *
kxthreads_yield(KxObject *self, KxMessage *message) 
{
	KX_THREADS_BEGIN
	sched_yield();
	KX_THREADS_END

	KXRETURN(self);
}

static KxObject *
kxthreads_set_yield_interval(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_LONG(param,0);
	KXCORE->yield_interval = param;
	KXRETURN(self);
}

static KxObject *
kxthreads_yield_interval(KxObject *self, KxMessage *message) 
{
	return KXINTEGER(KXCORE->yield_interval);
}


static KxObject *
kxthreads_this_thread(KxObject *self, KxMessage *message) 
{
	KxStack *stack = KXCORE->stack;
	if (stack->thread) {
		KXRETURN(stack->thread);
	} else {
		KXRETURN(KXCORE->object_nil);
	}
}



void static
kxthreads_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"yield",0, kxthreads_yield },
		{"yieldInterval",0, kxthreads_yield_interval },
		{"yieldInterval:",1, kxthreads_set_yield_interval },
		{"thisThread",0, kxthreads_this_thread },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
