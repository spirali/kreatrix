/*************************
 *
 *  GtkRequisition
 *
 */

#ifndef __KX_OBJECT_GTKREQUISITION_H
#define __KX_OBJECT_GTKREQUISITION_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKREQUISITION_DATA(self) ((GtkRequisition*) (self)->data.ptr)

#define KXGTKREQUISITION(data) kxgtkrequisition_from(KXCORE, (data))

#define IS_KXGTKREQUISITION(self) (kxobject_check_type(self,&kxgtkrequisition_extension))

#define KXPARAM_TO_GTKREQUISITION(param_name, param_id) \
	GtkRequisition* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKREQUISITION(tmp)) \
	{ param_name = KXGTKREQUISITION_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkrequisition_extension); }} 

typedef struct KxObject KxGtkRequisition;

KxGtkRequisition *kxgtkrequisition_new_prototype(KxObject *parent);
KxObject *kxgtkrequisition_from(KxCore *core, GtkRequisition* data);


void kxgtkrequisition_extension_init();
extern KxObjectExtension kxgtkrequisition_extension;


#endif // __KX_OBJECT_GTKREQUISITION_H
