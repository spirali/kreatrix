/*************************
 *
 *  GtkButton
 *
 */

#ifndef __KX_OBJECT_GTKBUTTON_H
#define __KX_OBJECT_GTKBUTTON_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKBUTTON_DATA(self) ((GtkButton*) (self)->data.ptr)

#define KXGTKBUTTON(data) kxgtkbutton_from(KXCORE, (data))

#define IS_KXGTKBUTTON(self) (kxobject_check_type(self,&kxgtkbutton_extension))

#define KXPARAM_TO_GTKBUTTON(param_name, param_id) \
	GtkButton* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKBUTTON(tmp)) \
	{ param_name = KXGTKBUTTON_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkbutton_extension); }} 

typedef struct KxObject KxGtkButton;

KxGtkButton *kxgtkbutton_new_prototype(KxObject *parent);
KxObject *kxgtkbutton_from(KxCore *core, GtkButton* data);


void kxgtkbutton_extension_init();
extern KxObjectExtension kxgtkbutton_extension;


#endif // __KX_OBJECT_GTKBUTTON_H
