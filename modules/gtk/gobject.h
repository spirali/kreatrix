/*************************
 *
 *  GObject
 *
 */

#ifndef __KX_OBJECT_GOBJECT_H
#define __KX_OBJECT_GOBJECT_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGOBJECT_DATA(self) ((GObject*) (self)->data.ptr)

#define KXGOBJECT(data) kxgobject_from(KXCORE, (data))

#define IS_KXGOBJECT(self) (kxobject_check_type(self,&kxgobject_extension))

#define KXPARAM_TO_GOBJECT(param_name, param_id) \
	GObject* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGOBJECT(tmp)) \
	{ param_name = KXGOBJECT_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgobject_extension); }} 

typedef struct KxObject KxGObject;

KxGObject *kxgobject_new_prototype(KxObject *parent);
KxObject *kxgobject_from(KxCore *core, GObject* data);


void kxgobject_extension_init();
extern KxObjectExtension kxgobject_extension;


#endif // __KX_OBJECT_GOBJECT_H
