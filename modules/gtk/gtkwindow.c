#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkwindow.h"

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

KxObjectExtension kxgtkwindow_extension;

static void kxgtkwindow_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkwindow_extension_init() {
	kxobjectext_init(&kxgtkwindow_extension);
	kxgtkwindow_extension.type_name = "GtkWindow";
	kxgtkwindow_extension.parent = &kxgtkbin_extension;
	kxgtkwindow_extension.free = kxgtkwindow_free;
//%init_extension%
}


static void
kxgtkwindow_add_method_table(KxGtkWindow *self);

KxObject *
kxgtkwindow_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkwindow_extension;

	
	kxgtkwindow_add_method_table(self);

	
	self->data.ptr = gtk_window_new(0);
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkwindow_from(KxCore *core, GtkWindow* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkwindow_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
gtk_window_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_window_new(0);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkwindow_clone_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	return KXGTKWINDOW((GtkWindow*)gtk_window_new(param0));
}
static KxObject *
kxgtkwindow_title_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_window_set_title(self->data.ptr,param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_policyShrink_grow_autoShrink_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	KXPARAM_TO_INT(param2,2);
	gtk_window_set_policy(self->data.ptr,param0,param1,param2);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_resizable_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_resizable(self->data.ptr,param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_isResizable (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_resizable(self->data.ptr));
}
static KxObject *
kxgtkwindow_activateFocus (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_activate_focus(self->data.ptr));
}
static KxObject *
kxgtkwindow_activateDefault (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_activate_default(self->data.ptr));
}
static KxObject *
kxgtkwindow_modal_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_modal(self->data.ptr,param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_defaultWidth_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_window_set_default_size(self->data.ptr,param0,param1);
	KXRETURN(self);
}



static void
kxgtkwindow_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_window_clone},
		{"clone:",1,kxgtkwindow_clone_},
		{"title:",1,kxgtkwindow_title_},
		{"policyShrink:grow:autoShrink:",3,kxgtkwindow_policyShrink_grow_autoShrink_},
		{"resizable:",1,kxgtkwindow_resizable_},
		{"isResizable",0,kxgtkwindow_isResizable},
		{"activateFocus",0,kxgtkwindow_activateFocus},
		{"activateDefault",0,kxgtkwindow_activateDefault},
		{"modal:",1,kxgtkwindow_modal_},
		{"defaultWidth:height:",2,kxgtkwindow_defaultWidth_height_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
