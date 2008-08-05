/*
   kxsymbol.h
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
	
#ifndef __KXSYMBOL_H
#define __KXSYMBOL_H

#include "kxcore.h"

#define IS_KXSYMBOL(self) ((self)->extension==&kxsymbol_extension)
#define KXSYMBOL_AS_CSTRING(symbol) (char*) (symbol)->data.ptr

#define KXCHECK_SYMBOL(object, param_id) if (!IS_KXSYMBOL(object)) { return kxobject_type_error( (object), &kxsymbol_extension, param_id); }

typedef struct KxObject KxSymbol;


void kxsymbol_init_extension();

KxSymbol *kxsymbol_new_prototype(KxCore *core);


KxSymbol *kxsymbol_clone_with(KxSymbol *self, char *symbolname);


extern KxObjectExtension kxsymbol_extension;

void kxsymbol_add_method_table(KxSymbol *self);

#endif // __KXSYMBOL_H
