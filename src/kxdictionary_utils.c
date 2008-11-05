/*
   kxdictionary_utils.c
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
	
#include <stdio.h>
#include <stdlib.h>

#include "kxdictionary_utils.h"
#include "kxset_utils.h"
#include "kxmessage.h"
#include "kxobject.h"
#include "kxassociation.h"

KxBaseDictionary * 
kxbasedictionary_new() 
{
	return kxbaseset_new();
}

void 
kxbasedictionary_free(KxBaseDictionary *self)
{
	kxbaseset_free(self);
}

void 
kxbasedictionary_clean(KxBaseDictionary *self)
{
	kxbaseset_clean(self);
}

KxBaseDictionary *
kxbasedictionary_copy(KxBaseDictionary *self)
{
	return kxbaseset_copy(self);
}


void 
kxbasedictionary_mark(KxBaseDictionary *self)
{
	kxbaseset_mark(self);
}

static int
kxassoc_get_hash_and_key(KxObject *self, KxObject **key,  unsigned long *hash) {
	KxObject *obj = KXASSOCIATION_GET_KEY(self);
	*key = obj;
	int r = kxobject_get_hash(obj, hash);
	if (!r) {
		return 0;
	}
	return 1;
}

static int
kxassoc_get_hash(KxObject *self, unsigned long *hash) {
	KxObject *obj =	KXASSOCIATION_GET_KEY(self);
	int r = kxobject_get_hash(obj, hash);
	return r;
}

static int 
kxbasedictionary_scan_for_key(KxBaseDictionary *self, KxObject *key, unsigned long hash)
{
	int index = hash % self->array_size;
	KxObject **array = self->array;
	int t;
	KxMessage msg;
	//msg.stack = stack;
	KxSymbol *msg_eq = KXCORE_FROM(key)->dictionary[KXDICT_EQ];
	KxObject *true_obj = KXCORE_FROM(key)->object_true; 
	

	if (key == NULL) {
		return -1;
	}

	for (t = index; t < self->array_size; t++) 	{
		if (!array[t]) {
			return t;
		}

		KxObject *key2 = KXASSOCIATION_GET_KEY(array[t]);
		REF_ADD(key2);
		
		kxmessage_init(&msg, key, 1, msg_eq);
		msg.params[0] = key2;
		KxObject *ret = kxmessage_send(&msg);
		if (ret == NULL) {
			return -1;
		}
		REF_REMOVE(ret);
	//	printf("x------x\n");
		if (ret == true_obj) {
			return t;
		}
	}

	for (t = 0; t < index; t++) 	{
		if (!array[t]) {
			return t;
		}

		KxObject *key2 = KXASSOCIATION_GET_KEY(array[t]);
		REF_ADD(key2);

		kxmessage_init(&msg, key, 1, msg_eq);
		msg.params[0] = key2;

		KxObject *ret = kxmessage_send(&msg);
		if (ret == NULL) {
			return -1;
		}
		REF_REMOVE(ret);
		if (ret == true_obj) {
			return t;
		}
	}

	printf("KxBaseDictionary: bug! item or NULL not found! %p %i/%i %li\n",
		self, self->array_size, self->items_count, hash);
	printf("index = %i\n", index);
	for (t=0;t<self->array_size;t++) {
		unsigned long hash;
		KxObject *key;
		if (array[t]) {
			kxassoc_get_hash_and_key(array[t], &key, &hash);
			printf("%i - %p %p hash=%li\n", t, array[t], key, hash);
		} else {
			printf("%i - null\n", t);
		}
	}

	abort();
}

static int 
kxbasedictionary_grow(KxBaseDictionary *self)
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
			if (!kxassoc_get_hash(array[t], &hash)) {
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
kxbasedictionary_add(KxBaseDictionary *self, KxObject *object)
{
	if (self->array_size - self->items_count < (MAX(self->array_size / 4,2))) {
		kxbasedictionary_grow(self);
	}
	unsigned long hash;
	KxObject *key;
	if (!kxassoc_get_hash_and_key(object, &key, &hash))
		return 0;
//	printf("add dict self=%p obj=%p key=%p hash=%li\n",self, object, key, hash);
	int index = kxbasedictionary_scan_for_key(self, key, hash);

	if (index == -1)
		return 0;
	if (!self->array[index]) {
		self->array[index] = object;
		self->items_count++;
		REF_ADD(object);
	} else {
		REF_ADD(object);
		REF_REMOVE(self->array[index]);
		self->array[index] = object;
	}
	return 1;
}

/**
	Returns 
	-1 - error
	 0 - not found
	 1 - found
*/
KxObject *
kxbasedictionary_at(KxBaseDictionary *self, KxObject *key)
{
	unsigned long hash;
	if (!kxobject_get_hash(key, &hash))
		return NULL;
	//printf("at** %p %p %li\n", self, key, hash);
	int index = kxbasedictionary_scan_for_key(self, key, hash);
	if (index == -1)
		return NULL;
	if (self->array[index]) {
		return self->array[index];
	} else {
		return KXCORE_FROM(key)->object_nil;
	}
}

static int
kxbasedictionary_fix_after_remove(KxBaseDictionary *self, int index)
{
	//int old_pos = index;
	KxObject **items = self->array;
	unsigned long hash;

	index++;
	index %= self->array_size;

	while(items[index]) {
		KxObject *key;
		if (!kxassoc_get_hash_and_key(items[index], &key, &hash))
			return 0;

//		printf("fixafter**\n");
		int new_index = kxbasedictionary_scan_for_key(self, key, hash);
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
kxbasedictionary_remove(KxBaseDictionary *self, KxObject *key)
{
	unsigned long hash;
	if (!kxobject_get_hash(key, &hash))
		return -1;

	//printf("remove**\n");
	int index = kxbasedictionary_scan_for_key(self, key, hash);

	if (!self->array[index]) {
		return 0;
	}

	REF_REMOVE(self->array[index]);
	self->array[index] = NULL;

	self->items_count--;
	if (self->items_count > 0) {
		if (!kxbasedictionary_fix_after_remove(self, index))
			return -1;
	}
	return 1;
}


