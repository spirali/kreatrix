/*************************
 *
 *  %Name%
 *  Generated file by kxgtk generator
 */

#ifndef __KX_OBJECT_%NAME%_H
#define __KX_OBJECT_%NAME%_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KX%NAME%_DATA(self) ((%ctype%) (self)->data.ptr)

#define KX%NAME%(data) kx%name%_from(KXCORE, (data))

#define IS_KX%NAME%(self) (kxobject_check_type(self,&kx%name%_extension))

#define KXPARAM_TO_%NAME%(param_name, param_id) \
	%ctype% param_name; { KxObject *tmp = message->params[param_id]; if (IS_KX%NAME%(tmp)) \
	{ param_name = KX%NAME%_DATA(tmp); } else { return kxobject_type_error(tmp,&kx%name%_extension, param_id); }} 

#define KXPARAM_TO_%NAME%_OR_NULL(param_name, param_id) \
	%ctype% param_name; { KxObject *tmp = message->params[param_id]; \
	if(tmp == KXCORE->object_nil) { param_name = NULL; } else if (IS_KX%NAME%(tmp)) \
	{ param_name = KX%NAME%_DATA(tmp); } else { return kxobject_type_error(tmp,&kx%name%_extension, param_id); }} 


typedef struct KxObject Kx%Name%;

Kx%Name% *kx%name%_new_prototype(KxObject *parent);
KxObject *kx%name%_from(KxCore *core, %ctype% data);


void kx%name%_extension_init();
extern KxObjectExtension kx%name%_extension;


#endif // __KX_OBJECT_%NAME%_H
