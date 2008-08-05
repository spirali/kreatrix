/*
   kxpool.h
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
	

#ifndef __KXPOOL_H
#define __KXPOOL_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "utils/dictionary.h"

#define IS_KXPOOL(kxobject) ((kxobject)->extension == &kxpool_extension)
#define KXPOOL_DATA(kxpool) ((KxPoolData *) ((kxpool)->data.ptr))

typedef struct KxObject KxPool;

typedef struct KxPoolData KxPoolData;

struct KxPoolData {
	Dictionary *objects;
	Dictionary *paths;
};


void kxpool_extension_init();
KxObject * kxpool_new_prototype(KxCore *core);

extern KxObjectExtension kxpool_extension;

#endif // __KXPOOL_H
