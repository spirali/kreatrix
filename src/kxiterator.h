/*************************
 *
 *  Iterator
 *
 *  Object is now used only for intern usage in KXCI_FOREACH
 */

#ifndef __KX_OBJECT_ITERATOR_H
#define __KX_OBJECT_ITERATOR_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXITERATOR_DATA(self) ((KxIteratorData*) (self)->data.ptr)

#define IS_KXITERATOR(self) ((self)->extension == &kxiterator_extension)

typedef struct KxObject KxIterator;
typedef struct KxIteratorData KxIteratorData;
typedef struct KxIteratorDataInt KxIteratorDataInt;

struct KxIteratorData {
	KxObject *object;
};

struct KxIteratorDataInt {
	KxObject *object;
	int position;
};

KxIterator *kxiterator_new_prototype(KxCore *core);
KxObject *kxiterator_create(KxCore *core, KxIteratorData *data);


void kxiterator_init_extension();
extern KxObjectExtension kxiterator_extension;


#endif // __KX_OBJECT_ITERATOR_H
