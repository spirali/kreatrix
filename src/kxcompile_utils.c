/*
   kxcompile_utils.c
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
#include <stdlib.h>

#include "kxcompile_utils.h"
#include "kxobject.h"
#include "kxcodeblock.h"
#include "compiler/kxcompiler.h"
#include "kxexception.h"
#include "kxglobals.h"

KxObject *
kxcompile_do_file(char *filename, KxObject *target, KxSymbol *message_name, int doc_flag)
{
	char *bytecode;
	int size;
	List *errors = kxc_compile_file(filename,doc_flag, kx_level_of_optimisation, &bytecode, &size);
	
	if (errors) {
		KxException *excp = kxexception_new_with_text(KXCORE_FROM(target),errors->items[0]);
		list_free_all(errors);
		kxstack_throw_object(KXSTACK_FROM(target),excp);
		return NULL;
	}
	if (kx_verbose) {
		printf("do_file: '%s'\n", filename);
	}
	KxObject * retobj =  kxcompile_run_bytecode(bytecode,target, filename, message_name);
	
	kxfree(bytecode);
	return retobj;

}


KxObject *
kxcompile_run_bytecode(char *bytecode,  KxObject *target, 
				char *source_filename, KxSymbol *message_name)
{
	char *bytecode_pos = bytecode;

	KxCodeBlock *codeblock = kxcodeblock_new_from_bytecode(KXCORE_FROM(target), &bytecode_pos, source_filename);

	KxMessage *start_msg = kxmessage_new(message_name, target, target);

	KxObject * obj;
	
	obj = kxcodeblock_run(codeblock,target, start_msg);

	kxmessage_free(start_msg);
	REF_REMOVE(codeblock);
	return obj;
}
