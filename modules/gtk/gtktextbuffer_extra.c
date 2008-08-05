/*
   gtktextbuffer_extra.c
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
#include <string.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

#include "gtktextbuffer.h"
#include "gtktextiter.h"


static KxObject *
kxgtktextbuffer_insert (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(string, 0);
	KXPARAM_TO_GTKTEXTITER(iter, 1);

	gtk_text_buffer_insert(self->data.ptr, iter, string, strlen(string));

	KXRETURN(self);
}

static KxObject *
kxgtktextbuffer_set_text (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(string, 0);

	gtk_text_buffer_set_text(self->data.ptr, string, strlen(string));

	KXRETURN(self);
}


void
kxgtktextbuffer_extra_init(KxObject *self)
{
	KxMethodTable table[] = {
		{"insert:at:",2,kxgtktextbuffer_insert},
		{"text:",1,kxgtktextbuffer_set_text},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
