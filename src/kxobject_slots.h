/*
   kxobject_slots.h
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
	
#ifndef __KXOBJECT_SLOTS_H
#define __KXOBJECT_SLOTS_H

#include "kxobject_struct.h"

#define KXOBJECT_SLOTFLAG_FREEZE 1

//#define kxobject_slots_count(self) ((self)->slots_count)

void kxobject_slots_init(KxObject *self);
void kxobject_slots_free(KxObject *self);
void kxobject_slots_clean(KxObject *self);

KxSlot *
kxobject_slot_find(KxObject *self, KxObject *key);

void kxobject_slots_copy(KxObject *self, KxObject *copy);
void kxobject_slot_add(KxObject *self, KxObject *key, KxObject *value, int flags);

List *kxobject_slots_to_list(KxObject *self);

void kxobject_slots_mark(KxObject *self);

void kxobject_slots_dump(KxObject *self);

int kxobject_slots_count(KxObject *self);

#endif // __KXOBJECT_SLOTS_H
