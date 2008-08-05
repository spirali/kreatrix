/*
   gdkevent.h
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

#ifndef __KX_OBJECT_GDKEVENT_H
#define __KX_OBJECT_GDKEVENT_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXGDKEVENT(self) ((self)->extension == &kxgdkevent_extension)

#define KXGDKEVENT_DATA(self) ((GdkEvent*) self->data.ptr)

#define KXPARAM_TO_GDKEVENT(param_name, param_id) \
	GdkEvent* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGDKEVENT(tmp)) \
	{ param_name = KXGDKEVENT_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgdkevent_extension); }} 


typedef struct KxObject KxGdkEvent;


KxGdkEvent *kxgdkevent_new_prototype(KxCore *core);


void kxgdkevent_extension_init();
extern KxObjectExtension kxgdkevent_extension;


#endif // __KX_OBJECT_GDKEVENT_H
