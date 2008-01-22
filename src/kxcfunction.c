/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "kxcfunction.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxexception.h"

KxObjectExtension kxcfunction_extension;
static KxObject * kxcfunction_activate(KxCFunction *self, KxObject *target, KxMessage *message);
static void kxcfunction_free(KxCFunction *self);

void 
kxcfunction_init_extenstion()
{
	kxobjectext_init(&kxcfunction_extension);
	kxcfunction_extension.type_name = "CFunction";
	kxcfunction_extension.activate = kxcfunction_activate;
	kxcfunction_extension.free = kxcfunction_free;
}

static void
kxcfunction_free(KxCFunction *self) 
{
	KxCFunctionData *data = self->data.ptr;
	if (data->objects) {
		KxSymbol **obj = data->objects;
		while (*obj) {
			REF_REMOVE(*obj);
			obj++;
		}
		kxfree(data->objects);
	}
	kxfree(data);
}

KxObject *
kxcfunction_returnself(KxObject *self, KxMessage *message) 
{
	REF_ADD(self);
	return self;
}


KxObject * 
kxcfunction_new_prototype(KxCore *core)
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxcfunction_extension;
	
	KxCFunctionData *data = kxmalloc(sizeof(KxCFunctionData));
	ALLOCTEST(data);

	data->type_extension = NULL;
	data->params_count = 0;
	data->cfunction = kxcfunction_returnself;
	data->objects = NULL;

	object->data.ptr = data;


	return object;
}


KxCFunction * 
kxcfuntion_clone_with(KxCFunction *self, KxObjectExtension *type_ext, int params_count, KxExternFunction *cfunction) 
{
	KxCFunction *child = kxobject_raw_clone(self);
	
	KxCFunctionData *data = kxmalloc(sizeof(KxCFunctionData));
	ALLOCTEST(data);

	data->type_extension = type_ext;
	data->params_count = params_count;
	data->cfunction = cfunction;
	data->objects = NULL;

	child->data.ptr = data;
	return child;
}


static KxObject *
kxcfunction_activate(KxCFunction *self, KxObject *target, KxMessage *message) 
{
	KxCFunctionData *data = self->data.ptr;
	
	if (data->type_extension && data->type_extension != target->extension) {
		KxException *excp = kxexception_new_with_text(KXCORE,"CFunction type error: %s expected\n",
			data->type_extension->type_name);
		KXTHROW(excp);
	}

	if (data->params_count != message->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"CFunction type error: invalid count of parameters %i (%i expected)\n",
			message->params_count, data->params_count);
		KXTHROW(excp);
	}

	KxStack *stack = KXSTACK;
	KxCFunction *old = stack->running_cfunction;
	stack->running_cfunction = self;
	KxObject *obj = data->cfunction(target, message);
	stack->running_cfunction = old;
	return obj;
}

/** Creates symbol frame with one symbol */
void 
kxcfunction_objects_set_one(KxCFunction *self, KxObject *object)
{
	KxCFunctionData *data = self->data.ptr;
	
	KxSymbol **objs = kxmalloc(sizeof(KxSymbol *) * 2);
	ALLOCTEST(objs);

	data->objects = objs;
	
	REF_ADD(object);
	objs[0] = object;
	objs[1] = NULL;
}


/** Creates symbol frame from list of symbols */
void 
kxcfunction_objects_set_array(KxCFunction *self, KxObject **array, int size) 
{
	KxCFunctionData *data = self->data.ptr;
	
	KxSymbol **objs = kxmalloc(sizeof(KxSymbol *) * (size+1));
	ALLOCTEST(objs);

	data->objects = objs;
	
	objs[size] = NULL;
	
	int t;
	for (t=0;t<size;t++) {
		REF_ADD(array[t]);
		objs[t] = array[t];
	}

}
