/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>


#include "kxboolean.h"
#include "kxobject.h"
#include "kxexception.h"

static void kxboolean_true_add_methods(KxObject *self);


KxObject *kxboolean_new_true(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	kxboolean_true_add_methods(self);
	return self;
}

/*static KxObject *
kxtrue_print(KxObject *self, KxMessage *message)
{
	printf("true");
	RETURN(self);
}*/


/*static KxObject *
kxtrue_iffalse(KxObject *self, KxMessage *message)
{
	KXRETURN(self);
}*/


static KxObject *
kxtrue_iftrue(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[0]);
}

static KxObject *
kxtrue_iftrue_iffalse(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[0]);
}

static KxObject *
kxtrue_iffalse_iftrue(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[1]);
}

// Replace with slot with "false", not cfunction
static KxObject *
kxtrue_not(KxObject *self, KxMessage *message)
{
	KXRETURN(KXCORE->object_false);
}

static KxObject *
kxtrue_eager_and(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (param == self) {
		KXRETURN(KXCORE->object_true);
	}
	if (param == KXCORE->object_false) {
		KXRETURN(param);
	}
	KxException *excp = kxexception_new_with_text(
		KXCORE,"Second parameter must be true or false, not '%s'",
		KXSYMBOL_AS_CSTRING(kxobject_type_name(param))
	);
	KXTHROW(excp);
}

static KxObject *
kxtrue_eager_or(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (param == self || param == KXCORE->object_false) {
		KXRETURN(KXCORE->object_true);
	}
	KxException *excp = kxexception_new_with_text(
		KXCORE,"Second parameter must be true or false, not '%s'",
		KXSYMBOL_AS_CSTRING(kxobject_type_name(param))
	);
	KXTHROW(excp);
}

static KxObject *
kxtrue_lazy_and(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];

	return kxobject_evaluate_block_simple(param);

}

static KxObject *
kxtrue_lazy_or(KxObject *self, KxMessage *message)
{
	//RETURN_TRUE;
	KXRETURN(KXCORE->object_true);
}


static void
kxboolean_true_add_methods(KxObject *self) 
{
	KxMethodTable table[] = {
		{"ifTrue:",1, kxtrue_iftrue },
		{"ifFalse:",1, kxcfunction_returnself },
		{"ifTrue:ifFalse:",2, kxtrue_iftrue_iffalse },
		{"ifFalse:ifTrue:",2, kxtrue_iffalse_iftrue },
		{"not",0, kxtrue_not },
		{"&&",1, kxtrue_eager_and },
		{"||",1, kxtrue_eager_or },
		{"and:",1, kxtrue_lazy_and },
		{"or:",1, kxtrue_lazy_or },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}



static void kxboolean_false_add_methods(KxObject *self);

KxObject *kxboolean_new_false(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	kxboolean_false_add_methods(self);
	return self;
}

/*static KxObject *
kxfalse_print(KxObject *self, KxMessage *message)
{
	printf("false");
	RETURN(self);
}*/


/*static KxObject *
kxfalse_iftrue(KxObject *self, KxMessage *message)
{
	KXRETURN(self);
}*/


static KxObject *
kxfalse_iffalse(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[0]);
}

static KxObject *
kxfalse_iftrue_iffalse(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[1]);
}

static KxObject *
kxfalse_iffalse_iftrue(KxObject *self, KxMessage *message)
{
	return kxobject_evaluate_block_simple(message->params[0]);
}


static KxObject *
kxfalse_not(KxObject *self, KxMessage *message)
{
	KXRETURN(KXCORE->object_true);
}

static KxObject *
kxfalse_or(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (param == self) {
		KXRETURN(KXCORE->object_false);
	}
	if (param == KXCORE->object_true) {
		KXRETURN(param);
	}
	KxException *excp = kxexception_new_with_text(
		KXCORE,"Second parameter must be true or false, not '%s'",
		KXSYMBOL_AS_CSTRING(kxobject_type_name(param))
	);
	KXTHROW(excp);
}

static KxObject *
kxfalse_and(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (param == self || param == KXCORE->object_true) {
		KXRETURN(KXCORE->object_false);
	}
	KxException *excp = kxexception_new_with_text(
		KXCORE,"Second parameter must be true or false, not '%s'",
		KXSYMBOL_AS_CSTRING(kxobject_type_name(param))
	);
	KXTHROW(excp);
}

static KxObject *
kxfalse_lazy_or(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];

	return kxobject_evaluate_block_simple(param);

}

static KxObject *
kxfalse_lazy_and(KxObject *self, KxMessage *message)
{
	KXRETURN(KXCORE->object_false);
}



static void
kxboolean_false_add_methods(KxObject *self) 
{
	KxMethodTable table[] = {
		{"ifTrue:",1, kxcfunction_returnself },
		{"ifFalse:",1, kxfalse_iffalse },
		{"ifTrue:ifFalse:",2, kxfalse_iftrue_iffalse },
		{"ifFalse:ifTrue:",2, kxfalse_iffalse_iftrue },
		{"not",0, kxfalse_not },
		{"&&",1, kxfalse_and },
		{"||",1, kxfalse_or },
		{"and:",1, kxfalse_lazy_and },
		{"or:",1, kxfalse_lazy_or },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}



