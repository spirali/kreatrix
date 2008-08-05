/*
   hash.h
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
