/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "kxlobby.h"
#include "kxobject.h"
#include "kxcore.h"
#include "compiler/kxcompiler.h"
#include "kxvm.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxbytecode.h"
#include "kxcodeblock.h"
#include "kxcompiler.h"
#include "kxcompile_utils.h"

//static KxObjectExtension ext;

char * create_kxc_filename(char *kxfilename);


// TODO: use standard function for do_file
static KxObject *
kxlobby_do_file(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_CSTRING(param,0);
	/*char *kxc_filename = create_kxc_filename(param);

	//printf("out = %s\n", kxc_filename);
	int size;
	List *errors;
	errors = kxc_compile_and_save_file(param,kxc_filename,0, &size);
	if (errors) {
		KxException *excp = kxexception_new_with_text(KXCORE,errors->items[0]);
		kxc_reset_error_list();
		free(kxc_filename);
		KXTHROW(excp);
	}
	char *source_filename;
	int bytecode_size;
	char *bytecode = bytecode_load_from_file(kxc_filename, &bytecode_size,&source_filename);
	if (bytecode == NULL) {
		free(kxc_filename);
		// TODO: Full error message
		KxException *excp = kxexception_new_with_text(KXCORE,"Problem in loading bytecode");
		KXTHROW(excp);
	}
	free(kxc_filename);
	
	KxCodeBlock *codeblock;
	char *bytecode_pos = bytecode;
	codeblock = kxcodeblock_new_from_bytecode(KXCORE, &bytecode_pos, source_filename);
	//REF_ADD(codeblock);

	free(source_filename);
	free(bytecode);

	//kxobject_mark(codeblock);
	//kxgc_collect(core);
	//KxSymbol *start_sym = kxcore_get_symbol(core,"init");

	KxSymbol *init_message = kxcore_get_symbol(KXCORE,"start");
	KxMessage *start_msg = kxmessage_new(init_message, KXCORE->lobby);
	KxObject *retobj = kxcodeblock_run(codeblock,KXCORE->lobby, start_msg, KXCB_RUN_NORMAL);
	kxmessage_free(start_msg);
	REF_REMOVE(init_message);
	REF_REMOVE(codeblock);
	return retobj;*/
	return kxcompile_do_file(param, KXCORE->lobby, message->message_name);

}

KxObject *kxlobby_new(KxCore *core) 
{


	KxObject *self = kxcore_clone_base_object(core);

	/** TMP */

	/*kxobjectext_init(&ext);
	ext.type_name = "Lobby";

	self->extension = &ext;*/


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Lobby"), self);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Object"), core->base_object);

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Integer"), 
		kxcore_get_basic_prototype(core,KXPROTO_INTEGER));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Float"), 
		kxcore_get_basic_prototype(core,KXPROTO_FLOAT));


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"String"), 
		kxcore_get_basic_prototype(core,KXPROTO_STRING));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Symbol"), 
		kxcore_get_basic_prototype(core,KXPROTO_SYMBOL));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Exception"), 
		kxcore_get_basic_prototype(core,KXPROTO_EXCEPTION));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"List"), 
		kxcore_get_basic_prototype(core,KXPROTO_LIST));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"File"), 
		kxcore_get_basic_prototype(core,KXPROTO_FILE));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Character"), 
		kxcore_get_basic_prototype(core,KXPROTO_CHARACTER));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Dictionary"), 
		kxcore_get_basic_prototype(core,KXPROTO_DICTIONARY));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Set"), 
		kxcore_get_basic_prototype(core,KXPROTO_SET));


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"DynLib"), 
		kxcore_get_basic_prototype(core,KXPROTO_DYNLIB));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Module"), 
		kxcore_get_basic_prototype(core,KXPROTO_MODULE));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"CFunction"), 
		kxcore_get_basic_prototype(core,KXPROTO_CFUNCTION));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"CodeBlock"), 
		kxcore_get_basic_prototype(core,KXPROTO_CODEBLOCK));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"ByteArray"), 
		kxcore_get_basic_prototype(core,KXPROTO_BYTEARRAY));


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"ScopedBlock"), 
		kxcore_get_basic_prototype(core,KXPROTO_SCOPEDBLOCK));


	kxobject_set_slot_no_ref2(self,kxcore_get_symbol(core,"VM"), kxvm_new(core));

	kxobject_set_slot_no_ref2(self,kxcore_get_symbol(core,"Compiler"), kxcompiler_new_prototype(core));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Lobby"), self);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"true"), core->object_true);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"false"), core->object_false);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"nil"), core->object_nil);


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stdout"), core->object_stdout);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stdin"), core->object_stdin);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stderr"), core->object_stderr);
	
	KxMethodTable table[] = {
		{"doFile:",1, kxlobby_do_file },
		{NULL,0,NULL}
	};
	kxobject_add_methods(self, table);





	return self;
}
