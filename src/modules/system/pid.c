
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



#include "pid.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxexception.h"

KxObjectExtension kxpid_extension;

KxObject *kxpid_clone(KxPid *self);

void static kxpid_add_method_table(KxObject *self);

void kxpid_free(KxPid *self) {
	// Prevent from freeing data
}

void kxpid_extension_init() {
	kxobjectext_init(&kxpid_extension);
	kxpid_extension.type_name = "Pid";
	kxpid_extension.free = kxpid_free;
	kxpid_extension.clone = kxpid_clone;
}

KxObject *
kxpid_clone(KxPid *self)
{
	KxPid *clone = kxobject_raw_clone(self);
	kxpid_set_pid(clone,KXPID_VALUE(self));
	return clone;
}

KxObject * 
kxpid_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxpid_extension;
	self->data.intval = 0;

	kxpid_add_method_table(self);
	return self;
}

KxObject *
kxpid_set_pid(KxPid *self, pid_t pid)
{
	self->data.intval = pid;
}

static KxObject *
kxpid_as_integer(KxObject *self, KxMessage *message) 
{
	return KXINTEGER(KXPID_VALUE(self));
	//KXRETURN(integer);
}

static KxObject *
kxpid_is_zero(KxObject *self, KxMessage *message) 
{
	pid_t val = KXPID_VALUE(self);
	KXRETURN_BOOLEAN(val == 0);
}

static KxObject *
kxpid_wait(KxObject *self, KxMessage *message)
{
	pid_t pid = KXPID_VALUE(self);
	int status;
	if (waitpid(pid, &status, 0) < 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"waitpid failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	return KXINTEGER(status);
}

static KxObject *
kxpid_async_wait(KxObject *self, KxMessage *message)
{
	pid_t pid = KXPID_VALUE(self);
	int status;
	int ret = waitpid(pid, &status, WNOHANG);
	if (ret < 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"waitpid failed: %s", strerror(errno));
		KXTHROW(excp);
	}

	if (ret == 0) {
	//	RETURN_NIL;
	}

	return KXINTEGER(status);
}

void static
kxpid_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"asInteger",0, kxpid_as_integer },
		{"isZero",0, kxpid_is_zero },
		{"wait",0, kxpid_wait },
		{"asyncWait",0, kxpid_wait },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
