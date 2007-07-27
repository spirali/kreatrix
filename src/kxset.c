/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

/*KXobject Base Set
 [Data structures] 
*/

#include <stdlib.h>
#include <stdio.h>

#include "kxset.h"

#include "kxset_utils.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxlist.h"

KxObjectExtension kxset_extension;

static void kxset_free(KxSet *self);
static void kxset_mark(KxSet *self);
static KxSet * kxset_clone(KxSet *self);
static void kxset_clean(KxSet *self);


static void kxset_add_methods_table(KxSet *self);

void
kxset_init_extension() 
{
	kxobjectext_init(&kxset_extension);
	kxset_extension.type_name = "Set";
	kxset_extension.free = kxset_free;
	kxset_extension.mark = kxset_mark;
	kxset_extension.clone = kxset_clone;
	kxset_extension.clean  = kxset_clean;
}

KxSet *
kxset_new_prototype(KxCore *core) 
{
	KxSet *self = kxcore_clone_base_object(core);
	self->extension = &kxset_extension;
	self->data.ptr = kxbaseset_new();

	kxset_add_methods_table(self);
	return self;
}

static void 
kxset_free(KxSet *self) 
{
	KxBaseSet *set = self->data.ptr;
	kxbaseset_free(set);
}


static void
kxset_clean(KxSet *self) {
	KxBaseSet *set = self->data.ptr;
	kxbaseset_clean(set);
}

static void
kxset_mark(KxSet *self)
{
	kxbaseset_mark(self->data.ptr);
}

/*static KxObject *
kxset_eq(KxInteger *self, KxMessage *message) 
{
	KXRETURN_BOOLEAN(self == message->params[0]);
}*/



static KxObject *
kxset_add(KxSet *self, KxMessage *message)
{
	KxBaseSet *set = self->data.ptr;
	if (!kxbaseset_add(set, message->params[0]))
		return NULL;

/*	kxbaseset_dump(set);
	printf("+++%i+++\n", set->items_count);*/
	KXRETURN(self);
}


static KxObject *
kxset_foreach(KxSet *self, KxMessage *message)
{
	KxObject *block = message->params[0];
	KxBaseSet *set = self->data.ptr;

	if (set->items_count == 0) {
		KXRETURN(KXCORE->object_nil);	
	}

	KxMessage msg;
	//msg.stack = message->stack;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;

	int t;
	for (t=0;t<set->array_size;t++) {
		if (set->array[t]) {
			msg.params[0] = set->array[t];
			KxObject * obj = kxobject_evaluate_block(block,&msg);
			KXCHECK(obj);
			REF_REMOVE(obj);
		}
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxset_size(KxSet *self, KxMessage *message)
{
	KxBaseSet *set =  self->data.ptr;
	return KXINTEGER(set->items_count);
}

static KxSet * 
kxset_clone(KxSet *self) 
{
	KxObject *clone = kxobject_raw_clone(self);

	KxBaseSet *set = self->data.ptr;
	clone->data.ptr =  kxbaseset_copy(set);

	return clone;
}

static KxObject * 
kxset_contains(KxSet *self, KxMessage *message) 
{
	int r = kxbaseset_contains(self->data.ptr, message->params[0]);
	if (r == -1)
		return NULL;
	KXRETURN_BOOLEAN(r);
}

static KxObject * 
kxset_remove(KxSet *self, KxMessage *message) 
{
	int r = kxbaseset_remove(self->data.ptr, message->params[0]);
	if (r == -1)
		return NULL;

	if (r == 0) {
		KXTHROW_EXCEPTION("Element not found in set.");
	}
	KXRETURN(self);
}

static KxObject * 
kxset_remove_ifabsent(KxSet *self, KxMessage *message) 
{
	int r = kxbaseset_remove(self->data.ptr, message->params[0]);
	if (r == -1)
		return NULL;

	if (r == 0) {
		return kxobject_evaluate_block_simple(message->params[1]);
	}
	KXRETURN(self);
}


static KxObject * 
kxset_copy(KxSet *self, KxMessage *message) 
{
	KxObject *copy = kxobject_raw_copy(self);

	KxBaseSet *set = self->data.ptr;
	copy->data.ptr =  kxbaseset_copy(set);

	return copy;
}


static void
kxset_add_methods_table(KxSet *self) 
{
	KxMethodTable table[] = {
		{"add:", 1, kxset_add },
		{"size",0, kxset_size},
		{"remove:",1, kxset_remove},
		{"remove:ifAbsent:",2, kxset_remove_ifabsent},
		{"contains:", 1, kxset_contains },
		{"foreach:", 1, kxset_foreach },
		{"copy",0, kxset_copy},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);

}
