/*
   kxcomp.h
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
	
#ifndef __KREATRIX_KXCOMP_H
#define __KREATRIX_KXCOMP_H

#include "kxparser.h"
#include "../utils/list.h"
#include "block.h"
typedef struct KxCompiler KxCompiler;

struct KxCompiler {
	int token;
	KxParser *parser;
	
	KxcBlock *block;

	int resend;
	int compile_docs;

	char *filename;
	List *errors;
	
};

List* kxcomp_compile_file(char *filename, int compile_docs, KxcBlock **block);
List *kxcomp_compile_string(char *string, char *filename, int compile_docs, KxcBlock **block);

List *kxcomp_one_error_list(char *form, ...);



#endif // __KREATRIX_KXCOMP_H
