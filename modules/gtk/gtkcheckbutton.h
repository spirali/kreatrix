/*************************
 *
 *  GtkCheckButton
 *
 */

#ifndef __KX_OBJECT_GTKCHECKBUTTON_H
#define __KX_OBJECT_GTKCHECKBUTTON_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKCHECKBUTTON_DATA(self) ((GtkCheckButton*) (self)->data.ptr)

#define KXGTKCHECKBUTTON(data) kxgtkcheckbutton_from(KXCORE, (data))

#define IS_KXGTKCHECKBUTTON(self) (kxobject_check_type(self,&kxgtkcheckbutton_extension))

#define KXPARAM_TO_GTKCHECKBUTTON(param_name, param_id) \
	GtkCheckButton* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKCHECKBUTTON(tmp)) \
	{ param_name = KXGTKCHECKBUTTON_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkcheckbutton_extension); }} 

typedef struct KxObject KxGtkCheckButton;

KxGtkCheckButton *kxgtkcheckbutton_new_prototype(KxObject *parent);
KxObject *kxgtkcheckbutton_from(KxCore *core, GtkCheckButton* data);


void kxgtkcheckbutton_extension_init();
extern KxObjectExtension kxgtkcheckbutton_extension;


#endif // __KX_OBJECT_GTKCHECKBUTTON_H
