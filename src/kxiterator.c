/*
   kxiterator.c
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

#include "kxiterator.h"

KxObjectExtension kxiterator_extension;

static void
kxiterator_free(KxIterator *self)
{
	KxIteratorData *data = KXITERATOR_DATA(self);
	REF_REMOVE(data->object);
	free(data);
}

static void
kxiterator_mark(KxIterator *self)
{
	KxIteratorData *data = KXITERATOR_DATA(self);
	kxobject_mark(data->object);
}


void kxiterator_init_extension() {
	kxobjectext_init(&kxiterator_extension);
	kxiterator_extension.type_name = "Iterator";

	kxiterator_extension.free = kxiterator_free;
	kxiterator_extension.mark = kxiterator_mark;
}

KxObject *kxiterator_create(KxCore *core, KxIteratorData *data)
{
	KxObject *self = kxcore_raw_clone_basic_prototype(core, KXPROTO_ITERATOR);
	self->data.ptr = data;
	return self;
}

KxObject *
kxiterator_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxiterator_extension;

	KxIteratorDataInt *data = kxmalloc(sizeof(KxIteratorDataInt));
	ALLOCTEST(data);
	data->object = kxcore_get_basic_prototype(core, KXPROTO_LIST);
	REF_ADD(data->object);
	data->position = 0;
	self->data.ptr = data;
	
	return self;
}
