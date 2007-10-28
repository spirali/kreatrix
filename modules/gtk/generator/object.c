#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "%name%.h"

%include%

KxObjectExtension kx%name%_extension;

static void kx%name%_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	%free%
	//kxgtk_remove_wrapper(self);
}

static void kx%name%_mark(KxObject *self)
{
	%mark%
}

%extra_code%

void kx%name%_extension_init() {
	kxobjectext_init(&kx%name%_extension);
	kx%name%_extension.type_name = "%Name%";
	kx%name%_extension.parent = %parent_extension%;
	kx%name%_extension.free = kx%name%_free;
	kx%name%_extension.mark = kx%name%_mark;
    %init_extension%
}


static void
kx%name%_add_method_table(Kx%Name% *self);

KxObject *
kx%name%_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kx%name%_extension;

	
	kx%name%_add_method_table(self);

	%init_prototype%

	return self;
}


KxObject *
kx%name%_from(KxCore *core, %ctype% data)
{
	%new_from%
/*
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kx%name%_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

%cfunctions%


static void
kx%name%_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
%cfunctions_table%
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
