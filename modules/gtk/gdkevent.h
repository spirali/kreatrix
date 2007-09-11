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

typedef struct KxObject KxGdkEvent;


KxGdkEvent *kxgdkevent_new_prototype(KxCore *core);


void kxgdkevent_extension_init();
extern KxObjectExtension kxgdkevent_extension;


#endif // __KX_OBJECT_GDKEVENT_H
