/*
   kxtimevalue.h
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

#ifndef __KX_OBJECT_TIMEVALUE_H
#define __KX_OBJECT_TIMEVALUE_H

#include <sys/time.h>
#include "kxobject_struct.h"
#include "kxcore.h"

#define KXPARAM_TO_TIMEVAL(tvalue, param_id) \
	struct timeval *tvalue; { KxObject *tmp = message->params[param_id]; if (IS_KXTIMEVALUE(tmp)) \
	{ tvalue = KXTIMEVALUE_VALUE(tmp); } else { kxobject_type_error(tmp,&kxtimevalue_extension, param_id); return NULL; }} 

#define KXTIMEVALUE(tv) kxtimevalue_new(KXCORE, (tv))

#define KXTIMEVALUE_VALUE(self) ((struct timeval*) (self)->data.ptr)

#define IS_KXTIMEVALUE(self) ((self)->extension == &kxtimevalue_extension)

typedef struct KxObject KxTimeValue;

KxTimeValue *kxtimevalue_new_prototype(KxCore *core);
KxObject *kxtimevalue_new(KxCore *core, struct timeval *tv);

void kxtimevalue_extension_init();
extern KxObjectExtension kxtimevalue_extension;


#endif // __KX_OBJECT_TIMEVALUE_H
