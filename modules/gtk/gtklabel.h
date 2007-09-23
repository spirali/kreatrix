/*************************
 *
 *  GtkLabel
 *
 */

#ifndef __KX_OBJECT_GTKLABEL_H
#define __KX_OBJECT_GTKLABEL_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKLABEL_DATA(self) ((GtkLabel*) (self)->data.ptr)

#define KXGTKLABEL(data) kxgtklabel_from(KXCORE, (data))

#define IS_KXGTKLABEL(self) (kxobject_check_type(self,&kxgtklabel_extension))

#define KXPARAM_TO_GTKLABEL(param_name, param_id) \
	GtkLabel* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKLABEL(tmp)) \
	{ param_name = KXGTKLABEL_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtklabel_extension); }} 

typedef struct KxObject KxGtkLabel;

KxGtkLabel *kxgtklabel_new_prototype(KxObject *parent);
KxObject *kxgtklabel_from(KxCore *core, GtkLabel* data);


void kxgtklabel_extension_init();
extern KxObjectExtension kxgtklabel_extension;


#endif // __KX_OBJECT_GTKLABEL_H
