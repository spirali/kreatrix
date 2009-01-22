/*
   time.c
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
	
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxtimevalue.h"

static void kxclock_add_method_table(KxObject *self);


KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{
	kxtimevalue_extension_init();

	KxObject *timevalue = kxtimevalue_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, timevalue);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("TimeValue"), timevalue);

	kxclock_add_method_table(self);
	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxtimevalue_extension);
}

static KxObject *
kxclock_millisecond_clock_value(KxObject *self, KxMessage *message)
{
	struct timeval tv;
	errno = 0;
	if (gettimeofday(&tv, NULL)) {
		KXTHROW(kxexception_from_errno(KXCORE,"millisecondClockValue"));
	}
	return KXINTEGER(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static KxObject *
kxclock_now(KxObject *self, KxMessage *message)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return KXTIMEVALUE(&tv);
}

static void
kxclock_add_method_table(KxObject *self)
{
	KxMethodTable table[] = {
		{"millisecondClockValue",0,kxclock_millisecond_clock_value},
		{"now",0,kxclock_now},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
