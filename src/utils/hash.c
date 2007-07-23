
#include <stdlib.h>
#include <stdio.h>
#include "hash.h"
#include "utils.h"

HashTable * 
hashtable_new() 
{
	HashTable *table = malloc(sizeof(HashTable));
	ALLOCTEST(table);
	
	table->keys = list_new();
	table->values = list_new();

	return table;
}

void
hashtable_free(HashTable *hashtable) 
{
	list_free(hashtable->keys);
	list_free(hashtable->values);
	free(hashtable);
}

void
hashtable_add(HashTable *hashtable, void *key, void *value) 
{
	int t;
	for (t=0;t<hashtable->keys->size;t++) {
		if (hashtable->keys->items[t] == key) {
			hashtable->values->items[t] = value;
			return;
		}
	}
	list_append(hashtable->keys,key);
	list_append(hashtable->values,value);
}

void *
hashtable_get(HashTable *hashtable, void *key) 
{
	int t;
	for (t=0;t<hashtable->keys->size;t++) {
		if (hashtable->keys->items[t] == key) {
			return hashtable->values->items[t];
		}
	}
	return NULL;
}

int
hashtable_has_key(HashTable *hashtable, void *key) 
{
	int t;
	for (t=0;t<hashtable->keys->size;t++) {
		if (hashtable->keys->items[t] == key) {
			return 1;
		}
	}
	return 0;
}

void hashtable_clean(HashTable *hashtable)
{
	list_clean(hashtable->keys);
	list_clean(hashtable->values);
}


void 
hashtable_foreach(HashTable *hashtable, HashForeachFcn *fcn)
{
	list_foreach(hashtable->values, fcn);
}

void 
hashtable_foreach_key(HashTable *hashtable, HashForeachFcn *fcn)
{
	list_foreach(hashtable->keys, fcn);
}

List * 
hashtable_keys_to_list(HashTable *hashtable)
{
	return list_copy(hashtable->keys);
}


HashTable * 
hashtable_copy(HashTable *self) 
{
	HashTable *copy = malloc(sizeof(HashTable));
	ALLOCTEST(copy);

	copy->keys = list_copy(self->keys);
	copy->values = list_copy(self->values);

	return copy;
}

int 
hashtable_size(HashTable *self) 
{
	return self->keys->size;
}
