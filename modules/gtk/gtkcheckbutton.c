#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkcheckbutton.h"

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

KxObjectExtension kxgtkcheckbutton_extension;

static void kxgtkcheckbutton_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkcheckbutton_extension_init() {
	kxobjectext_init(&kxgtkcheckbutton_extension);
	kxgtkcheckbutton_extension.type_name = "GtkCheckButton";
	kxgtkcheckbutton_extension.parent = &kxgtkbutton_extension;
	kxgtkcheckbutton_extension.free = kxgtkcheckbutton_free;
//%init_extension%
}


static void
kxgtkcheckbutton_add_method_table(KxGtkCheckButton *self);

KxObject *
kxgtkcheckbutton_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkcheckbutton_extension;

	
	kxgtkcheckbutton_add_method_table(self);

	
	self->data.ptr = gtk_check_button_new();
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkcheckbutton_from(KxCore *core, GtkCheckButton* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkcheckbutton_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
gtk_check_button_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_check_button_new();
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkcheckbutton_cloneWithLabel_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXGTKCHECKBUTTON((GtkCheckButton*)gtk_check_button_new_with_label(param0));
}
static KxObject *
kxgtkcheckbutton_cloneWithMnemonic_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXGTKCHECKBUTTON((GtkCheckButton*)gtk_check_button_new_with_mnemonic(param0));
}



static void
kxgtkcheckbutton_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_check_button_clone},
		{"cloneWithLabel:",1,kxgtkcheckbutton_cloneWithLabel_},
		{"cloneWithMnemonic:",1,kxgtkcheckbutton_cloneWithMnemonic_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
