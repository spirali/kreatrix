/*
   kxinteger.h
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
	
#ifndef __KXINTEGER_H
#define __KXINTEGER_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxstack.h"

typedef struct KxObject KxInteger;

#define IS_KXINTEGER(kxobject) ((kxobject)->extension == &kxinteger_extension)

#define KXINTEGER_VALUE(kxinteger) (KxInt) (kxinteger)->data.intval


#define KXPARAM_TO_LONG(integer, param_id) \
	long integer; { KxObject *tmp = message->params[param_id]; if (IS_KXINTEGER(tmp)) \
	{ integer = KXINTEGER_VALUE(tmp); } else { kxobject_type_error(tmp,&kxinteger_extension, param_id); return NULL; }} 


#define KXPARAM_TO_INT(integer, param_id) \
	int integer; { KxObject *tmp = message->params[param_id]; if (IS_KXINTEGER(tmp)) \
	{ integer = KXINTEGER_VALUE(tmp); } else { kxobject_type_error(tmp,&kxinteger_extension, param_id); return NULL; }} 


#define KXINTEGER(integer) kxcore_get_integer(KXCORE,integer)

typedef long KxInt;

extern KxObjectExtension kxinteger_extension;

void kxinteger_init_extension();

KxObject *kxinteger_new_prototype(KxCore *core);

KxInteger *kxinteger_clone_with(KxInteger *self, KxInt value);

KxInteger *kxinteger_new_with(KxCore *core, KxInt value);

#endif
