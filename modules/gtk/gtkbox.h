/*************************
 *
 *  GtkBox
 *
 */

#ifndef __KX_OBJECT_GTKBOX_H
#define __KX_OBJECT_GTKBOX_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKBOX_DATA(self) ((GtkBox*) (self)->data.ptr)

#define KXGTKBOX(data) kxgtkbox_from(KXCORE, (data))

#define IS_KXGTKBOX(self) (kxobject_check_type(self,&kxgtkbox_extension))

#define KXPARAM_TO_GTKBOX(param_name, param_id) \
	GtkBox* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKBOX(tmp)) \
	{ param_name = KXGTKBOX_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkbox_extension); }} 

typedef struct KxObject KxGtkBox;

KxGtkBox *kxgtkbox_new_prototype(KxObject *parent);
KxObject *kxgtkbox_from(KxCore *core, GtkBox* data);


void kxgtkbox_extension_init();
extern KxObjectExtension kxgtkbox_extension;


#endif // __KX_OBJECT_GTKBOX_H
