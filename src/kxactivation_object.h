/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXACTIVATION_OBJECT_H
#define __KXACTIVATION_OBJECT_H

#include "kxcore.h"

typedef struct KxObject KxActivationObject;

KxObject *kxactivationobject_new_prototype(KxCore *core);
void kxactivationobject_init_extension();
KxActivationObject *kxactivationobject_new(KxCore *core, struct KxActivation *activation);


extern KxObjectExtension kxactivationobject_extension;

#endif // __KXDYNLIB_H
