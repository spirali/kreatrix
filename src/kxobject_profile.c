/*
   kxobject_profile.c
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

#include <stdio.h>
#include <stdlib.h>

#include "kxobject_profile.h"
#include "utils/list.h"
#include "kxcodeblock.h"
#include "kxglobals.h"

KxObjectProfile * 
kxobject_profile_new()
{
	KxObjectProfile *self = kxmalloc(sizeof(KxObjectProfile));
	ALLOCTEST(self);
	
	self->slots_symbols = list_new();
	self->child_prototypes = list_new();

	return self;
}

void
kxobject_profile_free(KxObjectProfile *self)
{
	list_free(self->slots_symbols);
	list_free(self->child_prototypes);
	kxfree(self);
}

void
kxobject_profile_add_child_prototype(KxObjectProfile *self, KxObject *prototype)
{
	list_append(self->child_prototypes, prototype);
}

void
kxobject_profile_remove_child_prototype(KxObjectProfile *self, KxObject *child)
{
	list_remove_first(self->child_prototypes, child);
}

void
kxobject_profile_add_symbol(KxObjectProfile *self, KxSymbol *symbol)
{
	list_append(self->slots_symbols, symbol);
}

int
kxobject_profile_check_symbol(KxObjectProfile *self, KxSymbol *symbol)
{
	int t;
	int size = self->slots_symbols->size;
	KxSymbol **symbols = (KxSymbol **) self->slots_symbols->items;
	for (t=0; t < size; t++) {
		if (symbol == symbols[t]) 
			return 1;
	}
	return 0;
}

void
kxobject_check_profile_and_repair(KxObject *self, KxSymbol *symbol)
{
	int t;
	int size = self->profile->slots_symbols->size;
	KxSymbol **symbols = (KxSymbol **) self->profile->slots_symbols->items;
	for (t=0; t < size; t++) {
		if (symbol == symbols[t]) 
			return;
	}

	if (self->ptype == KXOBJECT_INSTANCE) {
		kxobject_set_as_singleton(self);
	} else {
		if (self->ptype == KXOBJECT_PROTOTYPE) {
			kx_inline_cache_repair_prototype_and_name(self, symbol);
			/*if (kx_verbose) {
				printf("Inline cache repair '%s'/", KXSYMBOL_AS_CSTRING(symbol));
				kxobject_dump(self);
			}*/
		}	
	}
}


KxObjectProfile *
kxobject_profile_new_for_child(KxObjectProfile *self, KxObject *child)
{
	KxObjectProfile *new = kxmalloc(sizeof(KxObjectProfile));
	ALLOCTEST(new);
	
	new->slots_symbols = list_copy(self->slots_symbols);
	new->child_prototypes = list_new();

	list_append(self->child_prototypes, child);

	return new;
}
