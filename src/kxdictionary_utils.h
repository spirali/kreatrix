/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXDICT_UTILS_H
#define __KXDICT_UTILS_H


#include "utils/utils.h"
#include "kxobject_struct.h"
#include "kxset_utils.h"


#define KXBASEDICTIONARY_DEFAULT_ARRAY_SIZE 5

typedef struct KxBaseSet KxBaseDictionary;

KxBaseDictionary * kxbasedictionary_new();
void kxbasedictionary_free(KxBaseDictionary *self);
void kxbasedictionary_clean(KxBaseDictionary *self);
void kxbasedictionary_mark(KxBaseDictionary *self);
KxBaseDictionary * kxbasedictionary_copy(KxBaseDictionary *self);



int kxbasedictionary_add(KxBaseDictionary *self, KxObject *object);
KxObject* kxbasedictionary_at(KxBaseDictionary *self, KxObject *key);
int kxbasedictionary_remove(KxBaseDictionary *self, KxObject *object);

#endif 
