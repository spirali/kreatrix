/*
   kxcompiler.h
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
	
#ifndef __OBJ_KXCOMPILER_H
#define __OBJ_KXCOMPILER_H

#include "kxcore.h"
typedef struct KxObject KxCompiler;

KxCompiler *
kxcompiler_new_prototype(KxCore *core);


#endif // __OBJ_KXCOMPILER_H
