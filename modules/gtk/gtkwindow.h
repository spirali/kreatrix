/*************************
 *
 *  GtkWindow
 *
 */

#ifndef __KX_OBJECT_GTKWINDOW_H
#define __KX_OBJECT_GTKWINDOW_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKWINDOW_DATA(self) ((GtkWindow*) (self)->data.ptr)

#define KXGTKWINDOW(data) kxgtkwindow_from(KXCORE, (data))

#define IS_KXGTKWINDOW(self) (kxobject_check_type(self,&kxgtkwindow_extension))

#define KXPARAM_TO_GTKWINDOW(param_name, param_id) \
	GtkWindow* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKWINDOW(tmp)) \
	{ param_name = KXGTKWINDOW_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkwindow_extension); }} 

typedef struct KxObject KxGtkWindow;

KxGtkWindow *kxgtkwindow_new_prototype(KxObject *parent);
KxObject *kxgtkwindow_from(KxCore *core, GtkWindow* data);


void kxgtkwindow_extension_init();
extern KxObjectExtension kxgtkwindow_extension;


#endif // __KX_OBJECT_GTKWINDOW_H
