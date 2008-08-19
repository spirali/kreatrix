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
#include "kxobject_slots.h"

#ifdef KX_INLINE_CACHE

KxObjectProfile * 
kxobject_profile_new()
{
	KxObjectProfile *self = kxmalloc(sizeof(KxObjectProfile));
	ALLOCTEST(self);
	
	self->slots_symbols = list_new();
	self->child_prototypes = list_new();
	return self;
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

KxObjectProfile *
kxobject_profile_copy(KxObjectProfile *self)
{
	KxObjectProfile *new = kxmalloc(sizeof(KxObjectProfile));
	ALLOCTEST(new);
	
	new->slots_symbols = list_copy(self->slots_symbols);
	new->child_prototypes = list_new();
	return new;
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

	if (kxobject_profile_check_symbol(self, symbol)) {
		return;
	}

	list_append(self->slots_symbols, symbol);

	int t;

	for (t=0; t < self->child_prototypes->size; t++) {
		KxObject *prototype = self->child_prototypes->items[t];
		kxobject_profile_add_symbol(prototype->profile, symbol);
	}
}

List *
kxobject_profile_instance_slots_to_list(KxObjectProfile *self)
{
	return list_copy(self->slots_symbols);
}

List *
kxobject_profile_child_prototypes_to_list(KxObjectProfile *self)
{
	return list_copy(self->child_prototypes);
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
kxobject_check_profile_and_repair(KxObject *self, KxSymbol *symbol, KxObject *value)
{
	if (IS_KXCODEBLOCK(value)) {
		KxCodeBlockData *cdata = KXCODEBLOCK_DATA(value);
		char *s = cdata->created_slots;
		if (s) {
			if (self->ptype != KXOBJECT_PROTOTYPE) {
				kxobject_set_as_prototype(self);
			}
			while(*s != -1) {
				if (kx_verbose) {
					printf("Added slot '%s' to profile from 'created_slots'\n", 
						KXSYMBOL_AS_CSTRING(cdata->literals[(int)*s]));
				}

				kxobject_profile_add_symbol(self->profile, cdata->literals[(int)*s]);
				s++;

			}
		}
	}

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
			kx_inline_cache_repair_by_prototype_and_name(self, symbol);
			/*if (kx_verbose) {
				printf("Inline cache repair '%s'/", KXSYMBOL_AS_CSTRING(symbol));
				kxobject_dump(self);
			}*/
		}	
	}
}

static void
kxobject_repair_prototype_profile_after_parent_change(KxObject *self)
{
	if (kxobject_recursive_mark_test(self))
		return;

	kxobject_recursive_mark_set(self);

	int size = self->parent_slot.parent->profile->slots_symbols->size;
	KxSymbol **symbols = (KxSymbol **) self->parent_slot.parent->profile->slots_symbols->items;
	int t;
	for (t=0; t < size; t++) {
			if (!kxobject_profile_check_symbol(self->profile, symbols[t])) {
				list_append(self->profile->slots_symbols, symbols[t]);
			}
	}

	for (t=0; t < self->profile->child_prototypes->size; t++) {
		KxObject *child = self->profile->child_prototypes->items[t];
		kxobject_repair_prototype_profile_after_parent_change(child);
	}

	kxobject_recursive_mark_reset(self);
}

static int
kxobject_check_if_instance_profile_is_valid(KxObject *self)
{
	int count = kxobject_slots_count(self);
	int size = self->profile->slots_symbols->size;
	int t;
	for (t=0; t < size; t++) {
			KxSymbol *symbol = self->profile->slots_symbols->items[t];

			if (kxobject_get_slot(self, symbol)) {
				count--;
			}
	}

	return count == 0;
}


void
kxobject_repair_profile_after_parent_change(KxObject *self)
{
	switch(self->ptype) {
		case KXOBJECT_INSTANCE:
			self->profile = self->parent_slot.parent->profile;
			if (!kxobject_check_if_instance_profile_is_valid(self)) {
				kxobject_set_as_singleton(self);
			}
			return;
		case KXOBJECT_SINGLETON:
			self->profile = self->parent_slot.parent->profile;
			return;
		case KXOBJECT_PROTOTYPE:
			kxobject_repair_prototype_profile_after_parent_change(self);
			kx_inline_cache_repair_by_prototype(self);
			return;
	}
}

void
kxobject_set_as_prototype(KxObject *self)
{
	self->profile = kxobject_profile_new_for_child(self->profile, self);
	self->ptype = KXOBJECT_PROTOTYPE;
}

void
kxobject_set_as_singleton(KxObject *self)
{
	self->ptype = KXOBJECT_SINGLETON;
}

void
kxobject_set_as_noparent(KxObject *self)
{
	if (self->ptype != KXOBJECT_PROTOTYPE) {
		kxobject_set_as_prototype(self);
	}
}

#endif // KX_INLINE_CACHE
