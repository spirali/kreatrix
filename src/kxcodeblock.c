/*
   kxcodeblock.c
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
#include "kxdecompiler.h"

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
	kxcodeblock_extension.is_immutable = 1;
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
kxcodeblock_clean_inline_cache(KxCodeBlock *self)
{
		KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
		int t;
		for (t=0; t<data->inline_cache_size; t++) {
			KxCodeBlockInlineCache *ic = &data->inline_cache[t];
			if (ic->prototype) {
				REF_REMOVE(ic->prototype);
				REF_REMOVE(ic->cached_object);
				REF_REMOVE(ic->slot_holder);
			}
			if (ic->message_name)
				REF_REMOVE(ic->message_name);
		}
		kxfree(data->inline_cache);

		if (KXCORE->first_codeblock_with_inline_cache == self) {
			KXCORE->first_codeblock_with_inline_cache = data->next_codeblock_with_inline_cache;
			if (data->next_codeblock_with_inline_cache == NULL) {
				KXCORE->last_codeblock_with_inline_cache = NULL;
			}
		} else {
			KxObject *obj = KXCORE->first_codeblock_with_inline_cache;
			KxCodeBlockData *objdata = KXCODEBLOCK_DATA(obj);

			while(objdata->next_codeblock_with_inline_cache != self) {
				obj = objdata->next_codeblock_with_inline_cache;
				objdata = KXCODEBLOCK_DATA(obj);
			}

			objdata->next_codeblock_with_inline_cache = data->next_codeblock_with_inline_cache;

			if (data->next_codeblock_with_inline_cache == NULL) {
				KXCORE->last_codeblock_with_inline_cache = obj;
			}
		}


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

	if (data->symbol_frame) {
		for (t=0;t<data->symbol_frame_size;t++) {
			REF_REMOVE(data->symbol_frame[t]);
		}
		kxfree(data->symbol_frame);
		data->symbol_frame = NULL;
	}

	if (data->inline_cache)
	{
		kxcodeblock_clean_inline_cache(self);
		data->inline_cache = NULL;
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

	if (data->inline_cache)
	{
		kxcodeblock_clean_inline_cache(self);
	}
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

	data->code[0] = KXCI_RETURN_SELF;

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

	//kxcodeblock_insert_inline_cache_instructions(codeblock);
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

	/*printf("source file: %s %i\n", data->source_filename, data->message_linenumbers[0]);
	List *list = kxdecompiler_instructions_list(self);
	kxdecompiler_dump_instructions(list);
	list_foreach(list, (ListForeachFcn*)kxinstructionwrapper_free);
	list_free(list);*/
	
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

int
kxcodeblock_message_name_id(KxCodeBlock *self, KxSymbol *message_name)
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	int t;
	for (t=0;t<data->symbol_frame_size;t++) {
		if (data->symbol_frame[t] == message_name) {
			return t;
		}
	}
	fprintf(stderr,"Internal error: kxcodeblock_message_name_id: message not found");
	abort();
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

	if (data->inline_cache)
	{
		int t;
		for (t=0; t<data->inline_cache_size; t++) {
			KxCodeBlockInlineCache *ic = &data->inline_cache[t];
			if (ic->prototype) {
				kxobject_mark(ic->prototype);
				kxobject_mark(ic->cached_object);
				kxobject_mark(ic->slot_holder);
			}
			if (ic->message_name)
				kxobject_mark(ic->message_name);
		}
	}

}

void
kxcodeblock_insert_inline_cache_instructions(KxCodeBlock *self)
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	if (data->inline_cache) {
		return;
	}

	List *ilist = kxdecompiler_instructions_list(self);

	KxSymbol *symbols[ilist->size];
	
	int changed = 0;
	int t;

	for (t = 0; t < ilist->size; ++t)
	{
		KxInstructionWrapper *iw = ilist->items[t];
		switch (iw->type) {
			case KXCI_UNARY_MSG:
			case KXCI_BINARY_MSG:
			case KXCI_KEYWORD_MSG:
			case KXCI_LOCAL_UNARY_MSG:
			case KXCI_LOCAL_KEYWORD_MSG:
			{
				symbols[changed] = data->symbol_frame[(int)iw->params[0]];
				iw->type = KXCI_UNARY_MSG_C + (iw->type - KXCI_UNARY_MSG);
				iw->params[0] = changed;
				changed++;
				break;
			}
			default:
				break;
		}
	}

	if (changed == 0) {
		for (t=0; t < ilist->size; t++) {
			kxinstructionwrapper_free(ilist->items[t]);
		}
		list_free(ilist);
		return;
	}

	KxCodeBlockInlineCache *icache = kxcalloc(changed, sizeof(KxCodeBlockInlineCache) + sizeof(KxCodeBlockInlineCache*));
	ALLOCTEST(icache);
	
	for (t=0; t<changed; t++) {
		REF_ADD(symbols[t]);
		icache[t].message_name = symbols[t];
	}

	data->inline_cache = icache;
	data->inline_cache_size = changed;

	kxdecompiler_instruction_wrapper_to_bytecode(ilist, data->code);

	for (t=0; t < ilist->size; t++) {
		kxinstructionwrapper_free(ilist->items[t]);
	}
	list_free(ilist);

	if (KXCORE->last_codeblock_with_inline_cache) {
		KxCodeBlockData *next = KXCODEBLOCK_DATA(KXCORE->last_codeblock_with_inline_cache);
		next->next_codeblock_with_inline_cache = self;
		KXCORE->last_codeblock_with_inline_cache = self;
	} else {
		KXCORE->last_codeblock_with_inline_cache = self;
		KXCORE->first_codeblock_with_inline_cache = self;
	}
}

void 
kx_inline_cache_repair_prototype(KxObject *prototype)
{
	if (kxobject_recursive_mark_test(prototype))
		return;

	kxobject_recursive_mark_set(prototype);

	KxCore *core = KXCORE_FROM(prototype);
	
	KxObject *obj = core->first_codeblock_with_inline_cache;

	while(obj) {
		KxCodeBlockData *data = KXCODEBLOCK_DATA(obj);
		KxCodeBlockInlineCache *ic = data->inline_cache;
		int size = data->inline_cache_size;
		
		int t;
		for (t=0; t < size; t++) {
			if (ic[t].prototype == prototype) {
				REF_REMOVE(ic[t].prototype);
				ic[t].prototype = NULL;
				REF_REMOVE(ic[t].cached_object);
				ic[t].cached_object = NULL;
			}
		}

		obj = data->next_codeblock_with_inline_cache;
	}

	int t;
	List *list = prototype->profile->child_prototypes;
	for (t=0; t < list->size; t++) {
		kx_inline_cache_repair_prototype(list->items[t]);
	}

	kxobject_recursive_mark_reset(prototype);
}

void 
kx_inline_cache_repair_prototype_and_name(KxObject *prototype, KxSymbol *message_name)
{
	if (kxobject_recursive_mark_test(prototype))
		return;

	kxobject_recursive_mark_set(prototype);


	KxCore *core = KXCORE_FROM(prototype);
	
	KxObject *obj = core->first_codeblock_with_inline_cache;

	while(obj) {
		KxCodeBlockData *data = KXCODEBLOCK_DATA(obj);
		KxCodeBlockInlineCache *ic = data->inline_cache;
		int size = data->inline_cache_size;
		
		int t;
		for (t=0; t < size; t++) {
			if (ic[t].prototype == prototype && ic[t].message_name == message_name) {
				REF_REMOVE(ic[t].prototype);
				ic[t].prototype = NULL;
				REF_REMOVE(ic[t].cached_object);
				ic[t].cached_object = NULL;
				REF_REMOVE(ic[t].slot_holder);
				ic[t].slot_holder = NULL;

			}
		}

		obj = data->next_codeblock_with_inline_cache;
	}


	int t;
	List *list = prototype->profile->child_prototypes;

	for (t=0; t < list->size; t++) {
		kx_inline_cache_repair_prototype_and_name(list->items[t], message_name);
	}

	kxobject_recursive_mark_reset(prototype);
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

static KxObject *
kxcodeblock_literals(KxCodeBlock *self, KxMessage *message) 
{
	KxCodeBlockData *data = self->data.ptr;
	if (data == NULL) {
	 	List *list = list_new();
		return KXLIST(list);
	}
	List *list = list_new_size(data->literals_count);
	int t;
	for (t=0;t<data->literals_count;t++) {
		list->items[t] = data->literals[t];
		REF_ADD(data->literals[t]);
	}
	return KXLIST(list);
}

static KxObject *
kxcodeblock_locals(KxCodeBlock *self, KxMessage *message) 
{
	KxCodeBlockData *data = self->data.ptr;
	if (data == NULL) {
	 	List *list = list_new();
		return KXLIST(list);
	}
	List *list = list_new_size(data->locals_count);
	int t;
	for (t=0;t<data->locals_count;t++) {
		list->items[t] = data->locals_symbols[t];
		REF_ADD((KxObject*)list->items[t]);
	}
	return KXLIST(list);
}

static KxObject *
kxcodeblock_message_names(KxCodeBlock *self, KxMessage *message) 
{
	KxCodeBlockData *data = self->data.ptr;
	if (data == NULL) {
	 	List *list = list_new();
		return KXLIST(list);
	}
	List *list = list_new_size(data->symbol_frame_size);
	int t;
	for (t=0;t<data->symbol_frame_size;t++) {
		list->items[t] = data->symbol_frame[t];
		REF_ADD((KxObject*)list->items[t]);
	}
	return KXLIST(list);
}

static KxObject *
kxcodeblock_insert_inline_cache(KxCodeBlock *self, KxMessage *message)
{
	kxcodeblock_insert_inline_cache_instructions(self);
	KXRETURN(self);
}

static void 
kxcodeblock_add_method_table(KxCodeBlock *self)
{
	KxMethodTable table[] = {
		{"params",0, kxcodeblock_params },
		{"literals",0, kxcodeblock_literals },
		{"locals",0, kxcodeblock_locals },
		{"messageNames",0, kxcodeblock_message_names },
		{"insertInlineCache",0, kxcodeblock_insert_inline_cache },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
