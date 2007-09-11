#ifndef __KX_GTK_UTILS_H
#define __KX_GTK_UTILS_H

#include <gtk/gtk.h>
#include "kxobject.h"

#define KXPARAM_TO_GBOOLEAN(param_name, param_id) \
	gboolean param_name; { KxObject *tmp = message->params[param_id]; if (tmp == KXCORE->object_true) \
	{ param_name = TRUE; } else if(tmp == KXCORE->object_false) { param_name = FALSE; } else \
	{ return kxobject_need_boolean(tmp); }} 

#define KXGBOOLEAN(value) kxgboolean_from(KXCORE, value);


KxObject *kxgboolean_from(KxCore *core, gboolean value);

extern KxObjectExtension gtkabstractclass_extension;

void gtk_utils_init();

void kxgtkobject_extra_init(KxObject *self);

void kxgtk_set_wrapper(KxObject *self, GObject *gobject);
void kxgtk_remove_wrapper(KxObject *self);

KxObject *kxgtk_check_wrapper(GObject *gobject);

KxObject * kxgtk_kxobject_from_gvalue(const GValue *value);
KxObject * kxgtk_kxobject_from_gobject(GObject *gobject);

typedef KxObject * (KxNewObjectFcn)(KxCore *, void *);

void kxgtk_register_boxed_type(GType boxed_type,  KxObject *prototype);


extern GQuark kxgtk_prototype_key;
extern GQuark kxgtk_wrapper_key;


#endif