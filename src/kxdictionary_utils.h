/*
   kxdictionary_utils.h
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
	
#ifndef __KXDICT_UTILS_H
#define __KXDICT_UTILS_H


#include "utils/utils.h"
#include "kxobject_struct.h"
#include "kxset_utils.h"


#define KXBASEDICTIONARY_DEFAULT_ARRAY_SIZE 5

typedef struct KxBaseSet KxBaseDictionary;

KxBaseDictionary * kxbasedictionary_new();
void kxbasedictionary_free(KxBaseDictionary *self);
void kxbasedictionary_clean(KxBaseDictionary *self);
void kxbasedictionary_mark(KxBaseDictionary *self);
KxBaseDictionary * kxbasedictionary_copy(KxBaseDictionary *self);



int kxbasedictionary_add(KxBaseDictionary *self, KxObject *object);
KxObject* kxbasedictionary_at(KxBaseDictionary *self, KxObject *key);
int kxbasedictionary_remove(KxBaseDictionary *self, KxObject *object);

#endif 
