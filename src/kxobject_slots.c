/*
   kxobject_slots.c
   Copyright (C) 2007, 2008  Stanislav Bohm

   This file is part of Kreatrix.

   Kreatrix is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Kreatrix is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Kreatrix; see the file COPYING. If not, see 
   <http://www.gnu.org/licenses/>.
*/
	
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
	self->slots = NULL;
	/*self->slots_capacity = KXSLOTS_DEFAULT_SIZE;
	self->slots = kxmalloc(sizeof(KxSlot) * (KXSLOTS_DEFAULT_SIZE));
	self->slots[0].key = NULL;
	ALLOCTEST(self->slots);*/
}

void 
kxobject_slots_free(KxObject *self)
{
	KxSlot *slot = self->slots;

	if (slot == NULL)
		return;

	int count = 0;
	while(slot->key) {
		REF_REMOVE(slot->key);
		REF_REMOVE(slot->value);
		slot++;
		count++;
	}

	//kxfree(self->slots);
	
	kxcore_slot_cache_put(KXCORE, count, self->slots);
}

void 
kxobject_slots_clean(KxObject *self)
{
	kxobject_slots_free(self);
	self->slots = NULL;
}



void
kxobject_slots_copy(KxObject *self, KxObject *copy) 
{
	if (self->slots == NULL) {
		copy->slots = NULL;
		return;
	}

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

	if (slot == NULL)
		return NULL;

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
	int count = 0;
	KxSlot *slot = self->slots;

	if (slot) {
		while(slot->key) {
			slot++;
			count++;
		}
		self->slots = kxcore_slot_cache_resize(KXCORE, count, count + 1, self->slots);
	} else {
		self->slots = kxcore_slot_cache_get(KXCORE, 1);
	}

	slot = &self->slots[count];

	slot->key = key;
	slot->flags = flags;
	slot->value = value;

	slot++;
	
	slot->key = NULL;

}

int 
kxobject_slots_count(KxObject *self) {
	KxSlot *slot = self->slots;

	if (slot == NULL) 
		return 0;

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
	if (slot == NULL)
		return list;

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

	if (slot == NULL)
		return;

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
