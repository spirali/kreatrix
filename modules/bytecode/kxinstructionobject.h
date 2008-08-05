/*
   kxinstructionobject.h
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
	
#ifndef __KX_OBJECT_INSTRUCTIONOBJECT_H
#define __KX_OBJECT_INSTRUCTIONOBJECT_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxdecompiler.h"

#define KXINSTRUCTIONOBJECT_DATA(self) ((KxInstructionObjectData*) (self)->data.ptr)

#define IS_KXINSTRUCTIONOBJECT(self) ((self)->extension == &kxinstructionobject_extension)

typedef struct KxObject KxInstructionObject;
typedef struct KxInstructionObjectData KxInstructionObjectData;

struct KxInstructionObjectData {
	KxInstructionWrapper *instruction;
	KxCodeBlock *codeblock;
};

KxInstructionObject *kxinstructionobject_new_prototype(KxCore *core);
KxObject *kxinstructionobject_new_from(KxCore *core, KxCodeBlock *codeblock, KxInstructionWrapper *instruction);


void kxinstructionobject_extension_init();
extern KxObjectExtension kxinstructionobject_extension;


#endif // __KX_OBJECT_INSTRUCTIONOBJECT_H
