/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kxcodeblock.h"
#include "kxobject.h"
#include "kxcore.h"
#include "utils/utils.h"
#include "kxactivation.h"
#include "kxexception.h"
#include "kxlist.h"
#include "kxinteger.h"
#include "kxcharacter.h"
#include "kxfloat.h"
#include "compiler/kxinstr.h"

#define GET_BYTECODE_CHAR *((*bytecode)++)

KxObjectExtension kxcodeblock_extension;

static void kxcodeblock_free(KxCodeBlock *self);
static void kxcodeblock_clean(KxCodeBlock *self);
static void kxcodeblock_mark(KxObject *self);
static KxCodeBlockData *kxcodeblock_data_new_return_self();

static void 
kxcodeblock_add_method_table(KxCodeBlock *self);

void 
kxcodeblock_init_extension()
{
	kxobjectext_init(&kxcodeblock_extension);
	kxcodeblock_extension.type_name = "CodeBlock";
	kxcodeblock_extension.free = kxcodeblock_free;
	kxcodeblock_extension.clean = kxcodeblock_clean;
	kxcodeblock_extension.activate = kxcodeblock_run;
	kxcodeblock_extension.mark = kxcodeblock_mark;
}


KxObject *
kxcodeblock_new_prototype(KxCore *core) {
	KxObject *codeblock = kxcore_clone_base_object(core);
	codeblock->extension = &kxcodeblock_extension;
	codeblock->data.ptr = kxcodeblock_data_new_return_self();
	kxcodeblock_add_method_table(codeblock);
	return codeblock;
}

static KxCodeBlock *
kxcodeblock_new(KxCore *core) {
	KxObject *prototype = kxcore_get_basic_prototype(core, KXPROTO_CODEBLOCK);
	KxCodeBlock *codeblock =  kxobject_raw_clone(prototype);
	
	codeblock->data.ptr = kxcalloc(1,sizeof(KxCodeBlockData));
	ALLOCTEST(codeblock->data.ptr);

	return codeblock;
}

static void
kxcodeblock_clean(KxCodeBlock *self) {
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	int t;
	if (data->literals) {
		for (t=0; t<data->literals_count; t++) {
			REF_REMOVE(data->literals[t]);
		}
		kxfree(data->literals);
		data->literals = NULL;
	}
}

static void
kxcodeblock_free(KxCodeBlock *self) {

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	kxfree(data->source_filename); 

	if (data->message_linenumbers)
		kxfree(data->message_linenumbers);

	int t;
	for (t=0;t<data->subcodeblocks_size;t++)
		REF_REMOVE(data->subcodeblocks[t]);
	
	if (data->symbol_frame) {
		for (t=0;t<data->symbol_frame_size;t++) {
			REF_REMOVE(data->symbol_frame[t]);
		}
		kxfree(data->symbol_frame);
	}

	if (data->literals) {
		for (t=0; t<data->literals_count; t++) {
			REF_REMOVE(data->literals[t]);
		}
		kxfree(data->literals);
	}

	
	if (data->subcodeblocks)
		kxfree(data->subcodeblocks);

	if (data->locals_symbols)
		kxfree(data->locals_symbols);

	
	if (data->code)
		kxfree(data->code);
	
	if (data->prealocated_locals) 
		kxfree(data->prealocated_locals);
	kxfree(data);
}

KxCodeBlockData *
kxcodeblock_data_new_return_self() 
{
	KxCodeBlockData *data;

	data = kxcalloc(1, sizeof(KxCodeBlockData));
	ALLOCTEST(data);

	data->type = KXCODEBLOCK_METHOD;
	data->source_filename = strdup("<build-in>");
	data->code = kxmalloc(1);
	ALLOCTEST(data->code);

	data->code[0] = KXCI_RETURNSELF;

	return data;
}

static void 
kxcodeblock_read_symbol_frame (KxCodeBlock *self, char **bytecode)
{
	int size = GET_BYTECODE_CHAR;

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	data->symbol_frame_size = size;

	if (size == 0) {
		data->symbol_frame = NULL;
		return;
	}


	data->symbol_frame = kxmalloc(sizeof(KxSymbol*) * size);
	ALLOCTEST(data->symbol_frame);

	int t;
	for (t=0;t<size;t++) {
		char *symbol = *bytecode;
	
		PDEBUG("Symbol: %s\n",symbol);

		data->symbol_frame[t] = kxcore_get_symbol(KXCORE, symbol);

		*bytecode += strlen(symbol)+1;
	}
}

static void 
kxcodeblock_read_literals (KxCodeBlock *self, char **bytecode)
{
	int size = GET_BYTECODE_CHAR;

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	data->literals_count = size;
	data->literals = kxmalloc(sizeof(KxObject*) * size);
	ALLOCTEST(data->literals);

	int t;
	for (t=0;t<size;t++) {
		KxcLiteralType type = GET_BYTECODE_CHAR;
		switch (type) {
			case KXC_LITERAL_SYMBOL: {
				char *symbol = *bytecode;
				data->literals[t] = kxcore_get_symbol(KXCORE, symbol);
				*bytecode += strlen(symbol)+1;
				continue;
			}	
			case KXC_LITERAL_STRING: {
				char *string = *bytecode;
				data->literals[t] = KXSTRING(string);
				*bytecode += strlen(string)+1;
				continue;
			}		
			case KXC_LITERAL_INTEGER: {
				int integer = *((int*)*bytecode);
				*bytecode += sizeof(int);
				data->literals[t] = KXINTEGER(integer);
				continue;
			} 
			case KXC_LITERAL_CHAR: {
				int charval = *((int*)*bytecode);
				*bytecode += sizeof(int);
				data->literals[t] = KXCHARACTER(charval);
				continue;
			} 
			case KXC_LITERAL_FLOAT: {
				double floatval = *((double*)*bytecode);
				*bytecode += sizeof(double);
				data->literals[t] = KXFLOAT(floatval);
				continue;
			} 

		}
	}
}

static void
kxcodeblock_read_params(KxCodeBlock *self, char **bytecode) 
{

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	data->params_count = GET_BYTECODE_CHAR;
}

static void
kxcodeblock_read_localslots(KxCodeBlock *self, char **bytecode) 
{
	int size = GET_BYTECODE_CHAR;
	
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	data->locals_count = size;

	if (size == 0) {
		data->locals_symbols = NULL;
		return;
	}

	data->locals_symbols = kxmalloc(sizeof(KxSymbol*) * size);
	ALLOCTEST(data->locals_symbols);

	int t;
	for (t=0;t<size;t++) {
		char pos = GET_BYTECODE_CHAR;
		data->locals_symbols[t] = data->symbol_frame[(int)pos];
		PDEBUG("Locals[%i]:%s\n",t,KXSYMBOL_AS_CSTRING(data->locals_symbols[t]));
	}
}

static void
kxcodeblock_init_data(KxCodeBlock *self, char **bytecode, char *source_filename)
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	data->source_filename = strdup(source_filename);

	char c = GET_BYTECODE_CHAR;
	if (c == 0)
		data->type = KXCODEBLOCK_METHOD;
	else
		data->type = KXCODEBLOCK_BLOCK;

	kxcodeblock_read_symbol_frame(self,bytecode);
}

static void
kxcodeblock_read_code(KxCodeBlock *self, char **bytecode) 
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	
	int size = *((int*)*bytecode);
	*bytecode += sizeof(int);

	PDEBUG("Code size = %i\n",size);

	if (size == 0) {
		data->code = NULL;
		return;
	}

	data->code = kxmalloc(size);
	ALLOCTEST(data->code);

	memcpy(data->code,*bytecode,size);
	*bytecode += size;

}

KxCodeBlock *
kxcodeblock_read_subblock(KxCore *core, char **bytecode, KxCodeBlock *parent_codeblock, char *source_filename);


static void
kxcodeblock_read_subblocks(KxCodeBlock *self, char **bytecode, char *source_filename) 
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);


	int count = GET_BYTECODE_CHAR;
	data->subcodeblocks_size = count; 


	PDEBUG("Subblocks count = %i\n",count);

	if (count == 0) {
		data->subcodeblocks = NULL;
		return;
	}

	data->subcodeblocks = kxmalloc(count * sizeof(KxCodeBlock*));
	ALLOCTEST(data->subcodeblocks);

	int t;

	for (t=0;t<count;t++) {
		KxCodeBlock *codeblock = kxcodeblock_read_subblock(KXCORE, bytecode, self, source_filename);		
		data->subcodeblocks[t] = codeblock;
	}
}

static int 
kxcodeblock_locals_total_count(KxCodeBlock *self)
{	
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	int count = data->locals_count;
	int t;
	for (t=0;t<data->subcodeblocks_size;t++) {
		count += kxcodeblock_locals_total_count(data->subcodeblocks[t]);
	}
	return count;
}


static void
kxcodeblock_read_message_linenumbers(KxCodeBlock *self, char **bytecode)
{
	int size = *(int*)(*bytecode);
	(*bytecode) += sizeof(int);

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	if (size == 0) {
		data->message_linenumbers = NULL;
		return;
	}

	data->message_linenumbers = kxmalloc(sizeof(int) * size);
	ALLOCTEST(data->message_linenumbers);

	int t;
	for (t=0;t<size;t++) {
		data->message_linenumbers[t] = *(int*)(*bytecode);
		(*bytecode) += sizeof(int);
	}
}

KxCodeBlock *
kxcodeblock_read_subblock(KxCore *core, char **bytecode, KxCodeBlock *parent_codeblock, char *source_filename)
{

	KxCodeBlock *codeblock = kxcodeblock_new(core);
	
	kxcodeblock_init_data(codeblock, bytecode,source_filename);
	
	kxcodeblock_read_literals(codeblock, bytecode);

	kxcodeblock_read_params(codeblock, bytecode);

	kxcodeblock_read_localslots(codeblock, bytecode);

	kxcodeblock_read_message_linenumbers(codeblock, bytecode);

	kxcodeblock_read_code(codeblock, bytecode);
	
	kxcodeblock_read_subblocks(codeblock, bytecode, source_filename);

	return codeblock;
}


KxCodeBlock *
kxcodeblock_new_from_bytecode(KxCore *core, char **bytecode, char *source_filename)
{
	KxCodeBlock *codeblock = kxcodeblock_read_subblock(core, bytecode, NULL, source_filename);
	return codeblock;
}

inline static KxObject *
kxcodeblock_run_activation(KxCodeBlock *self, KxObject *target, KxActivation *activation, KxMessage *message)
{
	KxCodeBlockData *data = self->data.ptr;
	activation->codeblock = self;

	REF_ADD(activation->codeblock);
	
	if (message->message_name) {
		activation->message_name = message->message_name;
		REF_ADD(activation->message_name);
	}

	// TODO: REF_ADD ref add must be only in method, not in block
	activation->receiver = target;
	REF_ADD(activation->receiver);

	activation->slot_holder_of_codeblock = message->slot_holder;
	REF_ADD(activation->slot_holder_of_codeblock);

	if (data->prealocated_locals) {
		activation->locals = data->prealocated_locals;
		data->prealocated_locals = NULL;
	} else {
		activation->locals = kxmalloc(sizeof(KxObject *) * data->locals_count);
	}


	int t;
	for (t=0; t<message->params_count;t++) {
		KxObject *param = message->params[t];
		REF_ADD(param);
		activation->locals[t] = param;
	}

	for (t=message->params_count; t<data->locals_count;t++) {
		REF_ADD(KXCORE->object_nil);
		activation->locals[t] = KXCORE->object_nil;
	}




	KxObject * retobj = kxactivation_run(activation);

	activation->ref_count--;
	if (activation->ref_count == 0) {
		kxactivation_free(activation);
	}

	return retobj;
}

KxObject *
kxcodeblock_run_scoped(KxCodeBlock *self, KxActivation *parent_activation, KxMessage *message)
{
	KxCodeBlockData *data = self->data.ptr;
	
	if (data->params_count != message->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"CodeBlock: Invalid count of parameters %i (%i expected)\n",
			message->params_count, data->params_count);
		KXTHROW(excp);
	}


	KxActivation *activation = kxactivation_new(KXCORE);

	activation->parent = parent_activation;
	
	return kxcodeblock_run_activation(self, parent_activation->receiver, activation, message);

}


KxObject *  
kxcodeblock_run(KxCodeBlock *self, KxObject *target, KxMessage *message)
{

	KxCodeBlockData *data = self->data.ptr;
	
	if (data->params_count != message->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"CodeBlock: Invalid count of parameters %i (%i expected)\n",
			message->params_count, data->params_count);
		KXTHROW(excp);
	}


	KxActivation *activation = kxactivation_new(KXCORE);	
	
	activation->parent = NULL;
	
	return kxcodeblock_run_activation(self, target, activation, message);
}

static void
kxcodeblock_mark(KxObject *self) 
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	if (!data) 
		return;
	
	int t;
	for (t=0;t<data->symbol_frame_size;t++) {
		kxobject_mark(data->symbol_frame[t]);
	}

	for (t=0;t<data->literals_count;t++) {
		kxobject_mark(data->literals[t]);
	}

	for (t=0;t<data->subcodeblocks_size;t++) {
		kxobject_mark(data->subcodeblocks[t]);
	}
}

static KxObject *
kxcodeblock_params(KxCodeBlock *self, KxMessage *message) 
{
	KxCodeBlockData *data = self->data.ptr;
	if (data == NULL) {
	 	List *list = list_new();
		return KXLIST(list);
	}
	List *list = list_new_size(data->params_count);
	int t;
	for (t=0;t<data->params_count;t++) {
		list->items[t] = data->locals_symbols[t];
		REF_ADD(data->locals_symbols[t]);
	}
	return KXLIST(list);
}

static void 
kxcodeblock_add_method_table(KxCodeBlock *self)
{
	KxMethodTable table[] = {
		{"params",0, kxcodeblock_params },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
