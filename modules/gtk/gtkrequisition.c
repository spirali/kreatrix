#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkrequisition.h"

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

KxObjectExtension kxgtkrequisition_extension;

static void kxgtkrequisition_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	free(self->data.ptr);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkrequisition_mark(KxObject *self)
{
	
}

void kxgtkrequisition_extension_init() {
	kxobjectext_init(&kxgtkrequisition_extension);
	kxgtkrequisition_extension.type_name = "GtkRequisition";
	kxgtkrequisition_extension.parent = NULL;
	kxgtkrequisition_extension.free = kxgtkrequisition_free;
	kxgtkrequisition_extension.mark = kxgtkrequisition_mark;
//%init_extension%
}


static void
kxgtkrequisition_add_method_table(KxGtkRequisition *self);

KxObject *
kxgtkrequisition_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkrequisition_extension;

	
	kxgtkrequisition_add_method_table(self);

	self->data.ptr = calloc(sizeof(GtkRequisition),1);


	return self;
}


KxObject *
kxgtkrequisition_from(KxCore *core, GtkRequisition* data)
{
	
	KxObject *self; 
	KxObject *proto = kxcore_get_prototype(core, &kxgtkrequisition_extension);
	self = kxobject_raw_clone(proto);
	self->data.ptr = data;
	return self;

/*
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkrequisition_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gtk_requisition_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = calloc(sizeof(GtkRequisition),1);
	return clone;
}
static KxObject *
gtk_requisition_width (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GtkRequisition*) self->data.ptr)->width);
}
static KxObject *
gtk_requisition_width_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GtkRequisition*) self->data.ptr)->width = param0;
}
static KxObject *
gtk_requisition_height (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GtkRequisition*) self->data.ptr)->height);
}
static KxObject *
gtk_requisition_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GtkRequisition*) self->data.ptr)->height = param0;
}



static void
kxgtkrequisition_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_requisition_clone},
		{"width",0,gtk_requisition_width},
		{"width:",1,gtk_requisition_width_},
		{"height",0,gtk_requisition_height},
		{"height:",1,gtk_requisition_height_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
