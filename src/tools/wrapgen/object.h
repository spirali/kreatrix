/*************************
 *
 *  %Name%
 *
 */

#ifndef __KX_OBJECT_%NAME%_H
#define __KX_OBJECT_%NAME%_H

#include "kreatrix/kxobject_struct.h"
#include "kreatrix/kxcore.h"

#define KX%NAME%_DATA(self) ((%data%) (self)->data.ptr)

#define KX%NAME%(data) kx%name%_new_from(KXCORE, (data))

#define IS_KX%NAME%(self) ((self)->extension == &kx%name%_extension)

#define KXPARAM_TO_%NAME%(param_name, param_id) \
	%data% param_name; { KxObject *tmp = message->params[param_id]; if (IS_KX%NAME%(tmp)) \
	{ param_name = KX%NAME%_DATA(tmp); } else { return kxobject_type_error(tmp,&kx%name%_extension); }} 

typedef struct KxObject Kx%Name%;

%header%

Kx%Name% *kx%name%_new_prototype(KxCore *core);
KxObject *kx%name%_new_from(KxCore *core, %data% data);


void kx%name%_extension_init();
extern KxObjectExtension kx%name%_extension;


#endif // __KX_OBJECT_%NAME%_H
