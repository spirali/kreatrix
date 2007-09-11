/*************************
 *
 *  GtkBin
 *
 */

#ifndef __KX_OBJECT_GTKBIN_H
#define __KX_OBJECT_GTKBIN_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKBIN_DATA(self) ((GtkBin*) (self)->data.ptr)

#define KXGTKBIN(data) kxgtkbin_from(KXCORE, (data))

#define IS_KXGTKBIN(self) (kxobject_check_type(self,&kxgtkbin_extension))

#define KXPARAM_TO_GTKBIN(param_name, param_id) \
	GtkBin* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKBIN(tmp)) \
	{ param_name = KXGTKBIN_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkbin_extension); }} 

typedef struct KxObject KxGtkBin;

KxGtkBin *kxgtkbin_new_prototype(KxObject *parent);
KxObject *kxgtkbin_from(KxCore *core, GtkBin* data);


void kxgtkbin_extension_init();
extern KxObjectExtension kxgtkbin_extension;


#endif // __KX_OBJECT_GTKBIN_H
