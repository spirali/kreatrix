/*************************
 *
 *  GdkEvent
 *
 */

#ifndef __KX_OBJECT_GDKEVENT_H
#define __KX_OBJECT_GDKEVENT_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXGDKEVENT(self) ((self)->extension == &kxgdkevent_extension)

#define KXGDKEVENT_DATA(self) ((GdkEvent*) self->data.ptr)

#define KXPARAM_TO_GDKEVENT(param_name, param_id) \
	GdkEvent* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGDKEVENT(tmp)) \
	{ param_name = KXGDKEVENT_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgdkevent_extension); }} 


typedef struct KxObject KxGdkEvent;


KxGdkEvent *kxgdkevent_new_prototype(KxCore *core);


void kxgdkevent_extension_init();
extern KxObjectExtension kxgdkevent_extension;


#endif // __KX_OBJECT_GDKEVENT_H
