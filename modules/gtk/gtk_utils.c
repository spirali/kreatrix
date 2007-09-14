
#include <stdlib.h>
#include <stdio.h>

#include "gtk_utils.h"

#include "kxstring.h"
#include "kxinteger.h"
#include "kxclosure.h"
#include "kxexception.h"

KxObjectExtension gtkabstractclass_extension;

GQuark kxgtk_prototype_key;
GQuark kxgtk_wrapper_key;
GQuark kxgtk_constructor_key;



KxObject * kxgtk_kxobject_from_boxed(const GValue *value);


KxObject *
kxgboolean_from(KxCore *core, gboolean value)
{
	if (value == TRUE) {
		KXRETURN(core->object_true);
	} else {
		KXRETURN(core->object_false);
	}
}

static void 
kxgtk_init_keys() 
{
    kxgtk_prototype_key = g_quark_from_static_string("KxGObject::prototype");
    kxgtk_wrapper_key = g_quark_from_static_string("KxGObject::wrapper");
    kxgtk_constructor_key = g_quark_from_static_string("KxGObject::constructor");
}

static void 
kxgtk_mark_windows(KxCore *core)
{
	List *list = kxcore_get_global_data(core, "kxgtk-window-list");
	int t;
	for (t=0;t<list->size;t++) {
		kxgtk_mark_gobject(list->items[t]);
	}
}

void kxgtk_utils_init(KxCore *core)
{
	kxgtk_init_keys();

	// Init abstract class
	kxobjectext_init(&gtkabstractclass_extension);
	gtkabstractclass_extension.type_name = "GtkAbstractClass";

	kxcore_register_mark_function(core, kxgtk_mark_windows);
}

static KxObject *
kxgtkobject_signal_connect(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(signal_name, 0);
	KxObject *block = message->params[1];
    guint handlerid, sigid, len;
    GQuark detail;
   if (!g_signal_parse_name(signal_name, G_OBJECT_TYPE(self->data.ptr),
			     &sigid, &detail, TRUE)) {
		// TODO: Gtk exception
		KXTHROW_EXCEPTION("Unknown signal name");
   }
   KxClosure *closure = kxclosure_new(block);

   handlerid = g_signal_connect_closure_by_id(
	   	(GObject*) self->data.ptr, sigid, detail, (GClosure*) closure, TRUE);
   
   return KXINTEGER(handlerid);
}

void kxgtkobject_extra_init(KxObject *self) 
{
	// Temporary remove extension, self type not implemented yet.
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"signal:connect:",2,  kxgtkobject_signal_connect},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;

}

static void 
kxgtk_toggle_ref(KxObject *self, GObject *gobject, gboolean is_last)
{
	//printf("toggle_ref %i : ", is_last); kxobject_dump(self);
	//KxObject *self = g_object_get_qdata(gobject, kxgtk_wrapper_key);
	//kxobject_dump(self);
	if (is_last == TRUE) {
//		printf("- ref %p %i\n", gobject, gobject->ref_count);
		REF_REMOVE(self);
//		printf("---\n");
	} else {
//		printf("+ ref %p \n", gobject);
		REF_ADD(self);
	}
}

void
kxgtk_destroy_notify(gpointer data)
{
	// Do nothing
}

static void
kxgtk_register_in_window_list(KxCore *core, GObject *gobject)
{
	List *list = kxcore_get_global_data(core, "kxgtk-window-list");
	list_append(list, gobject);
}

void 
kxgtk_set_wrapper(KxObject *self, GObject *gobject)
{
//	g_object_set_qdata_full(gobject, kxgtk_wrapper_key, self, kxgtk_destroy_notify);
	g_object_set_qdata(gobject, kxgtk_wrapper_key, self);

//	printf("set_wrapper %p ", gobject); kxobject_dump(self);
	/*printf("xxADD %p %p %p :: ", gobject, 
		(GToggleNotify) kxgtk_toggle_ref, self);
	kxobject_dump(self);*/

	g_object_add_toggle_ref(gobject, 
		(GToggleNotify) kxgtk_toggle_ref, self);
//	printf("Acount = %i %i\n",gobject->ref_count, self->ref_count);
	REF_ADD(self);
//	printf("Bcount = %i %i\n",gobject->ref_count, self->ref_count);
	//REF_ADD(self);
//	printf("Ccount = %i %i\n",gobject->ref_count, self->ref_count);

	if (g_type_is_a(G_OBJECT_TYPE(gobject),GTK_TYPE_WINDOW)) {
	/*	g_object_ref(gobject);
		g_object_unref(gobject);*/
		kxgtk_register_in_window_list(KXCORE, gobject);
	} else {
		if (g_object_is_floating(gobject)) {
			g_object_ref_sink(gobject);
		}
		g_object_unref(gobject);
	}

	/*g_object_ref(gobject);
	g_object_ref_sink(gobject);
	g_object_unref(gobject);*/
	//REF_ADD(self);
}

void
kxgtk_register_boxed_type(GType boxed_type, KxObject *prototype)
{
	g_type_set_qdata(boxed_type, kxgtk_prototype_key, prototype);
}


void 
kxgtk_remove_wrapper(KxObject *self)
{
	//printf("remove wrapper: ----------vvvvv---------\n");
	/*printf("xxREMOVE %p %p %p ::", self->data.ptr, 
		(GToggleNotify) kxgtk_toggle_ref, self);
	kxobject_dump(self);*/
	GObject *gobject = self->data.ptr;

	//g_object_steal_qdata(self->data.ptr, kxgtk_wrapper_key);


	if (g_type_is_a(G_OBJECT_TYPE(gobject),GTK_TYPE_WINDOW)) {
		g_object_ref(gobject);
		g_object_remove_toggle_ref(self->data.ptr, 
			(GToggleNotify) kxgtk_toggle_ref, self);
	//	g_object_unref(gobject);
	//
	} else {
		g_object_remove_toggle_ref(self->data.ptr, 
			(GToggleNotify) kxgtk_toggle_ref, self);
	}
	//printf("remove wrapper: ----------AAAAA---------\n");
}

KxObject *
kxgtk_check_wrapper(GObject *gobject)
{
	KxObject *self = g_object_get_qdata(gobject, kxgtk_wrapper_key);
	if (self != NULL) {
		REF_ADD(self);
		return self;
	}
	return NULL;
}

KxObject *
kxgtk_kxobject_from_gvalue(const GValue *value)
{
	switch(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value)))
	{
		case G_TYPE_OBJECT:
			return kxgtk_kxobject_from_gobject(g_value_get_object(value));
		case G_TYPE_BOXED: 
			return kxgtk_kxobject_from_boxed(value);
		case G_TYPE_PARAM:
			printf("PARAM!\n");
			abort();

		default:
			// TODO: fix this
		    printf("Cannot convert type %s/%s to KxObject\n",
				g_type_name(G_VALUE_TYPE(value)),
				g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value)))
			);
			abort();
	}
}

KxObject * 
kxgtk_kxobject_from_gobject(GObject *gobject)
{
	KxObject *self = g_object_get_qdata(gobject, kxgtk_wrapper_key);
	if (self != NULL) {
		REF_ADD(self);
		return self;
	}
	printf("Wrapper not found %p\n", gobject);
	abort();
}

KxObject * 
kxgtk_kxobject_from_boxed(const GValue *value)
{
	gpointer boxed = g_value_get_boxed(value);
	KxObject *proto = g_type_get_qdata(G_VALUE_TYPE(value), kxgtk_prototype_key);
	if (boxed == NULL) {
		printf("oops\n");
	}

	if (proto != NULL) {
		KxObject *self = kxobject_raw_clone(proto);
		self->data.ptr = boxed;
		return self;
	}

    printf("Cannot convert BOXED type %s/%s to KxObject\n",
				g_type_name(G_VALUE_TYPE(value)),
				g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value)))
			);
	abort();
}

static void kxgtk_mark_callback(GtkWidget *obj, gpointer data);

void kxgtk_mark_container(GObject *obj)
{
	GType type = G_OBJECT_TYPE(obj);
	if (G_TYPE_CHECK_CLASS_TYPE(type, GTK_TYPE_CONTAINER)) {
		gtk_container_forall(GTK_CONTAINER(obj), kxgtk_mark_callback, NULL);
	}
}

void
kxgtk_mark_gobject(GObject *obj) 
{
	KxObject *self = g_object_get_qdata(obj, kxgtk_wrapper_key);
	if (self) {
		if (KXOBJECT_HAS_GC_MARK(self)) {
			return;
		}
		kxobject_mark(self);
	} else 
		kxgtk_mark_container(obj);
}

static void
kxgtk_mark_callback(GtkWidget *obj, gpointer data)
{
	kxgtk_mark_bafff_gobject(G_OBJECT(obj));
}


