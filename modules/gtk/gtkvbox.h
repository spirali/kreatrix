/*************************
 *
 *  GtkVBox
 *
 */

#ifndef __KX_OBJECT_GTKVBOX_H
#define __KX_OBJECT_GTKVBOX_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKVBOX_DATA(self) ((GtkVBox*) (self)->data.ptr)

#define KXGTKVBOX(data) kxgtkvbox_from(KXCORE, (data))

#define IS_KXGTKVBOX(self) (kxobject_check_type(self,&kxgtkvbox_extension))

#define KXPARAM_TO_GTKVBOX(param_name, param_id) \
	GtkVBox* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKVBOX(tmp)) \
	{ param_name = KXGTKVBOX_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkvbox_extension); }} 

typedef struct KxObject KxGtkVBox;

KxGtkVBox *kxgtkvbox_new_prototype(KxObject *parent);
KxObject *kxgtkvbox_from(KxCore *core, GtkVBox* data);


void kxgtkvbox_extension_init();
extern KxObjectExtension kxgtkvbox_extension;


#endif // __KX_OBJECT_GTKVBOX_H
