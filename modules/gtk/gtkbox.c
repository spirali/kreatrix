#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkbox.h"

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

KxObjectExtension kxgtkbox_extension;

static void kxgtkbox_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkbox_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtkbox_extension_init() {
	kxobjectext_init(&kxgtkbox_extension);
	kxgtkbox_extension.type_name = "GtkBox";
	kxgtkbox_extension.parent = &kxgtkcontainer_extension;
	kxgtkbox_extension.free = kxgtkbox_free;
	kxgtkbox_extension.mark = kxgtkbox_mark;
//%init_extension%
}


static void
kxgtkbox_add_method_table(KxGtkBox *self);

KxObject *
kxgtkbox_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkbox_extension;

	
	kxgtkbox_add_method_table(self);

	
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkbox_from(KxCore *core, GtkBox* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkbox_extension);
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

	KxObject *proto = kxcore_get_prototype(core, &kxgtkbox_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
kxgtkbox_packStart_expand_fill_padding_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	KXPARAM_TO_GBOOLEAN(param1,1);
	KXPARAM_TO_GBOOLEAN(param2,2);
	KXPARAM_TO_INT(param3,3);
	gtk_box_pack_start(self->data.ptr,(GtkWidget*)param0,(gboolean)param1,(gboolean)param2,(guint)param3);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_packEnd_expend_fill_padding_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	KXPARAM_TO_GBOOLEAN(param1,1);
	KXPARAM_TO_GBOOLEAN(param2,2);
	KXPARAM_TO_INT(param3,3);
	gtk_box_pack_end(self->data.ptr,(GtkWidget*)param0,(gboolean)param1,(gboolean)param2,(guint)param3);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_packStartDefaults_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_box_pack_start_defaults(self->data.ptr,(GtkWidget*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_packEndDefaults_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_box_pack_end_defaults(self->data.ptr,(GtkWidget*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_isHomogeneous (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_box_get_homogeneous(self->data.ptr));
}
static KxObject *
kxgtkbox_homogeneous_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_box_set_homogeneous(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_getSpacing (KxObject *self, KxMessage *message)
{

	return KXINTEGER((gint)gtk_box_get_spacing(self->data.ptr));
}
static KxObject *
kxgtkbox_spacing_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_box_set_spacing(self->data.ptr,(gint)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkbox_reorderChild_position_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_box_reorder_child(self->data.ptr,(GtkWidget*)param0,(gint)param1);
	KXRETURN(self);
}



static void
kxgtkbox_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"packStart:expand:fill:padding:",4,kxgtkbox_packStart_expand_fill_padding_},
		{"packEnd:expend:fill:padding:",4,kxgtkbox_packEnd_expend_fill_padding_},
		{"packStartDefaults:",1,kxgtkbox_packStartDefaults_},
		{"packEndDefaults:",1,kxgtkbox_packEndDefaults_},
		{"isHomogeneous",0,kxgtkbox_isHomogeneous},
		{"homogeneous:",1,kxgtkbox_homogeneous_},
		{"getSpacing",0,kxgtkbox_getSpacing},
		{"spacing:",1,kxgtkbox_spacing_},
		{"reorderChild:position:",2,kxgtkbox_reorderChild_position_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
