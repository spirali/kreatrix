/*************************
 *
 *  GdkGeometry
 *
 */

#ifndef __KX_OBJECT_GDKGEOMETRY_H
#define __KX_OBJECT_GDKGEOMETRY_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGDKGEOMETRY_DATA(self) ((GdkGeometry*) (self)->data.ptr)

#define KXGDKGEOMETRY(data) kxgdkgeometry_from(KXCORE, (data))

#define IS_KXGDKGEOMETRY(self) (kxobject_check_type(self,&kxgdkgeometry_extension))

#define KXPARAM_TO_GDKGEOMETRY(param_name, param_id) \
	GdkGeometry* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGDKGEOMETRY(tmp)) \
	{ param_name = KXGDKGEOMETRY_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgdkgeometry_extension); }} 

typedef struct KxObject KxGdkGeometry;

KxGdkGeometry *kxgdkgeometry_new_prototype(KxObject *parent);
KxObject *kxgdkgeometry_from(KxCore *core, GdkGeometry* data);


void kxgdkgeometry_extension_init();
extern KxObjectExtension kxgdkgeometry_extension;


#endif // __KX_OBJECT_GDKGEOMETRY_H
