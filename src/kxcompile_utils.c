/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

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
	List *errors = kxc_compile_file(filename,doc_flag,&bytecode, &size);
	
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
