/*
   gdkevent.c
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
	

#include <stdlib.h>
#include <gtk/gtk.h>

#include <kxmessage.h>
#include <kxobject.h>
#include <kxcfunction.h>

#include "gdkevent.h"

KxObjectExtension kxgdkevent_extension;

static void
kxgdkevent_add_method_table(KxGdkEvent *self);

static KxObject *
kxgdkevent_clone(KxGdkEvent *self)
{
	KxGdkEvent *clone = kxobject_raw_clone(self);

	return clone;
}

static void
kxgdkevent_free(KxGdkEvent *self)
{
	if (self->data.ptr) {
		g_boxed_free(GDK_TYPE_EVENT, self->data.ptr);
	}
}

void kxgdkevent_extension_init() {
	kxobjectext_init(&kxgdkevent_extension);
	kxgdkevent_extension.type_name = "GdkEvent";

	kxgdkevent_extension.clone = kxgdkevent_clone;
	kxgdkevent_extension.free = kxgdkevent_free;
}


KxObject *
kxgdkevent_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxgdkevent_extension;
	
	// TODO Init data
	//
/*	GdkEvent event;
	event.type = GDK_NOTHING;
	g_value_get_boxed(&event);*/
	
	kxgdkevent_add_method_table(self);

	//kxgtk_register_boxed_type(GDK_TYPE_EVENT, self);
	kxgtk_register_extension(GDK_TYPE_EVENT, &kxgdkevent_extension);
	return self;
}

static KxObject *
kxgdkevent_method(KxGdkEvent *self, KxMessage *message)
{
	KXRETURN(self);
}

static void
kxgdkevent_add_method_table(KxGdkEvent *self)
{
	KxMethodTable table[] = {
		{"method",0, kxgdkevent_method },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

