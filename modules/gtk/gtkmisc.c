#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkmisc.h"

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

KxObjectExtension kxgtkmisc_extension;

static void kxgtkmisc_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkmisc_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtkmisc_extension_init() {
	kxobjectext_init(&kxgtkmisc_extension);
	kxgtkmisc_extension.type_name = "GtkMisc";
	kxgtkmisc_extension.parent = &kxgtkwidget_extension;
	kxgtkmisc_extension.free = kxgtkmisc_free;
	kxgtkmisc_extension.mark = kxgtkmisc_mark;
//%init_extension%
}


static void
kxgtkmisc_add_method_table(KxGtkMisc *self);

KxObject *
kxgtkmisc_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkmisc_extension;

	
	kxgtkmisc_add_method_table(self);

	
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkmisc_from(KxCore *core, GtkMisc* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkmisc_extension);
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

	KxObject *proto = kxcore_get_prototype(core, &kxgtkmisc_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}




static void
kxgtkmisc_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
