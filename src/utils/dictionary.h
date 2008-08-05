/*
   dictionary.h
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
	

#ifndef __DICTIONARY_H
#define __DICTIONARY_H

//tmp
#include "list.h"
#include "utils.h"

typedef struct Dictionary Dictionary;



struct Dictionary {
	List *keys;
	List *values;
};

Dictionary * dictionary_new();
void dictionary_free(Dictionary *self);


void dictionary_clean(Dictionary *self);
void dictionary_add(Dictionary *self, void *key, void *value);
void *dictionary_get(Dictionary *self, void *key);
void dictionary_remove(Dictionary *self, void *key);

void dictionary_add_with_compare(Dictionary *self, void *key, void *value, CompareFcn *fcn);
void *dictionary_get_with_compare(Dictionary *self, void *key, CompareFcn *fcn);
void dictionary_remove_with_compare(Dictionary *self, void *key, CompareFcn *fcn);


void dictionary_foreach_value(Dictionary *self, ForeachFcn *fcn);
void dictionary_foreach_key(Dictionary *self, ForeachFcn *fcn);
int dictionary_size(Dictionary *self);
Dictionary *dictionary_copy(Dictionary *self);


List *dictionary_keys_as_list(Dictionary *self);
List *dictionary_values_as_list(Dictionary *self);


#endif // __DICTIONARY_H
