/*
   gdkgeometry.h
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
	
#ifndef __KX_OBJECT_GDKGEOMETRY_H
#define __KX_OBJECT_GDKGEOMETRY_H

#include <gtk/gtk.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXGDKGEOMETRY_DATA(self) ((GdkGeometry*) (self)->data.ptr)

#define KXGDKGEOMETRY(data) kxgdkgeometry_from(KXCORE, (data))

#define IS_KXGDKGEOMETRY(self) (kxobject_check_type(self,&kxgdkgeometry_extension))

#define KXPARAM_TO_GDKGEOMETRY(param_name, param_id) \
	GdkGeometry* param_name; { KxObject *tmp = message->params[param_id]; if (IS_KXGDKGEOMETRY(tmp)) \
	{ param_name = KXGDKGEOMETRY_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgdkgeometry_extension); }} 

#define KXPARAM_TO_GDKGEOMETRY_OR_NULL(param_name, param_id) \
	GdkGeometry* param_name; { KxObject *tmp = message->params[param_id]; \
	if(tmp == KXCORE->object_nil) { param_name = NULL; } else if (IS_KXGDKGEOMETRY(tmp)) \
	{ param_name = KXGDKGEOMETRY_DATA(tmp); } else { return kxobject_type_error(tmp,&kxgdkgeometry_extension); }} 


typedef struct KxObject KxGdkGeometry;

KxGdkGeometry *kxgdkgeometry_new_prototype(KxObject *parent);
KxObject *kxgdkgeometry_from(KxCore *core, GdkGeometry* data);


void kxgdkgeometry_extension_init();
extern KxObjectExtension kxgdkgeometry_extension;


#endif // __KX_OBJECT_GDKGEOMETRY_H
