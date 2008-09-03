/*
   kxsymbol.c
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
	
/*KXobject Base Symbol
[Basic prototypes]
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kxsymbol.h"
#include "kxcore.h"
#include "kxobject.h"
#include "kxstring.h"

KxObjectExtension kxsymbol_extension;


static KxSymbol *kxsymbol_clone(KxSymbol *self);
static void kxsymbol_free(KxSymbol *self);
static void kxsymbol_dump(KxSymbol *self);


void kxsymbol_init_extension() 
{
	kxobjectext_init(&kxsymbol_extension);
	kxsymbol_extension.type_name = "Symbol";
	kxsymbol_extension.clone = kxsymbol_clone;
	kxsymbol_extension.free = kxsymbol_free;
	kxsymbol_extension.dump = kxsymbol_dump;
	kxsymbol_extension.is_immutable = 1;
}

KxSymbol *
kxsymbol_new_prototype(KxCore *core)
{
	KxObject *symbol = kxcore_clone_base_object(core);
	symbol->extension = &kxsymbol_extension;
	symbol->data.ptr = strdup("void");
	return symbol;
}



KxSymbol *
kxsymbol_clone_with(KxSymbol *self, char *symbolname)
{
	KxObject *symbol = kxobject_raw_clone(self);
	symbol->data.ptr = strdup(symbolname);

	return symbol;
}



static KxSymbol 
*kxsymbol_clone(KxSymbol *self) 
{
	// Symbol with same string value can be only one
	return self;	
}

static void 
kxsymbol_free(KxSymbol *self) 
{
	kxcore_symbol_remove(KXCORE,self);
	kxfree(self->data.ptr);
}

static void 
kxsymbol_dump(KxSymbol *self) 
{
	printf("#%s", KXSYMBOL_AS_CSTRING(self));
}

KxObject *
kxsymbol_as_string(KxSymbol *self, KxMessage *message)
{
	return KXSTRING(self->data.ptr);
}

KxObject *kxstring_lt(KxString *self, KxMessage *message);


void 
kxsymbol_add_method_table(KxSymbol *self)
{
	KxMethodTable table[] = {
		{"<",1, kxstring_lt },
		{"asString",0, kxsymbol_as_string },
		{"asSymbol",0, kxcfunction_returnself },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}


