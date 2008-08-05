/*
   kxrandom.c
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

#include <kxmessage.h>
#include <kxobject.h>
#include <kxcfunction.h>
#include <kxinteger.h>
#include <time.h>

#include "kxrandom.h"


KxObjectExtension kxrandom_extension;

static void
kxrandom_add_method_table(KxRandom *self);

static KxObject *
kxrandom_clone(KxRandom *self)
{
	KxRandom *clone = kxobject_raw_clone(self);
	
	clone->data.intval = self->data.intval;

	return clone;
}

static void
kxrandom_free(KxRandom *self)
{
}

void kxrandom_extension_init() {
	kxobjectext_init(&kxrandom_extension);
	kxrandom_extension.type_name = "Random";

	kxrandom_extension.clone = kxrandom_clone;
	kxrandom_extension.free = kxrandom_free;
}

KxObject *
kxrandom_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxrandom_extension;
	
	self->data.intval = 1;
	
	kxrandom_add_method_table(self);
	return self;
}


static KxObject *
kxrandom_set_seed(KxRandom *self, KxMessage *message)
{
	KXPARAM_TO_INT(intseed, 0);
	self->data.intval = intseed;
	KXRETURN(self);
}

static KxObject *
kxrandom_get_seed(KxRandom *self, KxMessage *message)
{
	return KXINTEGER(self->data.intval);
}

static KxObject *
kxrandom_seed_from_time(KxRandom *self, KxMessage *message)
{
	self->data.intval = time(0);
	KXRETURN(self);
}

static KxObject *
kxrandom_get_integer_with_limit(KxRandom *self, KxMessage *message)
{
	KXPARAM_TO_INT(limit, 0);
	int result = rand_r(&self->data.intval) % limit;
	return KXINTEGER(result);
}


static KxObject *
kxrandom_get_integer(KxRandom *self, KxMessage *message)
{
	int result = rand_r(&self->data.intval);
	return KXINTEGER(result);
}



static void
kxrandom_add_method_table(KxRandom *self)
{
	KxMethodTable table[] = {
		{"getInteger",0, kxrandom_get_integer },
		{"getInteger:",1, kxrandom_get_integer_with_limit },
		{"seed:",1, kxrandom_set_seed },
		{"seed",0, kxrandom_get_seed },
		{"seedFromTime",0, kxrandom_seed_from_time },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

