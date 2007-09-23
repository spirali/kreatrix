#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gdkgeometry.h"

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

KxObjectExtension kxgdkgeometry_extension;

static void kxgdkgeometry_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	free(self->data.ptr);
	//kxgtk_remove_wrapper(self);
}

static void kxgdkgeometry_mark(KxObject *self)
{
	
}

void kxgdkgeometry_extension_init() {
	kxobjectext_init(&kxgdkgeometry_extension);
	kxgdkgeometry_extension.type_name = "GdkGeometry";
	kxgdkgeometry_extension.parent = NULL;
	kxgdkgeometry_extension.free = kxgdkgeometry_free;
	kxgdkgeometry_extension.mark = kxgdkgeometry_mark;
//%init_extension%
}


static void
kxgdkgeometry_add_method_table(KxGdkGeometry *self);

KxObject *
kxgdkgeometry_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgdkgeometry_extension;

	
	kxgdkgeometry_add_method_table(self);

	self->data.ptr = calloc(sizeof(GdkGeometry),1);


	return self;
}


KxObject *
kxgdkgeometry_from(KxCore *core, GdkGeometry* data)
{
	
	KxObject *self; 
	KxObject *proto = kxcore_get_prototype(core, &kxgdkgeometry_extension);
	self = kxobject_raw_clone(proto);
	self->data.ptr = data;
	return self;

/*
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgdkgeometry_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gdk_geometry_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = calloc(sizeof(GdkGeometry),1);
	return clone;
}
static KxObject *
gdk_geometry_min_width (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->min_width);
}
static KxObject *
gdk_geometry_min_width_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->min_width = param0;
}
static KxObject *
gdk_geometry_min_height (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->min_height);
}
static KxObject *
gdk_geometry_min_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->min_height = param0;
}
static KxObject *
gdk_geometry_max_width (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->max_width);
}
static KxObject *
gdk_geometry_max_width_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->max_width = param0;
}
static KxObject *
gdk_geometry_max_height (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->max_height);
}
static KxObject *
gdk_geometry_max_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->max_height = param0;
}
static KxObject *
gdk_geometry_base_width (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->base_width);
}
static KxObject *
gdk_geometry_base_width_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->base_width = param0;
}
static KxObject *
gdk_geometry_base_height (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->base_height);
}
static KxObject *
gdk_geometry_base_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->base_height = param0;
}
static KxObject *
gdk_geometry_width_inc (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->width_inc);
}
static KxObject *
gdk_geometry_width_inc_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->width_inc = param0;
}
static KxObject *
gdk_geometry_height_inc (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->height_inc);
}
static KxObject *
gdk_geometry_height_inc_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->height_inc = param0;
}
static KxObject *
gdk_geometry_min_aspect (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->min_aspect);
}
static KxObject *
gdk_geometry_min_aspect_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->min_aspect = param0;
}
static KxObject *
gdk_geometry_max_aspect (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->max_aspect);
}
static KxObject *
gdk_geometry_max_aspect_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->max_aspect = param0;
}
static KxObject *
gdk_geometry_win_gravity (KxObject *self, KxMessage *message)
{
	return KXINTEGER( ((GdkGeometry*) self->data.ptr)->win_gravity);
}
static KxObject *
gdk_geometry_win_gravity_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	((GdkGeometry*) self->data.ptr)->win_gravity = param0;
}



static void
kxgdkgeometry_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gdk_geometry_clone},
		{"minWidth",0,gdk_geometry_min_width},
		{"minWidth:",1,gdk_geometry_min_width_},
		{"minHeight",0,gdk_geometry_min_height},
		{"minHeight:",1,gdk_geometry_min_height_},
		{"maxWidth",0,gdk_geometry_max_width},
		{"maxWidth:",1,gdk_geometry_max_width_},
		{"maxHeight",0,gdk_geometry_max_height},
		{"maxHeight:",1,gdk_geometry_max_height_},
		{"baseWidth",0,gdk_geometry_base_width},
		{"baseWidth:",1,gdk_geometry_base_width_},
		{"baseHeight",0,gdk_geometry_base_height},
		{"baseHeight:",1,gdk_geometry_base_height_},
		{"widthInc",0,gdk_geometry_width_inc},
		{"widthInc:",1,gdk_geometry_width_inc_},
		{"heightInc",0,gdk_geometry_height_inc},
		{"heightInc:",1,gdk_geometry_height_inc_},
		{"minAspect",0,gdk_geometry_min_aspect},
		{"minAspect:",1,gdk_geometry_min_aspect_},
		{"maxAspect",0,gdk_geometry_max_aspect},
		{"maxAspect:",1,gdk_geometry_max_aspect_},
		{"winGravity",0,gdk_geometry_win_gravity},
		{"winGravity:",1,gdk_geometry_win_gravity_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
