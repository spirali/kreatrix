/*
   kxobject_profile.h
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

#ifndef __KXOBJECT_PROFILE_H
#define __KXOBJECT_PROFILE_H

#include "kxobject.h"

KxObjectProfile * kxobject_profile_new();
void kxobject_profile_free(KxObjectProfile *self);
void kxobject_profile_add_child_prototype(KxObjectProfile *self, KxObject *prototype);
void kxobject_profile_add_symbol(KxObjectProfile *self, KxSymbol *symbol);
int kxobject_profile_check_symbol(KxObjectProfile *self, KxSymbol *symbol);
KxObjectProfile *kxobject_profile_new_for_child(KxObjectProfile *self, KxObject *child);
void kxobject_profile_remove_child_prototype(KxObjectProfile *self, KxObject *child);
void kxobject_check_profile_and_repair(KxObject *self, KxSymbol *symbol);

#endif // __KXOBJECT_PROFILE_H
