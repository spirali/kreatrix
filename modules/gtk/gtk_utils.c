
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
GQuark kxgtk_closures_key;



KxObject * kxgtk_kxobject_from_boxed(const GValue *value);

static void kxgtk_watch_closure(GObject *obj, KxClosure *closure);



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
    kxgtk_closures_key = g_quark_from_static_string("KxGObject::closures");
}

static void 
kxgtk_mark_windows(KxCore *core)
{
	List *list = kxcore_get_global_data(core, "kxgtk-window-list");

	if (list == NULL)
		return;
	
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

void kxgtk_utils_unload(KxCore *core) 
{
	//printf("Module UNLOAD\n");
	List *list = 
		(List*) kxcore_get_global_data(core, "kxgtk-window-list");
	kxcore_remove_global_data(core,"kxgtk-window-list");

	int t;
	for (t=0;t<list->size;t++) {
		gtk_widget_destroy(GTK_WIDGET(list->items[t]));
	}
	list_free(list);

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
  
   kxgtk_watch_closure((GObject*) self->data.ptr, closure);

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
	//kxobject_dump(self);
	if (is_last == TRUE) {
	//	printf("- ref %p %i\n", gobject, gobject->ref_count);
		REF_REMOVE(self);
	} else {
	//	printf("+ ref %p \n", gobject);
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

static void
kxgtk_window_destroy_notify(GObject *window, gpointer data)
{
	KxCore *core = data;
	List *list = kxcore_get_global_data(core, "kxgtk-window-list");
	if (list)
		list_remove_first(list, window);
}

void 
kxgtk_set_wrapper(KxObject *self, GObject *gobject)
{
//	g_object_set_qdata_full(gobject, kxgtk_wrapper_key, self, kxgtk_destroy_notify);
	g_object_set_qdata(gobject, kxgtk_wrapper_key, self);

	g_object_add_toggle_ref(gobject, 
		(GToggleNotify) kxgtk_toggle_ref, self);
	REF_ADD(self);

	if (g_type_is_a(G_OBJECT_TYPE(gobject),GTK_TYPE_WINDOW)) {
		kxgtk_register_in_window_list(KXCORE, gobject);
		g_object_connect(gobject, "signal::destroy", kxgtk_window_destroy_notify, KXCORE);
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
	/*printf("xxREMOVE %p %p %p ::", self->data.ptr, 
		(GToggleNotify) kxgtk_toggle_ref, self);
	kxobject_dump(self);*/
	GObject *gobject = self->data.ptr;



	if (g_type_is_a(G_OBJECT_TYPE(gobject),GTK_TYPE_WINDOW)) {
		
		g_object_remove_toggle_ref(self->data.ptr, 
			(GToggleNotify) kxgtk_toggle_ref, self);
	} else {
		g_object_remove_toggle_ref(self->data.ptr, 
			(GToggleNotify) kxgtk_toggle_ref, self);
	}
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
kxgtk_kxobject_from_gvalue(KxCore *core, const GValue *value)
{
	switch(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(value)))
	{
		case G_TYPE_OBJECT:
			return kxgtk_kxobject_from_gobject(core, g_value_get_object(value));
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
kxgtk_kxobject_from_gobject(KxCore *core, GObject *gobject)
{
	if (gobject == NULL) {
		REF_ADD(core->object_nil);
		return core->object_nil;
	}

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
		printf("oops boxed == NULL\n");
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
//	printf("container %s\n", g_type_name(type));
	if (G_TYPE_CHECK_INSTANCE_TYPE(obj, GTK_TYPE_CONTAINER)) {
//		printf("container OK\n");
		gtk_container_forall(GTK_CONTAINER(obj), kxgtk_mark_callback, NULL);
	}/* else {
		printf("no\n");
	}*/
}

static List *
kxgtk_list_of_closures(GObject *obj)
{
	List *list = g_object_get_qdata(obj, kxgtk_closures_key);
	if (list == NULL) {
		// TODO: replace List with SimpleList
		list = list_new();
	//	g_object_set_qdata(obj, kxgtk_closures_key, list);
		g_object_set_qdata_full(obj, kxgtk_closures_key, list, (GDestroyNotify) list_free);
	}
	return list;
}


void
kxgtk_mark_watched_closures(GObject *obj) 
{
	List *list = g_object_get_qdata(obj, kxgtk_closures_key);
	if (list) {
		int t;
		for (t=0;t<list->size;t++) {
			kxclosure_mark(list->items[t]);
		}
	}
}

void
kxgtk_mark_gobject(GObject *obj) 
{
	if (obj == NULL) 
		return;
	KxObject *self = g_object_get_qdata(obj, kxgtk_wrapper_key);
	if (self) {
		if (KXOBJECT_HAS_GC_MARK(self)) {
			return;
		}
		kxobject_mark(self);
	} else {
		kxgtk_mark_watched_closures(obj);
		kxgtk_mark_container(obj);
	}
}


static void
kxgtk_watch_closure(GObject *obj, KxClosure *closure) 
{
	List *list = kxgtk_list_of_closures(obj);
	list_append(list, closure);
}



static void
kxgtk_mark_callback(GtkWidget *obj, gpointer data)
{
	kxgtk_mark_gobject(G_OBJECT(obj));
}


