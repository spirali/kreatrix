/*************************
 *
 *  GtkAccelGroup
 *
 */

#ifndef __KX_OBJECT_GTKACCELGROUP_H
#define __KX_OBJECT_GTKACCELGROUP_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKACCELGROUP_DATA(self) ((GtkAccelGroup*) (self)->data.ptr)

#define KXGTKACCELGROUP(data) kxgtkaccelgroup_from(KXCORE, (data))

#define IS_KXGTKACCELGROUP(self) (kxobject_check_type(self,&kxgtkaccelgroup_extension))

#define KXPARAM_TO_GTKACCELGROUP(param_name, param_id) \
	GtkAccelGroup* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKACCELGROUP(tmp)) \
	{ param_name = KXGTKACCELGROUP_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkaccelgroup_extension); }} 

typedef struct KxObject KxGtkAccelGroup;

KxGtkAccelGroup *kxgtkaccelgroup_new_prototype(KxObject *parent);
KxObject *kxgtkaccelgroup_from(KxCore *core, GtkAccelGroup* data);


void kxgtkaccelgroup_extension_init();
extern KxObjectExtension kxgtkaccelgroup_extension;


#endif // __KX_OBJECT_GTKACCELGROUP_H
