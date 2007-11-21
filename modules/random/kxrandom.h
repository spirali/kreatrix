/*************************
 *
 *  Random
 *
 */

#ifndef __KX_OBJECT_RANDOM_H
#define __KX_OBJECT_RANDOM_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXRANDOM_DATA(self) ((nic*) (self)->data.ptr)

#define IS_KXRANDOM(self) ((self)->extension == &kxrandom_extension)

typedef struct KxObject KxRandom;


KxRandom *kxrandom_new_prototype(KxCore *core);


void kxrandom_extension_init();
extern KxObjectExtension kxrandom_extension;


#endif // __KX_OBJECT_RANDOM_H
