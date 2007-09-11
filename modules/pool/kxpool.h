
#ifndef __KXPOOL_H
#define __KXPOOL_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "utils/dictionary.h"

#define IS_KXPOOL(kxobject) ((kxobject)->extension == &kxpool_extension)
#define KXPOOL_DATA(kxpool) ((KxPoolData *) ((kxpool)->data.ptr))

typedef struct KxObject KxPool;

typedef struct KxPoolData KxPoolData;

struct KxPoolData {
	Dictionary *objects;
	Dictionary *paths;
};


void kxpool_extension_init();
KxObject * kxpool_new_prototype(KxCore *core);

extern KxObjectExtension kxpool_extension;

#endif // __KXPOOL_H
