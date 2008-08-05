/*
   kxfloat.h
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
	
#ifndef __KXFLOAT_H
#define __KXFLOAT_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxstack.h"

typedef struct KxObject KxFloat;

#define IS_KXFLOAT(kxobject) ((kxobject)->extension == &kxfloat_extension)

#define KXFLOAT_VALUE(kxfloat) (double) (kxfloat)->data.doubleval


#define KXPARAM_TO_DOUBLE(double_var, param_id) \
	double double_var; { KxObject *tmp = message->params[param_id]; if (IS_KXFLOAT(tmp)) \
	{ double_var = KXFLOAT_VALUE(tmp); } else { kxfloat_type_error(KXCORE); return NULL; }} 


#define KXFLOAT(fl) kxfloat_new_with(KXCORE,fl)

extern KxObjectExtension kxfloat_extension;

void kxfloat_init_extension();

KxObject *kxfloat_new_prototype(KxCore *core);

KxFloat *kxfloat_clone_with(KxFloat *self, double value);

KxFloat *kxfloat_new_with(KxCore *core, double value);

void kxfloat_type_error(KxCore *core);

#endif
