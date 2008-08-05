/*
   dictionary.c
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

#include "dictionary.h"
#include "list.h"

Dictionary * 
dictionary_new() 
{
	Dictionary *self = kxmalloc(sizeof(Dictionary));
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
	kxfree(self);
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
	Dictionary *copy = kxmalloc(sizeof(Dictionary));
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
