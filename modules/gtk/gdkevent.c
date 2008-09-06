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
#include <kxinteger.h>
#include <kxsymbol.h>
#include <kxexception.h>

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
kxgdkevent_throw_error(KxGdkEvent *self, KxMessage *message)
{
	KxException *excp = kxexception_new_with_text(KXCORE,
		"This event type don't support property '%s'",
		KXSYMBOL_AS_CSTRING(message->message_name));
	KXTHROW(excp);
}

static KxObject *
kxgdkevent_eventType(KxGdkEvent *self, KxMessage *message)
{
	GdkEvent *data = KXGDKEVENT_DATA(self);
	return KXINTEGER((long)data->type);
}

static KxObject *
kxgdkevent_state(KxGdkEvent *self, KxMessage *message)
{
	GdkEvent *data = KXGDKEVENT_DATA(self);
	if (data->type != GDK_KEY_PRESS && 
		data->type != GDK_KEY_RELEASE)
		return kxgdkevent_throw_error(self, message);

	return KXINTEGER((long)data->key.state);
}

static KxObject *
kxgdkevent_key_value(KxGdkEvent *self, KxMessage *message)
{
	GdkEvent *data = KXGDKEVENT_DATA(self);
	if (data->type != GDK_KEY_PRESS && 
		data->type != GDK_KEY_RELEASE)
		return kxgdkevent_throw_error(self, message);

	return KXINTEGER((long)data->key.keyval);
}

static void
kxgdkevent_add_method_table(KxGdkEvent *self)
{
	KxMethodTable table[] = {
		{"eventType",0, kxgdkevent_eventType },
		{"state",0, kxgdkevent_state },
		{"keyValue",0, kxgdkevent_key_value },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

