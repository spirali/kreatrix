/*
   kxarray2d.h
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
	
#ifndef __KX_OBJECT_ARRAY2D_H
#define __KX_OBJECT_ARRAY2D_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXARRAY2D_DATA(self) ((KxArray2dData*) (self)->data.ptr)

#define IS_KXARRAY2D(self) ((self)->extension == &kxarray2d_extension)

typedef struct KxObject KxArray2d;
typedef struct KxArray2dData KxArray2dData;

struct KxArray2dData {
	KxObject **array;
	int sizex;
	int sizey;
};


KxArray2d *kxarray2d_new_prototype(KxCore *core);


void kxarray2d_init_extension();
extern KxObjectExtension kxarray2d_extension;


#endif // __KX_OBJECT_ARRAY2D_H
