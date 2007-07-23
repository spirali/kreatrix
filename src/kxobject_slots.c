#include <stdlib.h>

#include "kxobject_slots.h"
#include "kxobject.h"

#define KXSLOTS_DEFAULT_SIZE 4

/***
 *   For simple implementation is now slots implement as simple list,
 *   It should be implemented as hashtables, but it need same benchmarks
 */ 

void 
kxobject_slots_init(KxObject *self)
{
	self->slots_capacity = KXSLOTS_DEFAULT_SIZE;
	self->slots = malloc(sizeof(KxSlot) * KXSLOTS_DEFAULT_SIZE);
	ALLOCTEST(self->slots);
}

void 
kxobject_slots_free(KxObject *self)
{
	int t;
	int size = self->slots_count;
	self->slots_count = 0;
	self->slots_capacity = 0;
	KxSlot *slots = self->slots;

	for (t=0;t<size;t++) {
		REF_REMOVE(slots[t].key);
		REF_REMOVE(slots[t].value);
	}
	free(slots);
}

void 
kxobject_slots_clean(KxObject *self)
{
	int t;
	int size = self->slots_count;
	KxSlot *slots = self->slots;

	self->slots_count = 0;
	self->slots_capacity = KXSLOTS_DEFAULT_SIZE;
	self->slots = malloc(sizeof(KxSlot) * KXSLOTS_DEFAULT_SIZE);

	for (t=0;t<size;t++) {
		REF_REMOVE(slots[t].key);
		REF_REMOVE(slots[t].value);
	}
	free(slots);
}



void
kxobject_slots_copy(KxObject *self, KxObject *copy) 
{
	/*List *list = hashtable_keys_to_list(self->slots);

	HashTable *source = self->slots;
	HashTable *out = copy->slots;

	LIST_FOREACH(list) {
		REF_ADD((KxObject*)LIST_EACH);
		KxObject *obj = hashtable_get(source, LIST_EACH);
		REF_ADD(obj);
		hashtable_add(out, LIST_EACH, obj);
	}

	list_free(list);*/

	KxSlot *source = self->slots;
	int t;
	for (t=0;t<self->slots_count;++t) {
		REF_ADD(source[t].key);
		REF_ADD(source[t].value);
		kxobject_slot_add(copy, source[t].key, source[t].value,source[t].flags);
	}
}


KxSlot *
kxobject_slot_find(KxObject *self, KxObject *key) 
{
	int t;
	KxSlot *slots = self->slots;
	for (t=0;t<self->slots_count;t++) {
		if (slots[t].key == key)
			return &slots[t];
	}
	return NULL;
}

void
kxobject_slot_add(KxObject *self, KxObject *key, KxObject *value, int flags) 
{
	//KxSlot *slots = self->slots;
	if (self->slots_capacity == self->slots_count) {
		self->slots_capacity <<= 1;
		self->slots = realloc(self->slots, self->slots_capacity * sizeof(KxSlot));
		ALLOCTEST(self->slots);
	}
	self->slots[self->slots_count].key = key;
	self->slots[self->slots_count].flags = flags;
	self->slots[self->slots_count++].value = value;
}

List *
kxobject_slots_to_list(KxObject *self) {
	/*List *list = hashtable_keys_to_list(self->slots);
	list_foreach(list, kxobject_ref_add);
	return list;*/
	List *list = list_new_size(self->slots_count);
	int t;
	for (t=0;t<self->slots_count;t++) {
		REF_ADD(self->slots[t].key);
		list->items[t] = self->slots[t].key;
	}
	return list;
}

void
kxobject_slots_mark(KxObject *self)
{
	int t;
	int size = self->slots_count;
	KxSlot *slots = self->slots;

	for (t=0;t<size;t++) {
		kxobject_mark(slots[t].key);
		kxobject_mark(slots[t].value);
	}
}
