/*
   kxset.h
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
	
#ifndef __KXSET_H
#define __KXSET_H

#include "kxcore.h"
#include "kxobject_struct.h"

typedef struct KxObject KxSet;

#define IS_KXSET(kxobject) ((kxobject)->extension == &kxset_extension)

//#define KXSET(set) (kxset_new_with(KXCORE,set))

void kxset_init_extension();

KxSet *kxset_new_prototype(KxCore *core);


extern KxObjectExtension kxset_extension;


#endif // __KXSET_H
