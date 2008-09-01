/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "kxcompiler.h"
#include "kxobject.h"
#include "kxstring.h"
#include "kxexception.h"
#include "compiler/kxcompiler.h"
#include "kxbytearray.h"
#include "kxcodeblock.h"
#include "kxcompile_utils.h"
#include "kxglobals.h"


/*KXobject Base Compiler
  Compiler of Kreatrix code
  Object that represents compiler. Main purpose is compiling Kreatrix code to bytecode.
*/

char * create_kxc_filename(char *kxfilename);

static void kxcompiler_add_method_table(KxCompiler *self);

KxCompiler * 
kxcompiler_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	kxobject_set_type(self,"Compiler");

	kxcompiler_add_method_table(self);

	return self;
}


/*KXdoc _compileFileToByteArray: aFilename
 Compile file into bytecode.
 Compile file and return bytecode as bytearray.
*/
static KxObject *
kxcompiler_compile_file_to_bytearray(KxCompiler *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param,0);


	char *bytecode;
	int size;
	// TODO: Set compile_docs flag
	List *errors = kxc_compile_file(param,0, kx_level_of_optimisation, &bytecode, &size);
	if (errors) {
		KxException *excp = kxexception_new_with_text(KXCORE,errors->items[0]);
		// TODO: save errors
		list_free_all(errors);
		KXTHROW(excp);
	}
	
	return KXBYTEARRAY(bytearray_new_set(bytecode, size));
}

/*KXdoc _compileStringToByteArray:withSourceFilename: code aFilename
  Compile string into bytecode.
 Compile string and return bytecode as bytearray.

*/
static KxObject *
kxcompiler_compile_string_to_bytearray(KxCompiler *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(code,0);
	KXPARAM_TO_CSTRING(filename,1);

	char *bytecode;
	int size;
	List *errors = kxc_compile_string(code,filename,0, kx_level_of_optimisation, &bytecode, &size);
	if (errors) {
		KxException *excp = kxexception_new_with_text(KXCORE,errors->items[0]);
		list_free_all(errors);
		// TODO: Throw "CompilationException"
		KXTHROW(excp);
	}
	
	return KXBYTEARRAY(bytearray_new_set(bytecode, size));
}

static KxObject *
kxcompiler_bytecode_to_codeblock(KxCompiler *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(source_filename, 1);

	KxObject *param = message->params[0];
	if (!IS_KXBYTEARRAY(param)) {
		KXTHROW_EXCEPTION("First parameter must be array with bytecode");
	}
	ByteArray *bytearray = KXBYTEARRAY_DATA(param);
	if (bytearray->array == NULL) {
		KXRETURN(KXCORE->object_nil);
	}
	char *bytecode = bytearray->array;
	
	return kxcodeblock_new_from_bytecode(KXCORE, &bytecode, source_filename);
}

/*KXdoc errors
  Return errors from compilation.
  Returns list of strings with error's messages from last compilation.
  If no errors occurs, empty list is returned.
*/
static KxObject *
kxcompiler_errors(KxCompiler *self, KxMessage *message)
{
	// TODO:
	//KXRETURN(self);
	KXTHROW_EXCEPTION("Not implemented");
}

static void 
kxcompiler_add_method_table(KxCompiler *self)
{
	KxMethodTable table[] = {
		{"_compileFileToByteArray:",1, kxcompiler_compile_file_to_bytearray },
		{"_compileStringToByteArray:withSourceFilename:",2, kxcompiler_compile_string_to_bytearray },
		{"_bytecodeToCodeBlock:sourceFilename:",2, kxcompiler_bytecode_to_codeblock },
		{"errors",0, kxcompiler_errors },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}



