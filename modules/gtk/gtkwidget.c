#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkwidget.h"

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

KxObjectExtension kxgtkwidget_extension;

static void kxgtkwidget_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkwidget_extension_init() {
	kxobjectext_init(&kxgtkwidget_extension);
	kxgtkwidget_extension.type_name = "GtkWidget";
	kxgtkwidget_extension.parent = &kxgtkobject_extension;
	kxgtkwidget_extension.free = kxgtkwidget_free;
//%init_extension%
}


static void
kxgtkwidget_add_method_table(KxGtkWidget *self);

KxObject *
kxgtkwidget_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkwidget_extension;

	
	kxgtkwidget_add_method_table(self);

	
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkwidget_from(KxCore *core, GtkWidget* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkwidget_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}

static KxObject *
kxgtkwidget_unparent (KxObject *self, KxMessage *message)
{

	gtk_widget_unparent(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_show (KxObject *self, KxMessage *message)
{

	gtk_widget_show(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_showNow (KxObject *self, KxMessage *message)
{

	gtk_widget_show_now(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_hide (KxObject *self, KxMessage *message)
{

	gtk_widget_hide(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_showAll (KxObject *self, KxMessage *message)
{

	gtk_widget_show_all(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_hideAll (KxObject *self, KxMessage *message)
{

	gtk_widget_hide_all(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwidget_map (KxObject *self, KxMessage *message)
{

	gtk_widget_map(self->data.ptr);
	KXRETURN(self);
}



static void
kxgtkwidget_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"unparent",0,kxgtkwidget_unparent},
		{"show",0,kxgtkwidget_show},
		{"showNow",0,kxgtkwidget_showNow},
		{"hide",0,kxgtkwidget_hide},
		{"showAll",0,kxgtkwidget_showAll},
		{"hideAll",0,kxgtkwidget_hideAll},
		{"map",0,kxgtkwidget_map},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
