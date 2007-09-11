/*************************
 *
 *  GtkContainer
 *
 */

#ifndef __KX_OBJECT_GTKCONTAINER_H
#define __KX_OBJECT_GTKCONTAINER_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKCONTAINER_DATA(self) ((GtkContainer*) (self)->data.ptr)

#define KXGTKCONTAINER(data) kxgtkcontainer_from(KXCORE, (data))

#define IS_KXGTKCONTAINER(self) (kxobject_check_type(self,&kxgtkcontainer_extension))

#define KXPARAM_TO_GTKCONTAINER(param_name, param_id) \
	GtkContainer* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKCONTAINER(tmp)) \
	{ param_name = KXGTKCONTAINER_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkcontainer_extension); }} 

typedef struct KxObject KxGtkContainer;

KxGtkContainer *kxgtkcontainer_new_prototype(KxObject *parent);
KxObject *kxgtkcontainer_from(KxCore *core, GtkContainer* data);


void kxgtkcontainer_extension_init();
extern KxObjectExtension kxgtkcontainer_extension;


#endif // __KX_OBJECT_GTKCONTAINER_H
