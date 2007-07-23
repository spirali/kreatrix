
#ifndef __HASH_H
#define __HASH_H

// TMP
#include "list.h"

typedef struct HashTable HashTable;

typedef void(HashForeachFcn)(void*);

struct HashTable {
	List * keys;
	List * values;
};

HashTable * hashtable_new();
void hashtable_free(HashTable *hashtable);
void hashtable_foreach_key(HashTable *hashtable, HashForeachFcn *fcn);

HashTable * hashtable_copy(HashTable *self);

void *hashtable_get(HashTable *hashtable, void *key);
void hashtable_add(HashTable *hashtable, void *key, void *value);

void hashtable_foreach(HashTable *hashtable, HashForeachFcn *fcn);

void hashtable_clean(HashTable *hashtable);

int hashtable_has_key(HashTable *hashtable, void *key);

List * hashtable_keys_to_list(HashTable *hashtable);

HashTable * hashtable_copy(HashTable *self);

int hashtable_size(HashTable *self);


#endif // __HASH_H
