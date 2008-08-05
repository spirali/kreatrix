/*
   kxlist.h
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
	
#ifndef __KXLIST_H
#define __KXLIST_H

#include "kxobject_struct.h"
#include "kxcore.h"

typedef struct KxObject KxList;


#define IS_KXLIST(kxobject) ((kxobject)->extension == &kxlist_extension)

#define KXLIST_DATA(kxlist) (List *)((kxlist)->data.ptr)
#define KXLIST_VALUE(kxlist) (List *)((kxlist)->data.ptr)

#define KXLIST(list) (kxlist_new_with(KXCORE,list))

#define KXPARAM_TO_LIST(list, param_id) \
	List * list; { KxObject *tmp = message->params[param_id]; if (IS_KXLIST(tmp)) \
	{ list = KXLIST_VALUE(tmp); } else { return kxobject_type_error(tmp, &kxlist_extension, param_id); }} 


void kxlist_init_extension();

KxList *kxlist_new_prototype(KxCore *core);




/** list is added directly into new object, no copy */
KxList *kxlist_new_with(KxCore *core, List *list);

/** list is not touched, new copy is createn */
KxList *kxlist_new_from_list_of_cstrings(KxCore *core, List *list);


List *kxlist_to_list_of_cstrings(KxList *self);




extern KxObjectExtension kxlist_extension;

#endif
