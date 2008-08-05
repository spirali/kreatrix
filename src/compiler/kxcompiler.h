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
	

#ifndef __KXCOMPILER_H
#define __KXCOMPILER_H

#include "../utils/list.h"


#define KXC_FILE_MAGIC "\1KXcode"
#define KXC_FILE_VERSION_MAJOR 0
#define KXC_FILE_VERSION_MINOR 1


struct List;

List *kxc_compile_and_save_file(char *input_filename, char *output_filename, int compile_docs);
List *kxc_compile_file(char *input_filename, int compile_docs, char **bytecode, int *size);
List *kxc_compile_string(char *code_string, char *filename, int compile_docs,  char **bytecode, int *size);

#endif // __KXCOMPILER_H
