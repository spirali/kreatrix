/*
   kxassociation.h
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
	
#ifndef __KXASSOCIATION_H
#define __KXASSOCIATION_H

#include "kxobject_struct.h"
#include "kxcore.h"

typedef struct KxObject KxAssociation;

#define IS_KXASSOCIATION(kxobject) ((kxobject)->extension == &kxassociation_extension)

#define KXASSOCIATION_GET_KEY(kxassoc) ((KxObject*) ((kxassoc)->data.data2.ptr1))
#define KXASSOCIATION_GET_VALUE(kxassoc) ((KxObject*) ((kxassoc)->data.data2.ptr2))

#define KXPARAM_TO_KXASSOCIATION(obj, param_id) \
	KxObject * obj; { obj = message->params[param_id]; if (!IS_KXASSOCIATION(obj)) \
		{ kxobject_type_error(obj, &kxassociation_extension, param_id); return NULL; }} 


extern KxObjectExtension kxassociation_extension;

void kxassociation_init_extension();

KxObject *kxassociation_new_prototype(KxCore *core);

extern KxObjectExtension kxassociation_extension;

#endif
