/*
   kxiterator.h
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

#ifndef __KX_OBJECT_ITERATOR_H
#define __KX_OBJECT_ITERATOR_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXITERATOR_DATA(self) ((KxIteratorData*) (self)->data.ptr)

#define IS_KXITERATOR(self) ((self)->extension == &kxiterator_extension)

typedef struct KxObject KxIterator;
typedef struct KxIteratorData KxIteratorData;
typedef struct KxIteratorDataInt KxIteratorDataInt;
typedef struct KxIteratorDataPtr KxIteratorDataPtr;

struct KxIteratorData {
	KxObject *object;
};

struct KxIteratorDataInt {
	KxObject *object;
	int position;
};

struct KxIteratorDataPtr {
	KxObject *object;
	char *ptr;
};


KxIterator *kxiterator_new_prototype(KxCore *core);
KxObject *kxiterator_create(KxCore *core, KxIteratorData *data);


void kxiterator_init_extension();
extern KxObjectExtension kxiterator_extension;


#endif // __KX_OBJECT_ITERATOR_H
