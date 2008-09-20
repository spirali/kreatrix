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
#include <string.h>

#include "kxobject_slots.h"
#include "kxobject.h"
#include "kxobject_profile.h"
#include "kxcodeblock.h"
#include "kxglobals.h"

#define KXSLOTS_GET_COUNT_POINTER(slots) ((int *) (slots))
#define KXSLOTS_GET_HASHTABLE(slots) (KxSlot *) (((int*) (slots)) + 1)

#define KXSLOTS_INIT_HASHTABLE_SIZE 11

void 
kxobject_slots_init(KxObject *self)
{
	//self->slots_capacity = 0;
}

void 
kxobject_slots_free(KxObject *self)
{
	int capacity = self->slots_capacity;
	if (capacity == 0)
		return;


	KxSlot *slot = self->slots;

	if (capacity < 0) {
		int count = 0;
		while(slot->key) {
			REF_REMOVE(slot->key);
			REF_REMOVE(slot->value);
			slot++;
			count++;
		}
		kxcore_slot_cache_put(KXCORE, count, self->slots);
	} else {
		KxSlot *slots = KXSLOTS_GET_HASHTABLE(slot);
		int t;
		for (t = 0; t < capacity; t++) {
			if (slots[t].key) {
				REF_REMOVE(slots[t].key);
				REF_REMOVE(slots[t].value);
			}
		}
		kxfree(slot);
	
	}

}

void 
kxobject_slots_clean(KxObject *self)
{
	kxobject_slots_free(self);
	self->slots_capacity = 0;
}

void
kxobject_slots_copy(KxObject *self, KxObject *copy) 
{
	int capacity = self->slots_capacity;

	if (capacity == 0) {
		return;
	}

	if (capacity < 0) {
		KxSlot *slot = self->slots;
		while(slot->key) {
			REF_ADD(slot->key);
			REF_ADD(slot->value);
			kxobject_slot_add(copy, slot->key, slot->value,slot->flags);
			slot++;
		}
	} else {
		int t;
		KxSlot *slots = KXSLOTS_GET_HASHTABLE(self->slots);
		for (t=0; t < capacity; t++) {
			if (slots[t].key) {
				REF_ADD(slots[t].key);
				REF_ADD(slots[t].value);
				kxobject_slot_add(copy, slots[t].key, slots[t].value, slots[t].flags);
			}
		}
	}
}

/*
static void dump(KxObject *self) {
	int capacity = self->slots_capacity;

	printf("cap = %i %p\n", capacity, self);

	if (capacity == 0) {
		return;
	}

	if (capacity > 0) {
		int *s = (int*) self->slots;
		KxSlot *slots = (KxSlot*)(s + 1);

		int t;

		printf("%i %i\n", self->slots_capacity, *s);
		for (t = 0; t < capacity; t++) {
			if (slots[t].key) {
				printf("%i| %lu\n", t, (unsigned long) slots[t].key);
			} else {
				printf("%i| NULL\n", t);
			}
		}
		return;
	}

	KxSlot *slot = self->slots;
	int t = 0;
	while(slot->key) {
		printf("%i> %lu\n", t, (unsigned long) slot->key);
		slot++;
		t++;
	}

}*/

KxSlot *
kxobject_slot_find(KxObject *self, KxObject *key) 
{
	int capacity = self->slots_capacity;

	if (capacity == 0) 
		return NULL;

	if (capacity < 0) {

		KxSlot *slot = self->slots;

		while(slot->key) {
			if (slot->key == key)
				return slot;
			slot++;
		}

		return NULL;
	} else {
		int index = ((unsigned long) key) % capacity;
		
		int t;

		KxSlot *slots = KXSLOTS_GET_HASHTABLE(self->slots);

		for (t = index; t < capacity; t++) {
			if (slots[t].key == key) {
				return &slots[t];
			}
			if (slots[t].key == NULL) {
				return NULL;
			}
		}

		for (t = 0; t < index; t++) {
			if (slots[t].key == key) {
				return &slots[t];
			}
			if (slots[t].key == NULL) {
				return NULL;
			}
		}

		printf("kxobject_slots_find: bug! item or NULL not found!\n");
		abort();
	}
}

static void
kxobject_slots_grow(KxObject *self)
{

	/* Compute new size */
	unsigned long old_size = self->slots_capacity;
	unsigned long new_size = old_size * 2 + 1;
	self->slots_capacity = new_size;


	/* Alloc new table */
	int tabsize = sizeof(KxSlot) * new_size + sizeof(int);
	KxSlot* *nnewtab = kxmalloc(tabsize);
	ALLOCTEST(nnewtab);
	memset(nnewtab, 0, tabsize);

	KxSlot *oldtable = self->slots;
	/* Write new table into object */
	self->slots = (KxSlot*) nnewtab;

	/* Copy data from old table to new table */
	*KXSLOTS_GET_COUNT_POINTER(nnewtab) = *KXSLOTS_GET_COUNT_POINTER(oldtable);

	KxSlot *newtab = KXSLOTS_GET_HASHTABLE(nnewtab);
	KxSlot *slots = KXSLOTS_GET_HASHTABLE(oldtable);

	unsigned long t;
	unsigned long index;
	for (t=0;t<old_size;t++) {
		if (slots[t].key) {
			index = ((unsigned long) slots[t].key) % new_size;

			int s;
			for (s=index;s<new_size;s++) {
				if (!newtab[s].key) {
					newtab[s] = slots[t];
					break;
				}
			}
			if (s != new_size) 		
				continue;
			for (s=0;s<index-1;s++) {
				if (!newtab[s].key) {
					newtab[s] = slots[t];
					break;
				}
			}
		}
	}
	kxfree(oldtable);
}

static KxSlot *
kxslottable_slot_scan(KxSlot *slots, KxObject *key, int capacity)
{
	int index = ((unsigned long) key) % capacity;
	int t;

	for (t = index; t < capacity; t++) {
		if (slots[t].key == NULL) {
			return &slots[t];
		}
	}

	for (t = 0; t < index; t++) {
		if (slots[t].key == NULL) {
			return &slots[t];
		}
	}

	printf("kxobject_slots_scan: bug! item or NULL not found!\n");
	abort();
}

void
kxobject_slot_add_by_hash(KxObject *self, KxObject *key, KxObject *value, int flags) 
{
	int *s = KXSLOTS_GET_COUNT_POINTER(self->slots);

	int size = (*s)++;
	if ((self->slots_capacity - size) <= (self->slots_capacity / 4)) {
			kxobject_slots_grow(self);
	}

	KxSlot *slots = KXSLOTS_GET_HASHTABLE(self->slots);

	KxSlot *slot = kxslottable_slot_scan(slots, key, self->slots_capacity);

	slot->key = key;
	slot->flags = flags;
	slot->value = value;
}

void
kxobject_slot_add(KxObject *self, KxObject *key, KxObject *value, int flags) 
{
	int capacity = self->slots_capacity;

	if (capacity > 0) {
		kxobject_slot_add_by_hash(self, key, value, flags);
		return;
	};

	int count = 0;
	KxSlot *slot = self->slots;

	if (capacity != 0) {
		do {
			slot++;
			count++;
		} while(slot->key);
		
		if (count == KXOBJECT_SIMPLE_SLOTS_LIMIT) {
			KxSlot *startslot = self->slots;
			int new_size = KXSLOTS_INIT_HASHTABLE_SIZE;
			int tabsize = sizeof(KxSlot) * new_size + sizeof(int);
			int *nnewtab = kxmalloc(tabsize);
			ALLOCTEST(nnewtab);
			memset(nnewtab, 0, tabsize);
			self->slots = (KxSlot *) nnewtab;
			self->slots_capacity = new_size;

			kxobject_slot_add_by_hash(self, key, value, flags);
			
			KxSlot *slot = startslot;
			while(slot->key) {
				kxobject_slot_add_by_hash(self, slot->key, slot->value, slot->flags);
				slot++;
			}
			kxcore_slot_cache_put(KXCORE, count, startslot);
			return;
		}

		self->slots = kxcore_slot_cache_resize(KXCORE, count, count + 1, self->slots);
	} else {
		self->slots = kxcore_slot_cache_get(KXCORE, 1);
		self->slots_capacity = -1;
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
	int capacity = self->slots_capacity;

	if (capacity == 0) {
		return 0;
	}

	if (capacity > 0) {
		int *s = KXSLOTS_GET_COUNT_POINTER(self->slots);
		return *s;
	}

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


	if (self->slots_capacity == 0)
		return list;

	if (self->slots_capacity > 0) {
		int capacity = self->slots_capacity;
		KxSlot *slots = KXSLOTS_GET_HASHTABLE(self->slots);
		int t;
		int r = 0;
		for (t=0; t < capacity; t++) {
			if (slots[t].key) {
				REF_ADD(slots[t].key);
				list->items[r] = slots[t].key;
				r++;
			}
		}
		return list;
	}

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

	int capacity = self->slots_capacity;
	if (capacity == 0)
		return;

	if (capacity < 0) {
		KxSlot *slot = self->slots;
		while(slot->key) {
			kxobject_mark(slot->key);
			kxobject_mark(slot->value);
			slot++;
		}
	} else {
		KxSlot *slots = KXSLOTS_GET_HASHTABLE(self->slots);
		int t;
		for (t = 0; t < capacity; t++) {
			if (slots[t].key) {
				kxobject_mark(slots[t].key);
				kxobject_mark(slots[t].value);
			}
		}
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
