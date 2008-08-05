/*
   kxcharacter.h
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
	
#ifndef __KXCHARACTER_H
#define __KXCHARACTER_H

#include "kxcore.h"
#include "kxobject_struct.h"

typedef struct KxObject KxCharacter;

#define IS_KXCHARACTER(kxobject) ((kxobject)->extension == &kxcharacter_extension)

#define KXCHARACTER_VALUE(kxcharacter) (char) (kxcharacter)->data.charval

#define KXCHARACTER(chr) kxcore_get_char(KXCORE, chr)

#define KXPARAM_TO_CHAR(c, param_id) \
	unsigned char c; { KxObject *tmp = message->params[param_id]; if (IS_KXCHARACTER(tmp)) \
	{ c = KXINTEGER_VALUE(tmp); } else { kxobject_type_error(tmp, &kxcharacter_extension, param_id); return NULL; }} 


void kxcharacter_init_extension();

KxObject *kxcharacter_new_prototype(KxCore *core);

KxCharacter * kxcharacter_new_with(KxCore *core, unsigned char ch);


KxObjectExtension kxcharacter_extension;

#endif // __KXCHARACTER_H
