/*************************
 *
 *  GtkWidget
 *
 */

#ifndef __KX_OBJECT_GTKWIDGET_H
#define __KX_OBJECT_GTKWIDGET_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKWIDGET_DATA(self) ((GtkWidget*) (self)->data.ptr)

#define KXGTKWIDGET(data) kxgtkwidget_from(KXCORE, (data))

#define IS_KXGTKWIDGET(self) (kxobject_check_type(self,&kxgtkwidget_extension))

#define KXPARAM_TO_GTKWIDGET(param_name, param_id) \
	GtkWidget* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKWIDGET(tmp)) \
	{ param_name = KXGTKWIDGET_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkwidget_extension); }} 

typedef struct KxObject KxGtkWidget;

KxGtkWidget *kxgtkwidget_new_prototype(KxObject *parent);
KxObject *kxgtkwidget_from(KxCore *core, GtkWidget* data);


void kxgtkwidget_extension_init();
extern KxObjectExtension kxgtkwidget_extension;


#endif // __KX_OBJECT_GTKWIDGET_H
