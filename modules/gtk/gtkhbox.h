/*************************
 *
 *  GtkHBox
 *
 */

#ifndef __KX_OBJECT_GTKHBOX_H
#define __KX_OBJECT_GTKHBOX_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKHBOX_DATA(self) ((GtkHBox*) (self)->data.ptr)

#define KXGTKHBOX(data) kxgtkhbox_from(KXCORE, (data))

#define IS_KXGTKHBOX(self) (kxobject_check_type(self,&kxgtkhbox_extension))

#define KXPARAM_TO_GTKHBOX(param_name, param_id) \
	GtkHBox* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKHBOX(tmp)) \
	{ param_name = KXGTKHBOX_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkhbox_extension); }} 

typedef struct KxObject KxGtkHBox;

KxGtkHBox *kxgtkhbox_new_prototype(KxObject *parent);
KxObject *kxgtkhbox_from(KxCore *core, GtkHBox* data);


void kxgtkhbox_extension_init();
extern KxObjectExtension kxgtkhbox_extension;


#endif // __KX_OBJECT_GTKHBOX_H
