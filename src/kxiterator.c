
#include <stdlib.h>

#include <kxmessage.h>
#include <kxobject.h>
#include <kxcfunction.h>

#include "kxiterator.h"

KxObjectExtension kxiterator_extension;

static void
kxiterator_free(KxIterator *self)
{
	KxIteratorData *data = KXITERATOR_DATA(self);
	REF_REMOVE(data->object);
	free(data);
}

static void
kxiterator_mark(KxIterator *self)
{
	KxIteratorData *data = KXITERATOR_DATA(self);
	kxobject_mark(data->object);
}


void kxiterator_init_extension() {
	kxobjectext_init(&kxiterator_extension);
	kxiterator_extension.type_name = "Iterator";

	kxiterator_extension.free = kxiterator_free;
	kxiterator_extension.mark = kxiterator_mark;
}

KxObject *kxiterator_create(KxCore *core, KxIteratorData *data)
{
	KxObject *self = kxcore_raw_clone_basic_prototype(core, KXPROTO_ITERATOR);
	self->data.ptr = data;
	return self;
}

KxObject *
kxiterator_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxiterator_extension;

	KxIteratorDataInt *data = kxmalloc(sizeof(KxIteratorDataInt));
	ALLOCTEST(data);
	data->object = kxcore_get_basic_prototype(core, KXPROTO_LIST);
	REF_ADD(data->object);
	data->position = 0;
	self->data.ptr = data;
	
	return self;
}
