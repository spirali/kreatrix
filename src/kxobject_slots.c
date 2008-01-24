#include <stdlib.h>
#include <stdio.h>

#include "kxobject_slots.h"
#include "kxobject.h"

#define KXSLOTS_DEFAULT_SIZE 5

/***
 *   Slots is now implemented as simple list due to simple implementation,
 *   It should be implemented as hashtables, but it need some benchmarks
 */ 

void 
kxobject_slots_init(KxObject *self)
{
	self->slots_capacity = KXSLOTS_DEFAULT_SIZE;
	self->slots = kxmalloc(sizeof(KxSlot) * (KXSLOTS_DEFAULT_SIZE));
	self->slots[0].key = NULL;
	ALLOCTEST(self->slots);
}

void 
kxobject_slots_free(KxObject *self)
{
	self->slots_capacity = 0;
	KxSlot *slot = self->slots;

	/*if (slot == NULL)
		return;*/

	while(slot->key) {
		REF_REMOVE(slot->key);
		REF_REMOVE(slot->value);
		slot++;
	}

	kxfree(self->slots);
}

void 
kxobject_slots_clean(KxObject *self)
{
	kxobject_slots_free(self);
	//self->slots = NULL;

	self->slots_capacity = KXSLOTS_DEFAULT_SIZE;
	self->slots = kxmalloc(sizeof(KxSlot) * (KXSLOTS_DEFAULT_SIZE));

	self->slots[0].key = NULL;
/*
	for (t=0;t<size;t++) {
		REF_REMOVE(slots[t].key);
		REF_REMOVE(slots[t].value);
	}
	kxfree(slots);*/
}



void
kxobject_slots_copy(KxObject *self, KxObject *copy) 
{
	/*if (self->slots == NULL) {
		copy->slots = NULL;
		return;
	}*/
	/*KxSlot *source = self->slots;
	int t;
	for (t=0;t<self->slots_count;++t) {
		REF_ADD(source[t].key);
		REF_ADD(source[t].value);
		kxobject_slot_add(copy, source[t].key, source[t].value,source[t].flags);
	}*/
	KxSlot *slot = self->slots;
	while(slot->key) {
		REF_ADD(slot->key);
		REF_ADD(slot->value);
		kxobject_slot_add(copy, slot->key, slot->value,slot->flags);
		slot++;
	}
}


KxSlot *
kxobject_slot_find(KxObject *self, KxObject *key) 
{
	KxSlot *slot = self->slots;

	while(slot->key) {
		if (slot->key == key)
			return slot;
		slot++;
	}

	return NULL;
}

void
kxobject_slot_add(KxObject *self, KxObject *key, KxObject *value, int flags) 
{
	int count = 1;
	KxSlot *slot = self->slots;

	while(slot->key) {
		slot++;
		count++;
	}

	if (self->slots_capacity == count) {
		KxSlot *old = self->slots;
		self->slots_capacity <<= 1;
		self->slots = kxrealloc(self->slots, ( (self->slots_capacity) * sizeof(KxSlot)));
		if (self->slots != old) {
			ALLOCTEST(self->slots);

			slot = self->slots;
			while(slot->key) {
				slot++;
			}
		}
	}
	slot->key = key;
	slot->flags = flags;
	slot->value = value;

	slot++;
	
	slot->key = NULL;

}

int 
kxobject_slots_count(KxObject *self) {
	KxSlot *slot = self->slots;
	int count = 0;
	while(slot->key) {
		slot++;
		count++;
	}
	return count;
}

List *
kxobject_slots_to_list(KxObject *self) {

	List *list = list_new_size(kxobject_slots_count(self));

	KxSlot *slot = self->slots;

	int t;
	t = 0;
	while(slot->key) {
		REF_ADD(slot->key);
		list->items[t] = slot->key;
		slot++;
		t++;
	}
	return list;
}

void
kxobject_slots_mark(KxObject *self)
{
	KxSlot *slot = self->slots;

	while(slot->key) {
		kxobject_mark(slot->key);
		kxobject_mark(slot->value);
		slot++;
	}
}

void
kxobject_slots_dump(KxObject *self) 
{
	/*printf("Slots for:");
	kxobject_dump(self);
	
	int t;
	for (t=0;t<self->slots_count;t++)
	{
		printf("%s -> ", (char*)(self->slots[t].key->data.ptr));
		kxobject_dump(self->slots[t].value);
	}*/
}
