#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkcontainer.h"

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

KxObjectExtension kxgtkcontainer_extension;

static void kxgtkcontainer_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkcontainer_extension_init() {
	kxobjectext_init(&kxgtkcontainer_extension);
	kxgtkcontainer_extension.type_name = "GtkContainer";
	kxgtkcontainer_extension.parent = &kxgtkwidget_extension;
	kxgtkcontainer_extension.free = kxgtkcontainer_free;
//%init_extension%
}


static void
kxgtkcontainer_add_method_table(KxGtkContainer *self);

KxObject *
kxgtkcontainer_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkcontainer_extension;

	
	kxgtkcontainer_add_method_table(self);

	
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkcontainer_from(KxCore *core, GtkContainer* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkcontainer_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
kxgtkcontainer_add_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_container_add(self->data.ptr,param0);
	KXRETURN(self);
}
static KxObject *
kxgtkcontainer_remove_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_container_remove(self->data.ptr,param0);
	KXRETURN(self);
}



static void
kxgtkcontainer_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"add:",1,kxgtkcontainer_add_},
		{"remove:",1,kxgtkcontainer_remove_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
