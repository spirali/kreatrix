/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "kxset_utils.h"
#include "kxmessage.h"
#include "kxobject.h"

KxBaseSet * 
kxbaseset_new() 
{
	KxBaseSet *s = kxmalloc(sizeof(KxBaseSet));
	ALLOCTEST(s);

	s->array_size = KXBASESET_DEFAULT_ARRAY_SIZE;
	s->items_count = 0;

	s->array = kxcalloc(1,sizeof(void*) * KXBASESET_DEFAULT_ARRAY_SIZE);
	ALLOCTEST(s->array);
	return s;
}

void 
kxbaseset_free(KxBaseSet *self)
{
	int t;
	KxObject **array = self->array;
	for (t=0;t<self->array_size;t++) {
		if (array[t])
			REF_REMOVE(array[t]);
	}
	kxfree(array);
	kxfree(self);
}

void 
kxbaseset_clean(KxBaseSet *self)
{
	int t;
	KxObject **array = self->array;
	for (t=0;t<self->array_size;t++) {
		if (array[t]) {
			REF_REMOVE(array[t]);
			array[t] = NULL;
		}
	}
	self->items_count = 0;
}

KxBaseSet *
kxbaseset_copy(KxBaseSet *self)
{
	KxBaseSet *s = kxmalloc(sizeof(KxBaseSet));
	ALLOCTEST(s);

	s->array_size = self->array_size;
	s->items_count = self->items_count;

	s->array = kxmalloc(sizeof(void*) * self->array_size);
	ALLOCTEST(s->array);

	int t;
	for (t=0;t<self->array_size;t++) {
		s->array[t] = self->array[t];
		if (s->array[t]) {
			REF_ADD(s->array[t]);
		}
	}
	return s;
}


void 
kxbaseset_mark(KxBaseSet *self)
{
	int t;
	KxObject **array = self->array;
	for (t=0;t<self->array_size;t++) {
		if (array[t]) {
			kxobject_mark(array[t]);
		}
	}
}



static int 
kxbaseset_scan_for(KxBaseSet *self, KxObject *object, unsigned long hash)
{
	int index = hash % self->array_size;
	KxObject **array = self->array;
	int t;
	KxMessage msg;
	//msg.stack = stack;
	KxSymbol *msg_eq = KXCORE_FROM(object)->dictionary[KXDICT_EQ];
	KxObject *true_obj = KXCORE_FROM(object)->object_true; 
	for (t = index; t < self->array_size; t++) 	{
		if (!array[t])
			return t;
		kxmessage_init(&msg, object, 1, msg_eq);
		msg.params[0] = array[t];
		REF_ADD(array[t]);
		KxObject *ret = kxmessage_send(&msg);
		if (ret == NULL)
			return -1;
		REF_REMOVE(ret);
		if (ret == true_obj)
			return t;
	}

	for (t = 0; t < index; t++) 	{
		if (!array[t])
			return t;
		kxmessage_init(&msg, object, 1, msg_eq);
		msg.params[0] = array[t];
		REF_ADD(array[t]);
		KxObject *ret = kxmessage_send(&msg);
		if (ret == NULL)
			return -1;
		REF_REMOVE(ret);
		if (ret == true_obj)
			return t;

	}
	
	printf("KxBaseSet: bug! item or NULL not found!\n");
	kxbaseset_dump(self);
	abort();
}


static int 
kxbaseset_grow(KxBaseSet *self)
{
	int old_size = self->array_size;
	int new_size = old_size * 2 + 1;
	KxObject **new_array = kxcalloc(1,new_size * sizeof(KxObject*));
	ALLOCTEST(new_array);

	KxObject **array = self->array;
	int t;

	unsigned long hash;
	int index;
	for (t=0;t<old_size;t++) {
		if (array[t]) {
			if (!kxobject_get_hash(array[t],&hash)) {
				kxfree(new_array);
				return 0;
			}
			index = hash % new_size;

			int s;
			for (s=index;s<new_size;s++) {
				if (!new_array[s]) {
					new_array[s] = array[t];
					break;
				}
			}
			if (s != new_size) 		
				continue;
			for (s=0;s<index-1;s++) {
				if (!new_array[s]) {
					new_array[s] = array[t];
					break;
				}
			}
		}
	}
	kxfree(array);
	self->array = new_array;
	self->array_size = new_size;
	return 1;
}

int
kxbaseset_add(KxBaseSet *self, KxObject *object)
{
	if (self->array_size - self->items_count < (MAX(self->array_size / 4,2))) {
		kxbaseset_grow(self);
	}
	unsigned long hash;
	if (!kxobject_get_hash(object, &hash))
		return 0;
	int index = kxbaseset_scan_for(self, object, hash);
	if (index == -1)
		return 0;
	if (!self->array[index]) {
		self->array[index] = object;
		self->items_count++;
		REF_ADD(object);
	}
	return 1;
}

/**
	Returns 
	-1 - error
	 0 - not found
	 1 - found
*/
int
kxbaseset_contains(KxBaseSet *self, KxObject *object)
{
	unsigned long hash;
	if (!kxobject_get_hash(object, &hash))
		return -1;
	int index = kxbaseset_scan_for(self, object, hash);
	if (index == -1)
		return -1;
	return self->array[index]?1:0;
}

static int
kxbaseset_fix_after_remove(KxBaseSet *self, int index)
{
	//int old_pos = index;
	KxObject **items = self->array;
	unsigned long hash;

	index++;
	index %= self->array_size;

	while(items[index]) {

		if (!kxobject_get_hash(items[index], &hash))
			return 0;
		int new_index = kxbaseset_scan_for(self, items[index], hash);
		if (new_index != index) {
			KxObject *obj = items[index];
			items[index] = items[new_index];
			items[new_index] = obj;
		}
		index++;
		index %= self->array_size;
	}
	return 1;
}

/**
	Returns 
	-1 - error
	 0 - not found
	 1 - found
*/
int
kxbaseset_remove(KxBaseSet *self, KxObject *object)
{
	unsigned long hash;
	if (!kxobject_get_hash(object, &hash))
		return -1;
	int index = kxbaseset_scan_for(self, object, hash);

	if (!self->array[index]) {
		return 0;
	}

	REF_REMOVE(self->array[index]);
	self->array[index] = NULL;

	self->items_count--;
	if (self->items_count > 0) {
		if (!kxbaseset_fix_after_remove(self, index))
			return -1;
	}
	return 1;
}

void
kxbaseset_dump(KxBaseSet *self)
{
	int t;
	printf("SET:\n");
	for (t=0;t<self->array_size;t++) {
		if (self->array[t]) {
			printf("%i : ",t);
			kxobject_dump(self->array[t]);
		} else {
			printf("%i : NULL\n",t);
		}
	}
}
