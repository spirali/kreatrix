#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkobject.h"

#include "gtkobject.h"
#include "gtkwidget.h"
#include "gtkcontainer.h"
#include "gtkbin.h"
#include "gtkwindow.h"
#include "gtkbutton.h"
#include "gtkcheckbutton.h"
#include "gtkbox.h"
#include "gtkvbox.h"
#include "gtkhbox.h"
#include "gtk_utils.h"

KxObjectExtension kxgtkobject_extension;

static void kxgtkobject_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkobject_extension_init() {
	kxobjectext_init(&kxgtkobject_extension);
	kxgtkobject_extension.type_name = "GtkObject";
	kxgtkobject_extension.parent = NULL;
	kxgtkobject_extension.free = kxgtkobject_free;
//%init_extension%
}


static void
kxgtkobject_add_method_table(KxGtkObject *self);

KxObject *
kxgtkobject_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkobject_extension;

	
	kxgtkobject_add_method_table(self);

	kxgtkobject_extra_init(self);
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkobject_from(KxCore *core, GtkObject* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkobject_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}




static void
kxgtkobject_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}