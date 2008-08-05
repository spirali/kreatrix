/*
   kxstring.h
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
	
#ifndef __KXSTRING_H
#define __KXSTRING_H

#include "kxobject_struct.h"
#include "kxcore.h"
//#include "kxstack.h"

typedef struct KxObject KxString;

#define IS_KXSTRING(kxobject) ((kxobject)->extension == &kxstring_extension)

#define KXSTRING_VALUE(kxinteger) (char*) ((kxinteger)->data.ptr)
#define KXSTRING_TO_CSTRING(kxinteger) (char*) ((kxinteger)->data.ptr)


#define KXPARAM_TO_CSTRING(str, param_id) \
	char * str; { KxObject *tmp = message->params[param_id]; if (IS_KXSTRING(tmp)) \
	{ str = KXSTRING_VALUE(tmp); } else { kxobject_type_error(tmp, &kxstring_extension, param_id); return NULL; }} 


#define KXSTRING(str) kxstring_new_with(KXCORE,str)


extern KxObjectExtension kxstring_extension;

void kxstring_init_extension();

KxObject *kxstring_new_prototype(KxCore *core);

KxString *kxstring_clone_with(KxString *self, char * value);

// Copy string into object
KxString *kxstring_new_with(KxCore *core, char *value);

// Use string in object, no coping of value
KxString *kxstring_from_cstring(KxCore *core, char *value);

extern KxObjectExtension kxstring_extension;


#endif
