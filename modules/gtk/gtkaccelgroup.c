#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkaccelgroup.h"

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

KxObjectExtension kxgtkaccelgroup_extension;

static void kxgtkaccelgroup_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkaccelgroup_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtkaccelgroup_extension_init() {
	kxobjectext_init(&kxgtkaccelgroup_extension);
	kxgtkaccelgroup_extension.type_name = "GtkAccelGroup";
	kxgtkaccelgroup_extension.parent = &kxgobject_extension;
	kxgtkaccelgroup_extension.free = kxgtkaccelgroup_free;
	kxgtkaccelgroup_extension.mark = kxgtkaccelgroup_mark;
//%init_extension%
}


static void
kxgtkaccelgroup_add_method_table(KxGtkAccelGroup *self);

KxObject *
kxgtkaccelgroup_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkaccelgroup_extension;

	
	kxgtkaccelgroup_add_method_table(self);

	
	self->data.ptr = gtk_accel_group_new();
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkaccelgroup_from(KxCore *core, GtkAccelGroup* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkaccelgroup_extension);
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

	KxObject *proto = kxcore_get_prototype(core, &kxgtkaccelgroup_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gtk_accel_group_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_accel_group_new();
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}



static void
kxgtkaccelgroup_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_accel_group_clone},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
