#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkbutton.h"

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

KxObjectExtension kxgtkbutton_extension;

static void kxgtkbutton_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkbutton_extension_init() {
	kxobjectext_init(&kxgtkbutton_extension);
	kxgtkbutton_extension.type_name = "GtkButton";
	kxgtkbutton_extension.parent = &kxgtkbin_extension;
	kxgtkbutton_extension.free = kxgtkbutton_free;
//%init_extension%
}


static void
kxgtkbutton_add_method_table(KxGtkButton *self);

KxObject *
kxgtkbutton_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkbutton_extension;

	
	kxgtkbutton_add_method_table(self);

	
	self->data.ptr = gtk_button_new();
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkbutton_from(KxCore *core, GtkButton* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkbutton_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
gtk_button_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_button_new();
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkbutton_cloneWithLabel_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXGTKBUTTON((GtkButton*)gtk_button_new_with_label(param0));
}



static void
kxgtkbutton_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_button_clone},
		{"cloneWithLabel:",1,kxgtkbutton_cloneWithLabel_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
