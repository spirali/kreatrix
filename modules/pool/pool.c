/*
   pool.c
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


#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxpool.h"

KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxpool_extension_init();

	KxPool *kxpool_prototype = kxpool_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxpool_prototype);
	
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Pool"),kxpool_prototype);
	
	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxpool_extension);
}




