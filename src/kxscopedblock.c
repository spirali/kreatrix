/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "kxscopedblock.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxlist.h"
#include "kxexception.h"
#include "kxstack.h"
#include "kxactivation.h"

KxObjectExtension kxscopedblock_extension;

static void kxscopedblock_free(KxScopedBlock *self);
static void kxscopedblock_mark(KxScopedBlock *self);
//static KxReturn kxscopedblock_activate(KxScopedBlock *self, KxObject *target, KxMessage *message);
static void kxscopedblock_clean(KxScopedBlock *self);


void 
kxscopedblock_init_extension()
{
	kxobjectext_init(&kxscopedblock_extension);
	kxscopedblock_extension.type_name = "ScopedBlock";
	kxscopedblock_extension.free = kxscopedblock_free;
	//kxscopedblock_extension.activate = kxscopedblock_activate;
	kxscopedblock_extension.mark = kxscopedblock_mark;
	kxscopedblock_extension.clean = kxscopedblock_clean;
}


KxScopedBlock*
kxscopedblock_clone_with(KxScopedBlock *self, KxCodeBlock *codeblock, struct KxActivation *scope)
{
	KxObject *object = kxobject_raw_clone(self);
	
	KxScopedBlockData *data = malloc(sizeof(KxScopedBlockData));
	ALLOCTEST(data);

	scope->ref_count++;
	data->scope = scope;
	
	REF_ADD(codeblock);
	data->codeblock = codeblock;
	object->data.ptr = data;
	return object;
}

KxScopedBlock *
kxscopedblock_new(KxCore *core, KxCodeBlock *codeblock, struct KxActivation *scope)
{
	KxObject *proto = kxcore_get_basic_prototype(core,KXPROTO_SCOPEDBLOCK);
	return kxscopedblock_clone_with(proto, codeblock,scope);
}

static void kxscopedblock_add_methods(KxObject *self);


KxObject *
kxscopedblock_new_prototype(KxCore *core) {
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxscopedblock_extension;
	kxscopedblock_add_methods(self);
	return self;
}


static void
kxscopedblock_free(KxScopedBlock *self) 
{
	KxScopedBlockData *data = self->data.ptr;

	if (!data)
		return;

	REF_REMOVE(data->codeblock);

	data->scope->ref_count--;
	if (data->scope->ref_count == 0)  {
		kxactivation_free(data->scope);
	}

	free(data);
}

static void
kxscopedblock_clean(KxScopedBlock *self)
{
	KxScopedBlockData *data = self->data.ptr;
	if (!data)
		return;
	REF_REMOVE(data->codeblock);

	data->scope->ref_count--;
	if (data->scope->ref_count == 0)  {
		kxactivation_free(data->scope);
	}
	free(data);
	self->data.ptr = NULL;
}

static void
kxscopedblock_mark(KxScopedBlock *self) 
{
	KxScopedBlockData *data = self->data.ptr;

	if (!data)
		return;

	kxactivation_mark(data->scope);
	kxobject_mark(data->codeblock);
}

KxObject *
kxscopedblock_run(KxScopedBlock *self, KxMessage *message) 
{

	kxstack_push_object(KXSTACK,self);

	KxScopedBlockData *data = self->data.ptr;
	message->slot_holder = data->scope->slot_holder_of_codeblock;
	KxObject * retobj =  kxcodeblock_run(data->codeblock,data->scope->locals, message, KXCB_RUN_SCOPED);
	kxstack_pop_object(KXSTACK);
	return retobj;
}

static KxObject *
kxscopedblock_catch_all(KxObject *self, KxMessage *message)
{
	KxStack *stack = KXSTACK;
/*	KxStackSave stacksave;
	kxstack_save(stack, &stacksave);*/

	KxObject * retobj = kxobject_evaluate_block_simple(self);

	if (retobj == NULL) {
		if (kxstack_get_return_state(stack) == RET_THROW)
		{
//			kxstack_resume(stack, &stacksave);

			KxObject *obj = kxstack_catch_thrown_object(stack);

			KxMessage msg;
			msg.params_count = 1;

			msg.params[0] = obj;

			msg.target = message->params[0];

			retobj = kxobject_evaluate_block(message->params[0],&msg);
			REF_REMOVE(obj);
		}
	}
	return retobj;
}

static KxObject *
kxscopedblock_catch(KxObject *self, KxMessage *message)
{
	KxStack *stack = KXSTACK;
/*	KxStackSave stacksave;
	kxstack_save(stack, &stacksave);*/

	KxObject *retobj = kxobject_evaluate_block_simple(self);
	if (retobj == NULL) {
		if (kxstack_get_return_state(stack) == RET_THROW)
		{
	//		kxstack_resume(stack, &stacksave);
			KxObject *obj = kxstack_get_thrown_object(stack);
			if (!kxobject_is_kind_of(obj,message->params[0])) {
				return NULL;
			}
		

			obj = kxstack_catch_thrown_object(stack);

			KxMessage msg;
			msg.message_name = NULL;
			msg.params_count = 1;
			msg.target = self;
			msg.params[0] = obj;
			retobj = kxobject_evaluate_block(message->params[1],&msg);
			REF_REMOVE(obj);
			
		}
	}
	return retobj;
}

static KxObject *
kxscopedblock_value_with_list(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (!IS_KXLIST(param)) {
		KxException *excp = kxexception_new_with_message(KXCORE,KXSTRING("Parametr must be list"));
		KXTHROW(excp);
	}
	List *list = param->data.ptr;
	KxMessage msg;
	msg.message_name = message->message_name;
	msg.params_count = list->size;
	int t;
	for (t=0;t<msg.params_count;t++) {
		msg.params[t] = list->items[t];
	}
	return kxscopedblock_run(self, &msg);
}

static KxObject *
kxscopedblock_while_true(KxObject *self, KxMessage *message)
{
	KxObject *retobj;
	do {
		retobj = kxscopedblock_run(self, message);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	} while(retobj == KXCORE->object_true);
	KXRETURN(self);
}

static KxObject *
kxscopedblock_while_false(KxObject *self, KxMessage *message)
{
	KxObject *retobj;
	do {
		retobj = kxscopedblock_run(self, message);
	//	REF_REMOVE(retobj);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	} while(retobj == KXCORE->object_false);
	KXRETURN(self);
}


static KxObject *
kxscopedblock_while_true_with(KxObject *self, KxMessage *message)
{

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 0;
	msg.target = self;

	while(1) {
		KxObject *retobj = kxscopedblock_run(self, &msg);

		KXCHECK(retobj);
		REF_REMOVE(retobj);

		if (retobj != KXCORE->object_true) {
			KXRETURN(self);
		}
		
		retobj = kxobject_evaluate_block(message->params[0],&msg);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	};
}

static KxObject *
kxscopedblock_while_false_with(KxObject *self, KxMessage *message)
{

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 0;
	msg.target = self;

	while(1) {
		KxObject * retobj = kxscopedblock_run(self, &msg);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
		
		if (retobj != KXCORE->object_false) {
			KXRETURN(self);
		}
		
		retobj = kxobject_evaluate_block(message->params[0],&msg);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	};
}

static KxObject *
kxscopedblock_loop(KxObject *self, KxMessage *message)
{
	do {
		KxObject * retobj = kxscopedblock_run(self, message);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	} while(1);
}


static KxObject *
kxscopedblock_ensure(KxObject *self, KxMessage *message)
{
	KxStack *stack = KXSTACK;
	KxObject * ret = kxobject_evaluate_block_simple(self);

	KxObject * robj = NULL;
	KxReturn rstate = RET_OK;
	if (ret == NULL) {
		rstate = kxstack_get_return_state(stack);
		robj = kxstack_get_and_reset_return_object(stack);
	}

	KxObject * ret2 = kxobject_evaluate_block_simple(message->params[0]);

	

	if (ret2 == NULL) {
		if (ret) {
			REF_REMOVE(ret);
		}
		if (robj) 
			REF_REMOVE(robj);
		return NULL;
	}

	REF_REMOVE(ret2);
	if (robj) {
		kxstack_set_return_state(stack, rstate);
		kxstack_set_return_object(stack, robj);
	}
	return ret;
}



static void
kxscopedblock_add_methods(KxObject *self) 
{
	KxMethodTable table[] = {
		{"catchAll:",1, kxscopedblock_catch_all },
		{"catch:do:",2, kxscopedblock_catch },
		{"value",0, kxscopedblock_run},
		{"value:",1, kxscopedblock_run},
		{"value:value:",2, kxscopedblock_run},
		{"value:value:value:",3, kxscopedblock_run},
		{"valueWithList:",1, kxscopedblock_value_with_list},
		{"whileTrue",0, kxscopedblock_while_true},
		{"whileFalse",0, kxscopedblock_while_false},
		{"whileTrue:",1, kxscopedblock_while_true_with},
		{"whileFalse:",1, kxscopedblock_while_false_with},
		{"loop",0, kxscopedblock_loop},
		{"ensure:",1,kxscopedblock_ensure},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}


