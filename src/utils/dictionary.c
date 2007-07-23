
#include <stdlib.h>

#include "dictionary.h"
#include "list.h"

Dictionary * 
dictionary_new() 
{
	Dictionary *self = malloc(sizeof(Dictionary));
	ALLOCTEST(self);

	self->keys = list_new();
	self->values = list_new();

	return self;
}


void 
dictionary_free(Dictionary *self) 
{
	list_free(self->keys);
	list_free(self->values);
	free(self);
}


void *
dictionary_get(Dictionary *self, void *key) 
{
	int t;
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;
	for (t=0;t<size;t++) {
		if (key == keys->items[t])
			return values->items[t];
	}
	return NULL;
}

void *
dictionary_get_with_compare(Dictionary *self, void *key, CompareFcn *fcn) 
{
	int t;
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;
	for (t=0;t<size;t++) {
		if (!fcn(key, keys->items[t]))
			return values->items[t];
	}
	return NULL;
}

void dictionary_remove(Dictionary *self, void *key) 
{
	int t;
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;
	for (t=0;t<size;t++) {
		if (key == keys->items[t]) {
			list_remove(self->keys,t);
			list_remove(self->values,t);
			return;
		}
	}
}

void dictionary_remove_with_compare(Dictionary *self, void *key, CompareFcn *fcn) 
{
	int t;
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;
	for (t=0;t<size;t++) {
		if (!fcn(key,keys->items[t])) {
			list_remove(self->keys,t);
			list_remove(self->values,t);
			return;
		}
	}
}


void
dictionary_add(Dictionary *self, void *key, void *value) 
{
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;

	int t;
	for (t=0;t<size;t++) {
		if (key == keys->items[t])
			values->items[t] = value;
	}

	list_append(keys, key);
	list_append(values, value);

}


void
dictionary_add_with_compare(Dictionary *self, void *key, void *value, CompareFcn *fcn) 
{
	List *keys = self->keys;
	List *values = self->values;
	int size = keys->size;

	int t;
	for (t=0;t<size;t++) {
		if (!fcn(key, keys->items[t]))
			values->items[t] = value;
	}

	list_append(keys, key);
	list_append(values, value);

}

void
dictionary_clean(Dictionary *self)
{
	list_clean(self->values);
	list_clean(self->keys);
}

void 
dictionary_foreach_value(Dictionary *self, ForeachFcn *fcn)
{
	int t;
	list_foreach(self->values,fcn);
}

void 
dictionary_foreach_key(Dictionary *self, ForeachFcn *fcn)
{
	int t;
	list_foreach(self->keys,fcn);
}

int
dictionary_size(Dictionary *self)
{
	return list_size(self->keys);
}

Dictionary *
dictionary_copy(Dictionary *self) 
{
	Dictionary *copy = malloc(sizeof(Dictionary));
	ALLOCTEST(copy);
	copy->keys = list_copy(self->keys);
	copy->values = list_copy(self->values);
	return copy;
}

List *dictionary_keys_as_list(Dictionary *self) 
{
	return list_copy(self->keys);
}

List *dictionary_values_as_list(Dictionary *self) 
{
	return list_copy(self->values);
}
