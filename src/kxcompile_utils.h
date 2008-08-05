/*
   kxcompile_utils.h
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
	
#ifndef __KXCOMPILE_UTILS_H
#define __KXCOMPILE_UTILS_H

#include "kxobject_struct.h"
#include "kxsymbol.h"


KxObject * kxcompile_do_file(char *filename, KxObject *target, KxSymbol *message_name, int doc_flag );

KxObject * kxcompile_run_bytecode(char *bytecode,  KxObject *target, char *source_filename, KxSymbol *message_name);


#endif
