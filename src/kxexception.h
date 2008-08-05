/*
   kxexception.h
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
	
#ifndef __KXEXCEPTION_H
#define __KXEXCEPTION_H

#include "kxcore.h"
#include "kxstring.h"
typedef KxObject KxException;

#define KXTHROW_EXCEPTION(text)  KXTHROW(kxexception_new_with_text(KXCORE,(text)));

#define KXTHROW_REGEXCEPTION(group, name) KXTHROW(kxcore_get_registered_exception(KXCORE, (group), (name)));

KxObject * kxexception_new_prototype(KxCore *core);

/* I don't add own reference on message, BACKWARD COMPATABILITY */
KxObject * kxexception_new_with_message(KxCore *core, KxString *message);

KxObject * kxexception_new_with_text(KxCore *core, char *form, ...);

KxException *kxexception_type_error(char *type_name, KxObject *obj);
KxException *kxexception_from_errno(KxCore *core, char *text);



#endif
