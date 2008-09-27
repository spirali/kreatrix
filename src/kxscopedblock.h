/*
   kxscopedblock.h
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
	
#ifndef __KXSCOPEDBLOCK_H
#define __KXSCOPEDBLOCK_H


#include "kxcodeblock.h"

#define IS_KXSCOPEDBLOCK(kxobject) ((kxobject)->extension == &kxscopedblock_extension)

#define KXSCOPEDBLOCK_CODEBLOCK(kxobject) ((KxObject *) ((kxobject)->data.data2.ptr2))
#define KXSCOPEDBLOCK_SCOPE(kxobject) ((KxActivation *) ((kxobject)->data.data2.ptr1))

typedef struct KxObject KxScopedBlock;

typedef struct KxScopedBlockData KxScopedBlockData;

struct KxActivation;

void kxscopedblock_init_extension();
KxObject *kxscopedblock_new_prototype(KxCore *core);

KxScopedBlock *kxscopedblock_new(KxCore *core, KxCodeBlock *codeblock, struct KxActivation *scope);

KxObject * kxscopedblock_run(KxScopedBlock *self, KxMessage *message);

extern KxObjectExtension kxscopedblock_extension;

#endif 
