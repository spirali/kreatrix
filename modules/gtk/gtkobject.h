/*************************
 *
 *  GtkObject
 *
 */

#ifndef __KX_OBJECT_GTKOBJECT_H
#define __KX_OBJECT_GTKOBJECT_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKOBJECT_DATA(self) ((GtkObject*) (self)->data.ptr)

#define KXGTKOBJECT(data) kxgtkobject_from(KXCORE, (data))

#define IS_KXGTKOBJECT(self) (kxobject_check_type(self,&kxgtkobject_extension))

#define KXPARAM_TO_GTKOBJECT(param_name, param_id) \
	GtkObject* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKOBJECT(tmp)) \
	{ param_name = KXGTKOBJECT_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkobject_extension); }} 

typedef struct KxObject KxGtkObject;

KxGtkObject *kxgtkobject_new_prototype(KxObject *parent);
KxObject *kxgtkobject_from(KxCore *core, GtkObject* data);


void kxgtkobject_extension_init();
extern KxObjectExtension kxgtkobject_extension;


#endif // __KX_OBJECT_GTKOBJECT_H
