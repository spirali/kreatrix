
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "block.h"

#include "../utils/utils.h"

#define BYTECODE_WRITE_CHAR(x) { **bytecode = (x); (*bytecode)++; }
#define BYTECODE_WRITE_STRING(x) { strcpy(*bytecode,x); (*bytecode)+=strlen(x)+1; }
#define BYTECODE_WRITE_INT(x) { memcpy(*bytecode,&x,sizeof(int)); (*bytecode)+=sizeof(int); }
#define BYTECODE_WRITE_DOUBLE(x) { memcpy(*bytecode,&x,sizeof(double)); (*bytecode)+=sizeof(double); }

/// -- KxcInstruction --------------------------------------------

/**
 * Create new instruction
 */
static KxcInstruction*
kxcinstruction_new(KxInstructionType type)
{
	KxcInstruction *instruction = malloc(sizeof(KxcInstruction));
	ALLOCTEST(instruction);
	instruction->type = type;
	instruction->value.integer = 0;
	return instruction;
}

/**
 * Free instruction 
 */
static void
kxcinstruction_free(KxcInstruction *instruction)
{   
	if (instruction->type == KXCI_PUSH_STRING) {
		free(instruction->value.string);
	}
	free(instruction);
}

/**
 * Get size of instruction in bytecode
 */
static int 
kxcinstruction_bytecode_size(KxcInstruction *instruction) 
{
	int typesize = sizeof(char);
	// minimal size is 1 byte for instruction type
	switch(instruction->type) {
		case KXCI_UNARY_MSG:
		case KXCI_BINARY_MSG:
		case KXCI_LOCAL_UNARY_MSG:
		case KXCI_RESEND_UNARY_MSG:
			return typesize+sizeof(char); // symbol id

		case KXCI_KEYWORD_MSG:
		case KXCI_LOCAL_KEYWORD_MSG:
		case KXCI_RESEND_KEYWORD_MSG:
			return typesize+2*sizeof(char); // symbol id, params count

		case KXCI_PUSH_INTEGER:
			return typesize+sizeof(int);

		case KXCI_PUSH_STRING:
			return typesize+strlen(instruction->value.string)+1;

		case KXCI_PUSH_FLOAT:
			return typesize+sizeof(double);

		case KXCI_PUSH_SYMBOL:
			return typesize+sizeof(char); // symbol id

		case KXCI_PUSH_CHARACTER:
			return typesize+sizeof(int); 
	
		case KXCI_POP:
			return typesize;
		
		case KXCI_PUSH_METHOD:			
		case KXCI_PUSH_BLOCK:
			return typesize + sizeof(char); // block id

		case KXCI_LIST:
			return typesize+sizeof(int); // size of list

		case KXCI_END_OF_BLOCK:
		case KXCI_RETURN:
		case KXCI_LONGRETURN:
		case KXCI_RETURNSELF:
			return typesize;
	}
	fprintf(stderr,"Internal error, invalid instruction type");
	return typesize;
}

static char
params_count(char *name) 
{
	int count = 0;
	int t;
	for (t=0;t<strlen(name);t++) {
		if (name[t] == ':')
			count++;
	}
	return count;
}

/**
 * Write bytecode at pointer and move pointer after written bytes
 */
static void 
kxcinstruction_bytecode_write(KxcInstruction *instruction, char **bytecode, KxcBlock *block) 
{
	BYTECODE_WRITE_CHAR(instruction->type);
	int typesize = sizeof(char);

	switch(instruction->type) {
		case KXCI_UNARY_MSG:
		case KXCI_BINARY_MSG:
		case KXCI_LOCAL_UNARY_MSG:
		case KXCI_RESEND_UNARY_MSG:
		case KXCI_PUSH_SYMBOL:
			BYTECODE_WRITE_CHAR(instruction->value.symbol);
			return;

		case KXCI_KEYWORD_MSG: 
		case KXCI_LOCAL_KEYWORD_MSG: 
		case KXCI_RESEND_KEYWORD_MSG: {
			char symbol = instruction->value.symbol;
			BYTECODE_WRITE_CHAR(symbol);
			char *msg_name = list_get(block->symbols,(int)symbol);
			char params = params_count(msg_name);
			BYTECODE_WRITE_CHAR(params);
			return;
		}
		case KXCI_LIST:
			BYTECODE_WRITE_INT(instruction->value.list_size);
			return;

		case KXCI_PUSH_INTEGER:
			BYTECODE_WRITE_INT(instruction->value.integer);
			return;

		case KXCI_PUSH_CHARACTER:
			BYTECODE_WRITE_INT(instruction->value.charval);
			return;


		case KXCI_PUSH_STRING:
			BYTECODE_WRITE_STRING(instruction->value.string);
			return;

		case KXCI_PUSH_FLOAT:
            BYTECODE_WRITE_DOUBLE(instruction->value.floatval);
			return;

	
		case KXCI_POP:
		case KXCI_END_OF_BLOCK:
		case KXCI_RETURN:
		case KXCI_LONGRETURN:
		case KXCI_RETURNSELF:
			return; // No aditional data
	
		case KXCI_PUSH_METHOD:			
		case KXCI_PUSH_BLOCK:
			BYTECODE_WRITE_CHAR(instruction->value.codeblock);
			return;
	}

	fprintf(stderr,"Internal error, invalid instruction type");
}

/// -- KxcBlock --------------------------------------------------

static int kxcblock_get_symbol(KxcBlock *block, char *symbolname);
static void kxcblock_add_params(KxcBlock *block, List *parameters, List *errors);
static void kxcblock_add_localslots(KxcBlock *block, List *localslots, List *errors);

/**
 * Create new block
 * if parent is NULL, method is createn otherwise block is createn
 * paramaters is list of parameters's name
 */
struct KxcBlock*
kxcblock_new(int type, List *parameters, List *localslots, List *errors)
{
	struct KxcBlock *block = malloc(sizeof(KxcBlock));
	ALLOCTEST(block);
	

	block->symbols = list_new();

	block->type = type;

	//block->params_list = parameters;
	
	kxcblock_add_params(block,parameters, errors);
	kxcblock_add_localslots(block,localslots, errors);

	block->code = list_new();
	block->subblocks = list_new();
	block->message_linenumbers = list_new();
	return block;
}

// Free block and all subblocks recursively
void
kxcblock_free(KxcBlock *block)
{
	
	if (block->type == KXCBLOCK_TYPE_METHOD) {
		int t;
		for (t=0;t<block->symbols->size;t++) {
			PDEBUG("symbol=%s\n",block->symbols->items[t]);
		}
	}
	
	// Free subblocks
	list_foreach(block->subblocks, (ListForeachFcn*) &kxcblock_free);
	list_free(block->subblocks);

	list_free_all(block->symbols);

	if (block->params)
		free(block->params);

	if (block->localslots)
		free(block->localslots);


	// Free instructions
	list_foreach(block->code, (ListForeachFcn*) &kxcinstruction_free);
	list_free(block->code);
	
	list_free(block->message_linenumbers);

	free(block);
}

/**
 *	Add new child subblock into block.
 *	Parameter type is type of new block
 *	Returns subblock-id of new block
 */
static int
kxcblock_add_subblock(KxcBlock *parent, KxcBlock *child) 
{
	list_append(parent->subblocks, child);
	return parent->subblocks->size - 1;
}


/**
 *  Add symbol into symbol table of block
 *  return id of new symbol
 */
static int
kxcblock_add_symbol(KxcBlock *block, char *symbolname) 
{
	list_append(block->symbols, symbolname);
	return block->symbols->size-1;
}


/**
 *  Add instruction into block
 */
static void 
kxcblock_add_instruction(KxcBlock *block, KxcInstruction *instruction) 
{
	list_append(block->code,instruction);
}

/**
 *	Find id of symbols by symbolname.
 *	If symbolname don't exist, symbol is createn
 */
static int 
kxcblock_get_symbol(KxcBlock *block, char *symbolname)
{
	int t;
	for (t=0;t<block->symbols->size;t++) {
		if (!strcmp(symbolname,block->symbols->items[t])) {
			free(symbolname); // Symbol find in table, our symbolname can be freed
			return t;
		}
	}
	return kxcblock_add_symbol(block,symbolname);
}

/**
 *	Add parameters into block
 *	"parameters" is list of names of parameters
 */
static void 
kxcblock_add_params(KxcBlock *block, List *parameters, List *errors) 
{
	if (parameters == NULL) {
		block->params_count = 0;
		block->params = NULL;
		return;
	}

	block->params_count = parameters->size;
	block->params = malloc(parameters->size * sizeof(char));
	ALLOCTEST(block->params);


	int t;
	for (t=0;t<parameters->size;t++) {
		PDEBUG("param :%s\n",parameters->items[t]);
		int symbol = kxcblock_get_symbol(block, (char*)parameters->items[t]);
		block->params[t] = symbol;
		int s;
		for (s=0;s<t;s++) {
			if (block->params[s] == symbol) {
				char tmp[250];
				snprintf(tmp,250, "Two parameters has same name '%s'",parameters->items[s]);
				list_append(errors, strdup(tmp));
				break;
			}
		}
	}
	list_free(parameters);
}

/**
 *	Add name of local slots into block
 *	"parameters" is list of names of parameters
 */
static void 
kxcblock_add_localslots(KxcBlock *block, List *slots, List *errors) 
{
	if (slots == NULL) {
		block->localslots_count = 0;
		block->localslots = NULL;
		return;
	}

	block->localslots_count = slots->size;
	block->localslots = malloc(slots->size * sizeof(char));
	ALLOCTEST(block->localslots);


	int t;
	for (t=0;t<slots->size;t++) {
		PDEBUG("local slot :%s\n",slots->items[t]);
		int symbol = kxcblock_get_symbol(block, (char*)slots->items[t]);
		block->localslots[t] = symbol;
		int s;
		for (s=0;s<t;s++) {
			if (block->localslots[s] == symbol) {
				char tmp[250];
				snprintf(tmp,250, "Two local slots has same name '%s'",slots->items[s]);
				list_append(errors, strdup(tmp));
				break;
			}

		}
	}
	list_free(slots);	
}


/**
 *  Put message into block
 *	msgtype is instruction type of message
 *	messagename is symbol name of instruction
 */
void
kxcblock_put_message(KxcBlock *block, KxInstructionType msgtype, char *messagename, int lineno) 
{
	PDEBUG("sendmsg: %i %s\n",msgtype,messagename);
	int symbol = kxcblock_get_symbol(block, messagename);

	KxcInstruction *i = kxcinstruction_new(msgtype);
	i->value.symbol = symbol;
	kxcblock_add_instruction(block,i);

	list_append(block->message_linenumbers,(void*)(long)lineno);
}

/**
 *	Creates new child-codeblock into and add instruction KXCI_PUSH_BLOCK into code
 *	with id of new codeblock
 *	returns new codeblock
 */
KxcBlock *
kxcblock_put_new_block(KxcBlock *block, int type, List *parameters, List *localslots, List *errors)
{
	PDEBUG("Put new block\n");

	int itype = (type == KXCBLOCK_TYPE_METHOD)?KXCI_PUSH_METHOD:KXCI_PUSH_BLOCK;

	// Create new instruction
	KxcInstruction *i = kxcinstruction_new(itype);
	
	// Create new kxcblock
	KxcBlock *child = kxcblock_new(type, parameters, localslots, errors);

	// Put new block into parent and add id into instruction
	i->value.codeblock = kxcblock_add_subblock(block,child);
	
	// Add instruction into block
	kxcblock_add_instruction(block,i);

	return child;
}


/**
 *	Function is called when inserting code into codeblock is finished.
 *	Put exit sequence of instruction into codeblock
 */
void 
kxcblock_end_of_code(KxcBlock *block)
{
	int append_end = 1;

	KxcInstruction *i =  (KxcInstruction*)list_top(block->code);

	if (i && (i->type == KXCI_RETURN || i->type==KXCI_LONGRETURN))
		append_end = 0;

	if (block->type == KXCBLOCK_TYPE_METHOD) {
		if (append_end)
			kxcblock_put_returnself(block);

		PDEBUG("End of method\n");
	} else {
		if (append_end) {
			if (i && i->type == KXCI_POP) {
				list_pop(block->code);
				kxcinstruction_free(i);
			} else if (i == NULL) {
				// TODO: Vracet rozumny objekt
				kxcblock_put_integer(block,0);
			}
			kxcblock_put_endofblock(block);
		}
		PDEBUG("End of block\n");
	}

}

/**
 *	Function is called when inserting code into main codeblock is finished.
 *	Put exit sequence of instruction into codeblock
 */
void 
kxcblock_end_of_main_block(KxcBlock *block)
{
	PDEBUG("End of main block\n");
	int append_end = 1;

	KxcInstruction *i =  (KxcInstruction*)list_top(block->code);

	if (!i || i->type != KXCI_RETURN) {
		if (i && i->type == KXCI_POP) {
				list_pop(block->code);
				kxcinstruction_free(i);
				PDEBUG("Removing pop\n");
		} else if (i == NULL) {
			// TODO: Vracet rozumny objekt
			kxcblock_put_integer(block,0);
		}
		kxcblock_put_endofblock(block);
	}
}


/**
 *	Put instuction KXCI_PUSH_INTEGER into codeblock
 */
void 
kxcblock_put_integer(KxcBlock *block, int integer)
{
	PDEBUG("Push integer %i\n", integer);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_INTEGER);
	i->value.integer = integer;
	
	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_PUSH_CHARACTER into codeblock
 */
void 
kxcblock_put_character(KxcBlock *block, char character)
{
	PDEBUG("Push character %c\n", character);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_CHARACTER);
	i->value.charval = character;
	
	kxcblock_add_instruction(block,i);
}


/**
 *	Put instuction KXCI_PUSH_FLOAT into codeblock
 */
void 
kxcblock_put_float(KxcBlock *block, double floatval)
{
	PDEBUG("Push integer %lg\n", floatval);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_FLOAT);
	i->value.floatval = floatval;
	
	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_PUSH_STRING into codeblock
 */
void 
kxcblock_put_string(KxcBlock *block, char *string)
{
	PDEBUG("Push string %s\n", string);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_STRING);
	i->value.string = string;

	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_PUSH_SYMBOL into codeblock
 */
void 
kxcblock_put_symbol(KxcBlock *block, char *symbolname)
{
	PDEBUG("Push symbol %s\n", symbolname);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_SYMBOL);
	i->value.symbol = kxcblock_get_symbol(block,symbolname);

	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_POP into codeblock
 */
void 
kxcblock_put_pop(KxcBlock *block) 
{
	KxcInstruction *i =  (KxcInstruction*)list_top(block->code);

	if (i) {
		switch(i->type) {
			case KXCI_RETURN:
			case KXCI_RETURNSELF:
			case KXCI_END_OF_BLOCK:
			case KXCI_LONGRETURN:
				return;
		}
	}

	PDEBUG("Pop\n");
	i = kxcinstruction_new(KXCI_POP);
	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_RETURN into codeblock
 */
void
kxcblock_put_return(KxcBlock *block)
{
	if (block->type == KXCBLOCK_TYPE_METHOD) {
		PDEBUG("Return\n");
		KxcInstruction *i = kxcinstruction_new(KXCI_RETURN);
		kxcblock_add_instruction(block,i);
	} else {
		KxcInstruction *i = kxcinstruction_new(KXCI_LONGRETURN);
		kxcblock_add_instruction(block,i);
	}
}

/**
 *	Put instuction KXCI_RETURNSELF into codeblock
 */
void 
kxcblock_put_returnself(KxcBlock *block)
{
	PDEBUG("Return self\n");
	KxcInstruction *i = kxcinstruction_new(KXCI_RETURNSELF);
	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_END_OF_BLOCK into codeblock
 */
void 
kxcblock_put_endofblock(KxcBlock *block)
{
	PDEBUG("End-Of-Block\n");
	KxcInstruction *i = kxcinstruction_new(KXCI_END_OF_BLOCK);
	kxcblock_add_instruction(block,i);
}

/**
 *	Put instuction KXCI_LIST into codeblock
 */
void 
kxcblock_put_list(KxcBlock *block, int list_size)
{
	PDEBUG("List %i\n",list_size);
	KxcInstruction *i = kxcinstruction_new(KXCI_LIST);
	i->value.list_size = list_size;
	kxcblock_add_instruction(block,i);

}

/**
 * Get bytecode's size of symbol table
 */
static int 
kxcblock_bytecode_symboltable_size(KxcBlock *block)
{
	/*if (block->type == KXCBLOCK_TYPE_BLOCK)
		return 1; // block hasn't own symbol table (writes one byte with value -1)*/

	int size=1; // count of symbols
	int t;
	for (t=0;t<block->symbols->size;t++) {
		size += 1+strlen((char*)block->symbols->items[t]);
	}
	return size;
}

/**
 * Get bytecode's size of code
 */
static int 
kxcblock_bytecode_code_size(KxcBlock *block)
{
	int size=sizeof(int); // count of instruction
	int t;
	for (t=0;t<block->code->size;t++) {
		size += kxcinstruction_bytecode_size(block->code->items[t]);
	}
	return size;
}

static int
kxcblock_bytecode_lineno_size(KxcBlock *block)
{
	int size=sizeof(int);
	size+=sizeof(int)*block->message_linenumbers->size;
	return size;
}

/**
 * Get bytecode's size of whole block
 */
int 
kxcblock_bytecode_size(KxcBlock *block) 
{
	int size = sizeof(char); // count of instruction

	size += kxcblock_bytecode_symboltable_size(block);
	size += block->params_count + 1; // 1 byte for count of params
	size += block->localslots_count + 1; // 1 byte for count of local slots
	size += kxcblock_bytecode_code_size(block);
	size += kxcblock_bytecode_lineno_size(block);

	size++; // number of subblocks
	int t;
	// Size of subblocks
	for (t=0;t<block->subblocks->size;t++) {
		size += kxcblock_bytecode_size(block->subblocks->items[t]);
	}
	return size;
}

/**
 * Write bytecode of symbletable, 
 * If codeblock is block, only byte with value -1 is written
 * bytecode pointer is moved after symboltable
 */ 
static void 
kxcblock_bytecode_symboltable_write(KxcBlock *block, char **bytecode) 
{

	/*if (block->type == KXCBLOCK_TYPE_BLOCK) {
		**bytecode = -1;
		(*bytecode)++;
		return; // block hasn't own symbol table, downt write anything
	}*/
	BYTECODE_WRITE_CHAR(block->symbols->size);
	int t;
	for(t=0;t<block->symbols->size;t++) {
		char *z = *bytecode;
		BYTECODE_WRITE_STRING(block->symbols->items[t]);

	}
}

/**
 * Write bytecode of codeblock's parameters, 
 * bytecode pointer is moved after parameters
 */ 
static void
kxcblock_bytecode_params_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->params_count);
	memcpy(*bytecode, block->params, sizeof(char) * block->params_count);
	*bytecode += block->params_count;
	
}

static void
kxcblock_bytecode_localslots_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->localslots_count);
	memcpy(*bytecode, block->localslots, sizeof(char) * block->localslots_count);
	*bytecode += block->localslots_count;
	
}


/**
 * Write bytecode of codeblock's code, 
 * bytecode pointer is moved after written data
 */ 
static void
kxcblock_bytecode_code_write(KxcBlock *block, char **bytecode) 
{
	char *sizeaddr = *bytecode;
	*bytecode+=sizeof(int); // reserve space, size will be writen later
	char *startaddr = *bytecode;
	int t;

	for (t=0;t<block->code->size;t++) {
		char *s = *bytecode;
		kxcinstruction_bytecode_write(block->code->items[t],bytecode,block);
	}

	int size = *bytecode - startaddr;
	memcpy(sizeaddr,&size,sizeof(int));

}

static void
kxcblock_bytecode_lineno_write(KxcBlock *block, char **bytecode)
{
	BYTECODE_WRITE_INT(block->message_linenumbers->size);
	int t;
	for (t=0;t<block->message_linenumbers->size;t++) {
		int lineno = (long)block->message_linenumbers->items[t];
		BYTECODE_WRITE_INT(lineno);
	}
}

/**
 * Write bytecode of codeblock and all subblocks recursively, 
 */ 
void
kxcblock_bytecode_write(KxcBlock *block, char **bytecode)
{
	BYTECODE_WRITE_CHAR(block->type);
	kxcblock_bytecode_symboltable_write(block,bytecode);	
	kxcblock_bytecode_params_write(block, bytecode);
	kxcblock_bytecode_localslots_write(block, bytecode);
	kxcblock_bytecode_lineno_write(block, bytecode);
	kxcblock_bytecode_code_write(block,bytecode);
	
	BYTECODE_WRITE_CHAR(block->subblocks->size);
	int t;
	for (t=0;t<block->subblocks->size;t++) {
		kxcblock_bytecode_write(block->subblocks->items[t],bytecode);
	}
}
