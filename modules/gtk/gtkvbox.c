#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkvbox.h"

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

KxObjectExtension kxgtkvbox_extension;

static void kxgtkvbox_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkvbox_extension_init() {
	kxobjectext_init(&kxgtkvbox_extension);
	kxgtkvbox_extension.type_name = "GtkVBox";
	kxgtkvbox_extension.parent = &kxgtkbox_extension;
	kxgtkvbox_extension.free = kxgtkvbox_free;
//%init_extension%
}


static void
kxgtkvbox_add_method_table(KxGtkVBox *self);

KxObject *
kxgtkvbox_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkvbox_extension;

	
	kxgtkvbox_add_method_table(self);

	
	self->data.ptr = gtk_vbox_new(TRUE,0);
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkvbox_from(KxCore *core, GtkVBox* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkvbox_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
gtk_vbox_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_vbox_new(TRUE,0);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkvbox_cloneHomogenous_spacing_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	KXPARAM_TO_INT(param1,1);
	return KXGTKHBOX((GtkHBox*)gtk_vbox_new(param0,param1));
}



static void
kxgtkvbox_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_vbox_clone},
		{"cloneHomogenous:spacing:",2,kxgtkvbox_cloneHomogenous_spacing_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
