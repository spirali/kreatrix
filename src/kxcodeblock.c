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

#define GET_BYTECODE_CHAR *((*bytecode)++)

KxObjectExtension kxcodeblock_extension;

static void kxcodeblock_free(KxCodeBlock *self);

void kxcodeblock_mark(KxObject *self);
KxObject * kxcodeblock_activate(KxCodeBlock *self, KxObject *target, KxMessage *message);

static void 
kxcodeblock_add_method_table(KxCodeBlock *self);



// TODO: proto object data!=NULL, condtions eliminationg
void 
kxcodeblock_init_extension()
{
	kxobjectext_init(&kxcodeblock_extension);
	kxcodeblock_extension.type_name = "CodeBlock";
	kxcodeblock_extension.free = kxcodeblock_free;
	kxcodeblock_extension.activate = kxcodeblock_activate;
	kxcodeblock_extension.mark = kxcodeblock_mark;
}


KxObject *
kxcodeblock_new_prototype(KxCore *core) {
	KxObject *codeblock = kxcore_clone_base_object(core);
	codeblock->extension = &kxcodeblock_extension;
	kxcodeblock_add_method_table(codeblock);
	return codeblock;
}

KxCodeBlock *
kxcodeblock_new(KxCore *core) {
	KxObject *prototype = kxcore_get_basic_prototype(core, KXPROTO_CODEBLOCK);
	KxCodeBlock *codeblock =  kxobject_raw_clone(prototype);
	
	codeblock->data.ptr = kxcalloc(1,sizeof(KxCodeBlockData));
	ALLOCTEST(codeblock->data.ptr);

	return codeblock;
}

static void
kxcodeblock_free(KxCodeBlock *self) {

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	if (!data)
		return;



	kxfree(data->source_filename); 
	kxfree(data->message_linenumbers);

	int t;
	for (t=0;t<data->subcodeblocks_size;t++)
		REF_REMOVE(data->subcodeblocks[t]);
	
	/*if (data->parent_codeblock)
		REF_REMOVE(data->parent_codeblock);*/

	for (t=0;t<data->symbol_frame_size;t++) {
		REF_REMOVE(data->symbol_frame[t]);
	}
	kxfree(data->symbol_frame);
	
	if (data->subcodeblocks)
		kxfree(data->subcodeblocks);

	if (data->params_symbols)
		kxfree(data->params_symbols);

	if (data->localslots_symbols)
		kxfree(data->localslots_symbols);

	
	if (data->code)
		kxfree(data->code);
	kxfree(data);
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

	//	REF_ADD(data->symbol_frame[t]);

		*bytecode += strlen(symbol)+1;
	}
}

static void
kxcodeblock_read_params(KxCodeBlock *self, char **bytecode) 
{
	int size = GET_BYTECODE_CHAR;
	
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	data->params_count = size;
	
	if (size == 0) {
		data->params_symbols = NULL;
		return;
	}

	data->params_symbols = kxmalloc(sizeof(KxSymbol*) * size);
	ALLOCTEST(data->params_symbols);

	int t;
	for (t=0;t<size;t++) {
		char pos = GET_BYTECODE_CHAR;
		data->params_symbols[t] = data->symbol_frame[(int)pos];
		PDEBUG("Param[%i]:%s\n",t,KXSYMBOL_AS_CSTRING(data->params_symbols[t]));
	}
}

static void
kxcodeblock_read_localslots(KxCodeBlock *self, char **bytecode) 
{
	int size = GET_BYTECODE_CHAR;
	
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	data->localslots_count = size;
	
	if (size == 0) {
		data->localslots_symbols = NULL;
		return;
	}

	data->localslots_symbols = kxmalloc(sizeof(KxSymbol*) * size);
	ALLOCTEST(data->localslots_symbols);

	int t;
	for (t=0;t<size;t++) {
		char pos = GET_BYTECODE_CHAR;
		data->localslots_symbols[t] = data->symbol_frame[(int)pos];
		PDEBUG("Localslots[%i]:%s\n",t,KXSYMBOL_AS_CSTRING(data->localslots_symbols[t]));
	}
}

static void
kxcodeblock_init_data(KxCodeBlock *self, char **bytecode, char *source_filename)
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

	data->source_filename = strdup(source_filename);

	/*if (**bytecode == -1) {
		PDEBUG("new block\n");
		// BLOCK
		if (parent_codeblock == NULL) {
			fprintf(stderr,"Internal error: kxcodeblock_new_from_bytecode: parent_codeblock is NULL");
			exit(-1);
		}
*/
		/* !! TEMPORATY - proti leaku dokud nebude GC, az bude GC zapnout !! */
		//REF_ADD(parent_codeblock);
		//data->parent_codeblock = parent_codeblock;
/*
		data->type = KXCODEBLOCK_BLOCK;
		data->symbol_frame = KXCODEBLOCK_DATA(parent_codeblock)->symbol_frame;
		data->symbol_frame_size = -1;
		(*bytecode)++;
	} else {
		PDEBUG("new method\n");*/

	char c = GET_BYTECODE_CHAR;
	if (c == 0)
		data->type = KXCODEBLOCK_METHOD;
	else
		data->type = KXCODEBLOCK_BLOCK;
	//data->parent_codeblock = NULL;
	kxcodeblock_read_symbol_frame(self,bytecode);
//	}
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
	//	REF_ADD(codeblock);
		data->subcodeblocks[t] = codeblock;
	}
}

static void
kxcodeblock_read_message_linenumbers(KxCodeBlock *self, char **bytecode)
{
	int size = *(int*)(*bytecode);
	(*bytecode) += sizeof(int);

	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);

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


KxObject *  
kxcodeblock_run(KxCodeBlock *self, KxObject *target, KxMessage *message, KxCodeBlockRunFlag flag)
{

	KxCodeBlockData *data = self->data.ptr;
	
	if (data->params_count != message->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"CodeBlock: Invalid count of parameters %i (%i expected)\n",
			message->params_count, data->params_count);
		KXTHROW(excp);
	}


	KxActivation *activation = kxactivation_new(KXCORE); // = kxactivation_clone_with_new_data(prototype);

	if (flag == KXCB_RUN_SCOPED) {

		
		KxActivation *t_act = (KxActivation*) target->data.ptr;
		if (t_act->long_return) {
			activation->long_return = t_act->long_return;
		} else {
			activation->long_return = t_act;
		}

	    activation->locals = kxobject_raw_clone(target);
		activation->locals->extension = NULL;
		activation->locals->data.ptr = activation;
		target = t_act->receiver;
	} else {

		activation->locals = kxobject_new_from(target);
		activation->locals->extension = NULL;
		activation->locals->data.ptr = activation;
		kxobject_set_parent(activation->locals,target);

		kxobject_set_slot_with_flags(activation->locals,KXCORE->dictionary[KXDICT_SELF],target, KXOBJECT_SLOTFLAG_FREEZE);
	}

	activation->codeblock = self;

	REF_ADD(activation->codeblock);
	
	if (message->message_name) {
		activation->message_name = message->message_name;
		REF_ADD(activation->message_name);
	}
	//activation->stack = message->stack;
	activation->receiver = target;
	REF_ADD(activation->receiver);

	activation->slot_holder_of_codeblock = message->slot_holder;
	REF_ADD(activation->slot_holder_of_codeblock);


	activation->target = activation->locals;

	int t;
	for (t=0; t<data->localslots_count;t++) {
		kxobject_set_slot_with_flags(activation->locals,data->localslots_symbols[t],KXCORE->object_nil, KXOBJECT_SLOTFLAG_FREEZE);
	}

	for (t=0; t<message->params_count;t++) {
		kxobject_set_slot_with_flags(activation->locals,data->params_symbols[t],message->params[t], KXOBJECT_SLOTFLAG_FREEZE);
	}



	KxObject * retobj = kxactivation_run(activation);
	//REF_REMOVE(activation);
	
	/* BUG HERE! Locals free function needed */
	activation->ref_count--;
	if (activation->ref_count == 0) {
		kxactivation_free(activation);
	}

	return retobj;
}



KxObject *
kxcodeblock_activate(KxCodeBlock *self, KxObject *target, KxMessage *message) 
{
	KxCodeBlockData *data = self->data.ptr;
	if (data == NULL) {
		//return NULL;
		KXTHROW_EXCEPTION("Call proto codeblock is prohibited, TODO: Better solution of this");
	}
	return kxcodeblock_run(self,target, message, KXCB_RUN_NORMAL);
}

void
kxcodeblock_mark(KxObject *self) 
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	if (!data) 
		return;
	
	int t;
	for (t=0;t<data->symbol_frame_size;t++) {
		kxobject_mark(data->symbol_frame[t]);
	}


	for (t=0;t<data->subcodeblocks_size;t++) {
		kxobject_mark(data->subcodeblocks[t]);
	}

	/*if (data->parent_codeblock)
		kxobject_mark(data->parent_codeblock);*/
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
		list->items[t] = data->params_symbols[t];
		REF_ADD(data->params_symbols[t]);
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
