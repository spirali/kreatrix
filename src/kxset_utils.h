/**********************************************************************
 *
 *	$Id$
 *
 *  Help structure for KxSet
 *
 **********************************************************************/

#ifndef __KXSET_UTILS_H
#define __KXSET_UTILS_H


#include "utils/utils.h"
#include "kxobject_struct.h"


#define KXBASESET_DEFAULT_ARRAY_SIZE 5

typedef struct KxBaseSet KxBaseSet;


struct KxBaseSet {
	KxObject **array;
	int array_size; // size of array
	int items_count; // number of used positions in array
};


KxBaseSet * kxbaseset_new();
void kxbaseset_free(KxBaseSet *self);
void kxbaseset_clean(KxBaseSet *self);
void kxbaseset_mark(KxBaseSet *self);
KxBaseSet * kxbaseset_copy(KxBaseSet *self);



int kxbaseset_add(KxBaseSet *self, KxObject *object);
int kxbaseset_contains(KxBaseSet *self, KxObject *object);
int kxbaseset_remove(KxBaseSet *self, KxObject *object);



// DEBUG
void kxbaseset_dump(KxBaseSet *self);


#endif 
