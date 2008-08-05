/*
   kxclosure.c
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

#include "kxclosure.h"
#include "kxglobals.h"
#include "gtk_utils.h"

static void
catch_failed_call(KxStack *stack)
{
	KxReturn state = kxstack_get_return_state(stack);
	if (state == RET_THROW) {
		kxstack_dump_throw(stack);
	} else if (state == RET_EXIT) {
		printf("VM quit called in callback, this is not allowed\n");
	} else 	{
		printf("Internal error: thread_body: Invalid return");
		abort();
	}
}

static void
kxclosure_marshal(GClosure *closure,
		    GValue *return_value,
		    guint n_param_values,
		    const GValue *param_values,
		    gpointer invocation_hint,
		    gpointer marshal_data)
{
	// TODO: Threads support!
	
	KxClosure *kxclosure = (KxClosure*) closure;
	KxObject *self = kxclosure->callback;

	if (KXCORE_IS_BLOCKED(KXCORE)) {
		if (kx_verbose) {
			printf("KxGtkClosure blocked by core\n");
		} 
		return;
	}

	/*KxObject *return_obj = 
		kxobject_evaluate_block_simple(self);*/

	KxMessage message;
	message.params_count = n_param_values;
	int t;
	for (t = 0; t < n_param_values; t++) {
		GValue *value = (GValue *) &param_values[t];
	//	printf("b) %i %i %s\n",t,n_param_values, g_type_name(G_VALUE_TYPE(value)));
		message.params[t] = 
			kxgtk_kxobject_from_gvalue(KXCORE, &param_values[t]);
//		kxobject_dump(message.params[t]);
//		printf("end\n");

	}
	KxObject *return_obj = kxobject_evaluate_block(self, &message);
	for (t = 0; t < n_param_values; t++) {
	//	kxobject_dump(message.params[t]);
		REF_REMOVE(message.params[t]);
	}


	if (return_obj) {
		REF_REMOVE(return_obj);
	} else {
		catch_failed_call(KXSTACK);
	}
}

void
kxclosure_mark(KxClosure *closure) 
{
	if (closure->callback) {
		kxobject_mark(closure->callback);
	}
}

void
kxclosure_invalidate(gpointer data, GClosure *closure)
{
	KxClosure *kxclosure = (KxClosure *) closure;
	if (kxclosure->callback) {
		REF_REMOVE(kxclosure->callback);
		kxclosure->callback = NULL;
	}
}

KxClosure *
kxclosure_new(KxObject *callback)
{
	KxClosure * closure = 
		(KxClosure *) g_closure_new_simple(sizeof(KxClosure), NULL);
    g_closure_add_invalidate_notifier(
		(GClosure*) closure, NULL, kxclosure_invalidate);
	g_closure_add_finalize_notifier (
		(GClosure*) closure, NULL, kxclosure_invalidate);
    g_closure_set_marshal((GClosure*)closure, kxclosure_marshal);

	REF_ADD(callback);
	closure->callback = callback;
	
	return closure;
}
