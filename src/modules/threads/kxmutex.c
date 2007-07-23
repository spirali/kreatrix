
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

#include "kxmutex.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxglobals.h"

KxObjectExtension kxmutex_extension;

KxObject *kxmutex_clone(KxMutex *self);

void static kxmutex_add_method_table(KxObject *self);

void kxmutex_free(KxMutex *self) {
	pthread_mutex_destroy(self->data.ptr);
	free(self->data.ptr);
}


void kxmutex_extension_init() {
	kxobjectext_init(&kxmutex_extension);
	kxmutex_extension.type_name = "Mutex";
	kxmutex_extension.free = kxmutex_free;
	kxmutex_extension.clone = kxmutex_clone;
}

void *
kxmutex_new_data() 
{
	void *data = malloc(sizeof(pthread_mutex_t));
	ALLOCTEST(data);
	pthread_mutex_init(data, NULL);
	return data;
}

KxObject *
kxmutex_clone(KxMutex *self)
{
	KxMutex *clone = kxobject_raw_clone(self);
	clone->data.ptr = kxmutex_new_data();
	return clone;
}

KxObject * 
kxmutex_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxmutex_extension;
	self->data.ptr = kxmutex_new_data();
	kxmutex_add_method_table(self);
	return self;
}

static KxObject *
kxmutex_protect(KxMutex *self, KxMessage *message) 
{
	KX_THREADS_BEGIN
	pthread_mutex_lock(self->data.ptr);
	KX_THREADS_END
	KxObject *obj = kxobject_evaluate_block_simple(message->params[0]);
	pthread_mutex_unlock(self->data.ptr);
	return obj;
}

static KxObject *
kxmutex_unlock(KxMutex *self, KxMessage *message) 
{
	pthread_mutex_unlock(self->data.ptr);
	KXRETURN(self);
}


static KxObject *
kxmutex_lock(KxMutex *self, KxMessage *message) 
{
	KX_THREADS_BEGIN
	pthread_mutex_lock(self->data.ptr);
	KX_THREADS_END
	KXRETURN(self);
}



void static
kxmutex_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"lock",0, kxmutex_lock },
		{"unlock",0, kxmutex_unlock },
		{"protect:",1, kxmutex_protect },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
