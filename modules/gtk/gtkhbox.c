#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkhbox.h"

#include "gobject.h"
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
#include "gtkmisc.h"
#include "gtklabel.h"
#include "gtkaccelgroup.h"
#include "gtkrequisition.h"
#include "gdkgeometry.h"
#include "gtk_utils.h"

KxObjectExtension kxgtkhbox_extension;

static void kxgtkhbox_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkhbox_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtkhbox_extension_init() {
	kxobjectext_init(&kxgtkhbox_extension);
	kxgtkhbox_extension.type_name = "GtkHBox";
	kxgtkhbox_extension.parent = &kxgtkbox_extension;
	kxgtkhbox_extension.free = kxgtkhbox_free;
	kxgtkhbox_extension.mark = kxgtkhbox_mark;
//%init_extension%
}


static void
kxgtkhbox_add_method_table(KxGtkHBox *self);

KxObject *
kxgtkhbox_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkhbox_extension;

	
	kxgtkhbox_add_method_table(self);

	
	self->data.ptr = gtk_hbox_new(TRUE,0);
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkhbox_from(KxCore *core, GtkHBox* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkhbox_extension);
	self = kxobject_raw_clone(proto);
	g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;

/*
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkhbox_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gtk_hbox_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_hbox_new(TRUE,0);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkhbox_cloneHomogenous_spacing_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	KXPARAM_TO_INT(param1,1);
	return KXGTKVBOX((GtkVBox*)gtk_hbox_new((gboolean)param0,(gint)param1));
}



static void
kxgtkhbox_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_hbox_clone},
		{"cloneHomogenous:spacing:",2,kxgtkhbox_cloneHomogenous_spacing_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
