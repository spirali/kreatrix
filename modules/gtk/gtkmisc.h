/*************************
 *
 *  GtkMisc
 *
 */

#ifndef __KX_OBJECT_GTKMISC_H
#define __KX_OBJECT_GTKMISC_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGTKMISC_DATA(self) ((GtkMisc*) (self)->data.ptr)

#define KXGTKMISC(data) kxgtkmisc_from(KXCORE, (data))

#define IS_KXGTKMISC(self) (kxobject_check_type(self,&kxgtkmisc_extension))

#define KXPARAM_TO_GTKMISC(param_name, param_id) \
	GtkMisc* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGTKMISC(tmp)) \
	{ param_name = KXGTKMISC_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgtkmisc_extension); }} 

typedef struct KxObject KxGtkMisc;

KxGtkMisc *kxgtkmisc_new_prototype(KxObject *parent);
KxObject *kxgtkmisc_from(KxCore *core, GtkMisc* data);


void kxgtkmisc_extension_init();
extern KxObjectExtension kxgtkmisc_extension;


#endif // __KX_OBJECT_GTKMISC_H
