/*
   kxtimevalue.c
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
#include <string.h>

#include "kxtimevalue.h"
#include "kxinteger.h"

KxObjectExtension kxtimevalue_extension;

static void
kxtimevalue_add_method_table(KxTimeValue *self);

static KxObject *
kxtimevalue_clone(KxTimeValue *self)
{
	KxTimeValue *clone = kxobject_raw_clone(self);
	
	clone->data.ptr = kxmalloc(sizeof(struct timeval));
	ALLOCTEST(clone->data.ptr);
	memcpy(clone->data.ptr, self->data.ptr, sizeof(struct timeval));

	return clone;
}

KxObject *
kxtimevalue_new(KxCore *core, struct timeval *tv)
{
	KxTimeValue *proto = kxcore_get_prototype(core, &kxtimevalue_extension);
	
	KxObject *self = kxobject_raw_clone(proto);
	self->data.ptr = kxmalloc(sizeof(struct timeval));
	ALLOCTEST(self->data.ptr);
	memcpy(self->data.ptr, tv, sizeof(struct timeval));
	return self;
}

static void
kxtimevalue_free(KxTimeValue *self)
{
	kxfree(self->data.ptr);
}

void kxtimevalue_extension_init() {
	kxobjectext_init(&kxtimevalue_extension);
	kxtimevalue_extension.type_name = "TimeValue";

	kxtimevalue_extension.clone = kxtimevalue_clone;
	kxtimevalue_extension.free = kxtimevalue_free;
}

KxObject *
kxtimevalue_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxtimevalue_extension;

	self->data.ptr = kxmalloc(sizeof(struct timeval));
	ALLOCTEST(self->data.ptr);
	timerclear(KXTIMEVALUE_VALUE(self));
	
	kxtimevalue_add_method_table(self);
	return self;
}


static KxObject *
kxtimevalue_clear(KxTimeValue *self, KxMessage *message)
{
	timerclear(KXTIMEVALUE_VALUE(self));
	KXRETURN(self);
}

static KxObject *
kxtimevalue_add(KxTimeValue *self, KxMessage *message)
{
	KXPARAM_TO_TIMEVAL(tv, 0);
	struct timeval tv2;
	timeradd(KXTIMEVALUE_VALUE(self), tv, &tv2);
	return KXTIMEVALUE(&tv2);
}

static KxObject *
kxtimevalue_sub(KxTimeValue *self, KxMessage *message)
{
	KXPARAM_TO_TIMEVAL(tv, 0);
	struct timeval tv2;
	timersub(KXTIMEVALUE_VALUE(self), tv, &tv2);
	return KXTIMEVALUE(&tv2);
}

static KxObject *
kxtimevalue_sec(KxTimeValue *self, KxMessage *message)
{
	return KXINTEGER(KXTIMEVALUE_VALUE(self)->tv_sec);
}

static KxObject *
kxtimevalue_usec(KxTimeValue *self, KxMessage *message)
{
	return KXINTEGER(KXTIMEVALUE_VALUE(self)->tv_usec);
}

static KxObject *
kxtimevalue_cmp1(KxTimeValue *self, KxMessage *message)
{
	KXPARAM_TO_TIMEVAL(tv2, 0);
	struct timeval *tv1 = KXTIMEVALUE_VALUE(self);
	KXRETURN_BOOLEAN(timercmp(tv1, tv2, <));
}

static KxObject *
kxtimevalue_cmp2(KxTimeValue *self, KxMessage *message)
{
	KXPARAM_TO_TIMEVAL(tv2, 0);
	struct timeval *tv1 = KXTIMEVALUE_VALUE(self);
	KXRETURN_BOOLEAN(timercmp(tv1, tv2, >));
}

static void
kxtimevalue_add_method_table(KxTimeValue *self)
{
	KxMethodTable table[] = {
		{"seconds", 0, kxtimevalue_sec},
		{"useconds", 0, kxtimevalue_usec},
		{"+",1, kxtimevalue_add },
		{"-",1, kxtimevalue_sub },
		{"<",1, kxtimevalue_cmp1 },
		{">",1, kxtimevalue_cmp2 },
		{"clear",0, kxtimevalue_clear },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

