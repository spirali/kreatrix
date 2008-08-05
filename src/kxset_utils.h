/*
   kxset_utils.h
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
	
#ifndef __KXSET_UTILS_H
#define __KXSET_UTILS_H


#include "utils/utils.h"
#include "kxobject_struct.h"


#define KXBASESET_DEFAULT_ARRAY_SIZE 5

typedef struct KxBaseSet KxBaseSet;


struct KxBaseSet {
	KxObject **array;
	int array_size; // size of array
	int items_count; // number of used positions in array
};


KxBaseSet * kxbaseset_new();
void kxbaseset_free(KxBaseSet *self);
void kxbaseset_clean(KxBaseSet *self);
void kxbaseset_mark(KxBaseSet *self);
KxBaseSet * kxbaseset_copy(KxBaseSet *self);



int kxbaseset_add(KxBaseSet *self, KxObject *object);
int kxbaseset_contains(KxBaseSet *self, KxObject *object);
int kxbaseset_remove(KxBaseSet *self, KxObject *object);



// DEBUG
void kxbaseset_dump(KxBaseSet *self);


#endif 
