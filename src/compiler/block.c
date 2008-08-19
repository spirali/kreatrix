/*
   block.c
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
	instruction->value.symbol = 0;
	return instruction;
}

/**
 * Free instruction 
 */
static void
kxcinstruction_free(KxcInstruction *instruction)
{   
	free(instruction);
}

/**
 * Get size of instruction in bytecode
 */
static int 
kxcinstruction_bytecode_size(KxcInstruction *instruction) 
{
	return kxinstructions_info[instruction->type].params_count + 1;
}

static KxcLiteral*
kxcliteral_new(KxcLiteralType type)
{
	KxcLiteral *literal = malloc(sizeof(KxcLiteral));
	ALLOCTEST(literal);
	literal->type = type;
	literal->value.string = NULL;
	return literal;
}

static void
kxcliteral_free(KxcLiteral *literal)
{   
	if (literal->type == KXC_LITERAL_STRING || literal->type == KXC_LITERAL_SYMBOL) {
		free(literal->value.string);
	}
	free(literal);
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
			BYTECODE_WRITE_CHAR(instruction->value.list_size);
			return;

		case KXCI_POP:
		case KXCI_RETURN_STACK_TOP:
		case KXCI_LONGRETURN:
		case KXCI_RETURN_SELF:
			return; // No aditional data
	
		case KXCI_PUSH_METHOD:			
		case KXCI_PUSH_BLOCK:
			BYTECODE_WRITE_CHAR(instruction->value.codeblock);
			return;

		case KXCI_PUSH_SELF:
		case KXCI_PUSH_ACTIVATION:
			return;

		case KXCI_PUSH_LITERAL:
			BYTECODE_WRITE_CHAR(instruction->value.literal);
			return;

		case KXCI_PUSH_LOCAL:
		case KXCI_UPDATE_LOCAL:
			BYTECODE_WRITE_CHAR(instruction->value.local.index);
			return;

		case KXCI_PUSH_OUTER_LOCAL:
		case KXCI_UPDATE_OUTER_LOCAL:
			BYTECODE_WRITE_CHAR(instruction->value.local.scope - 1);
			BYTECODE_WRITE_CHAR(instruction->value.local.index);
			return;
	}

	fprintf(stderr,"Internal error, invalid instruction type\n");
	abort();
}

/// -- KxcBlock --------------------------------------------------

static int kxcblock_get_symbol(KxcBlock *block, char *symbolname);
static void kxcblock_add_params_and_locals(KxcBlock *block, List *parameters, List *locals, List *errors);


/**
 * Create new block
 */
struct KxcBlock*
kxcblock_new(int type, KxcBlock * parent, List *parameters, List *localslots, List *errors)
{
	struct KxcBlock *block = malloc(sizeof(KxcBlock));
	ALLOCTEST(block);
	
	if (type == KXCBLOCK_TYPE_METHOD)
		block->parent_block = NULL;
	else 
		block->parent_block = parent;

	block->symbols = list_new();
	block->literals = list_new();
	block->created_slots = NULL;

	block->type = type;

	kxcblock_add_params_and_locals(block,parameters, localslots, errors);

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

	if (block->locals)
		free(block->locals);
	
	if (block->created_slots)
		list_free(block->created_slots);

	// Free instructions
	list_foreach(block->code, (ListForeachFcn*) &kxcinstruction_free);
	list_free(block->code);


	list_foreach(block->literals, (ListForeachFcn*) &kxcliteral_free);
	list_free(block->literals);
	
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
 *	Add parameters and local slots into block
 *	"parameters" is list of names of parameters
 *	"locals" is list of names of localslots
 */
static void 
kxcblock_add_params_and_locals(KxcBlock *block, List *parameters, List *locals, List *errors) 
{
	if (parameters == NULL) {
		block->params_count = 0;
	} else {
		block->params_count = parameters->size;
	}

	block->locals_count = block->params_count;
	int locals_count;

	if (locals == NULL) {
		block->locals_count += 0;
		locals_count = 0;
	} else {
		block->locals_count += locals->size;
		locals_count = locals->size;
	}
	
	block->locals = malloc(block->locals_count * sizeof(char));
	ALLOCTEST(block->locals);


	int t;
	for (t=0;t<block->params_count;t++) {
		PDEBUG("param :%s\n",parameters->items[t]);
		int symbol = kxcblock_get_symbol(block, (char*)parameters->items[t]);
		block->locals[t] = symbol;
		int s;
		for (s=0;s<t;s++) {
			if (block->locals[s] == symbol) {
				char tmp[250];
				snprintf(tmp,250, "Two parameters has same name '%s'",parameters->items[s]);
				list_append(errors, strdup(tmp));
				break;
			}
		}
	}

	for (t=0;t<locals_count;t++) {
		PDEBUG("local slot :%s\n",locals->items[t]);
		int symbol = kxcblock_get_symbol(block, (char*)locals->items[t]);
		int pos = t + block->params_count;
		block->locals[pos] = symbol;
		int s;
		for (s=0;s<pos;s++) {
			if (block->locals[s] == symbol) {
				char tmp[250];
				if (s < block->params_count) {
					snprintf(tmp,250, "Local slot and parameter has same name '%s'",parameters->items[s]);
				} else {
					snprintf(tmp,250, "Two local slots has same name '%s'",
						locals->items[s - block->params_count]);
				}
				list_append(errors, strdup(tmp));
				break;
			}

		}
	}

	if (parameters)
		list_free(parameters);

	if (locals)
		list_free(locals);
}

void
kxcblock_push_local(KxcBlock *block, KxcLocal *local)
{
	KxInstructionType itype;

	if (local->scope == 0) {
		itype = KXCI_PUSH_LOCAL;
	} else {
		itype = KXCI_PUSH_OUTER_LOCAL;
	}

	KxcInstruction *i = kxcinstruction_new(itype);
	i->value.local = *local;
	kxcblock_add_instruction(block,i);
}

static int 
kxcblock_find_local_recursive(KxcBlock *block, char *messagename, KxcLocal *local)
{
	int t;
	for (t=0; t<block->locals_count; t++) {
		if (!strcmp(messagename, block->symbols->items[block->locals[t]])) {
			local->index = t;
			return 1;
		}
	}
	if (block->parent_block) {
		local->scope++;
		return kxcblock_find_local_recursive(block->parent_block, messagename, local);
	}
	return 0;

}

int
kxcblock_find_local(KxcBlock *block, char *messagename, KxcLocal *local)
{
	local->scope = 0;
	return kxcblock_find_local_recursive(block, messagename, local);
}


static int
kxcblock_message_substitution(KxcBlock *block, char *messagename)
{
	KxcInstruction *i;
	if (!strcmp(messagename,"self")) {
		free(messagename);
		i = kxcinstruction_new(KXCI_PUSH_SELF);
		kxcblock_add_instruction(block, i);
		return 1;
	}

	if (!strcmp(messagename,"activation")) {
		free(messagename);
		i = kxcinstruction_new(KXCI_PUSH_ACTIVATION);
		kxcblock_add_instruction(block, i);
		return 1;
	}

	KxcLocal local;
	int is_local = kxcblock_find_local(block, messagename, &local);

	if (is_local) {
			free(messagename);
			kxcblock_push_local(block, &local);		
			return 1;
	}
	return 0;
}


static List *
kxcblock_construct_stack_state(KxcBlock *block)
{
	List *stack = list_new();
	int t;
	for (t=0; t < block->code->size; t++) {
		KxcInstruction *i = block->code->items[t];

		switch(i->type) {

			case KXCI_BINARY_MSG:
				list_fast_pop(stack);
				list_fast_pop(stack);
				list_append(stack, i);
				break;

			case KXCI_UNARY_MSG:  
				list_fast_pop(stack);
				list_append(stack, i);
				break;
		
			case KXCI_KEYWORD_MSG:
				list_fast_pop(stack);
			case KXCI_LOCAL_KEYWORD_MSG:
			case KXCI_RESEND_KEYWORD_MSG:
				{
					int s;
					char *msgname = (char*) block->symbols->items[i->value.symbol];
					int count = params_count(msgname);

					for (s = 0; s < count; s++) {
						list_fast_pop(stack);
					}
					list_append(stack, i);
					break;
				}

			case KXCI_LIST:
				{
					int s;
					for (s = 0; s < i->value.list_size; s++) {
						list_fast_pop(stack);
					}
					list_append(stack, i);
					break;
				}


			case KXCI_UPDATE_LOCAL:
			case KXCI_UPDATE_OUTER_LOCAL:
			case KXCI_POP:
				list_fast_pop(stack);
				break;

			case KXCI_LOCAL_UNARY_MSG:
			case KXCI_RESEND_UNARY_MSG:
			case KXCI_PUSH_LITERAL:
			case KXCI_PUSH_METHOD:
			case KXCI_PUSH_BLOCK: 
			case KXCI_PUSH_LOCAL:
			case KXCI_PUSH_OUTER_LOCAL:
			case KXCI_PUSH_SELF:
			case KXCI_PUSH_ACTIVATION:
				list_append(stack, i);
				break;
				 
			case KXCI_RETURN_STACK_TOP:
			case KXCI_RETURN_SELF:
			case KXCI_LONGRETURN:
				 break;

			case KXCI_HOTSPOT_PROBE:
			case KXCI_UNARY_MSG_C:
			case KXCI_BINARY_MSG_C:
			case KXCI_KEYWORD_MSG_C:
			case KXCI_LOCAL_UNARY_MSG_C:
			case KXCI_LOCAL_KEYWORD_MSG_C:
			case KXCI_INSTRUCTIONS_COUNT:
			default:
				printf("Internal error: kxcblock_construct_stack_state");
				abort();
				break;
		}
	}
	return stack;
}

void kxcblock_add_to_created_slots(KxcBlock *block)
{
	List *stack = kxcblock_construct_stack_state(block);

	int size = stack->size;
	if (size < 2) {
		list_free(stack);
		return;
	}
	
	KxcInstruction *i = stack->items[size - 2];
	if (i->type == KXCI_PUSH_LITERAL) {
		int lindex = i->value.literal;
		KxcLiteral *literal = block->literals->items[lindex];
		if (literal->type == KXC_LITERAL_SYMBOL) {
			if (block->created_slots == NULL) {
				block->created_slots = list_new();
			}
			if (!list_contains(block->created_slots, (void*) lindex)) {
				list_append(block->created_slots, (void*) lindex);
			}
		}
	}

	list_free(stack);
}


/**
 *  Put message into block
 *	msgtype is instruction type of message
 *	messagename is symbol name of instruction
 */
void
kxcblock_put_message(KxcBlock *block, KxInstructionType msgtype, char *messagename, int lineno) 
{
	if (msgtype == KXCI_LOCAL_UNARY_MSG) {
		if (kxcblock_message_substitution(block, messagename))
			return;
	}

	if (msgtype == KXCI_LOCAL_KEYWORD_MSG && !strcmp("slot:set:", messagename)) {
		kxcblock_add_to_created_slots(block);
	}

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
	KxcBlock *child = kxcblock_new(type, block, parameters, localslots, errors);

	// Put new block into parent and add id into instruction
	i->value.codeblock = kxcblock_add_subblock(block,child);
	
	// Add instruction into block
	kxcblock_add_instruction(block,i);

	return child;
}


void
kxcblock_put_local_update(KxcBlock *block, KxcLocal *local)
{
	KxInstructionType itype;

	if (local->scope == 0) {
		itype = KXCI_UPDATE_LOCAL;
	} else {
		itype = KXCI_UPDATE_OUTER_LOCAL;
	}

	KxcInstruction *i = kxcinstruction_new(itype);
	i->value.local = *local;
	kxcblock_add_instruction(block, i);
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

	if (i && (i->type == KXCI_RETURN_STACK_TOP || i->type==KXCI_LONGRETURN))
		append_end = 0;

	if (block->type == KXCBLOCK_TYPE_METHOD) {
		if (append_end)
			kxcblock_put_returnself(block);

		PDEBUG("End of method\n");
	} else {
		if (append_end) {
			if (i && i->type == KXCI_POP) {
				list_fast_pop(block->code);
				kxcinstruction_free(i);
			} else if (i == NULL) {
				// TODO: return something else
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

	if (!i || i->type != KXCI_RETURN_STACK_TOP) {
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

static int
kxcblock_literals_equals(KxcLiteral *literal1, KxcLiteral *literal2)
{
	if (literal1->type != literal2->type)
		return 0;
	switch(literal1->type) {
		case KXC_LITERAL_STRING:
		case KXC_LITERAL_SYMBOL:
			return !strcmp(literal1->value.string, literal2->value.string);
		case KXC_LITERAL_INTEGER:
			return literal1->value.integer == literal2->value.integer;
		case KXC_LITERAL_CHAR:
			return literal1->value.charval == literal2->value.charval;
		case KXC_LITERAL_FLOAT:
			return literal1->value.floatval == literal2->value.floatval;
	}
}

static int
kxcblock_add_literal(KxcBlock *block, KxcLiteral *literal)
{
	int t;
	for (t=0;t<block->literals->size;t++) {
		if (kxcblock_literals_equals(literal, block->literals->items[t])) {
			kxcliteral_free(literal);
			return t;
		}
	}
	list_append (block->literals, literal);
	return block->literals->size - 1;
}

static void
kxcblock_put_literal(KxcBlock *block, KxcLiteral *literal)
{
	int literal_id = kxcblock_add_literal(block,literal);

	KxcInstruction *i = kxcinstruction_new(KXCI_PUSH_LITERAL);
	i->value.literal = literal_id;
	
	kxcblock_add_instruction(block,i);
}


/**
 *	Put instuction KXCI_PUSH_INTEGER into codeblock
 */
void 
kxcblock_put_integer(KxcBlock *block, int integer)
{
	PDEBUG("Push integer %i\n", integer);

	KxcLiteral *l = kxcliteral_new(KXC_LITERAL_INTEGER);
	l->value.integer = integer;

	kxcblock_put_literal(block, l);
}

/**
 *	Put instuction KXCI_PUSH_CHARACTER into codeblock
 */
void 
kxcblock_put_character(KxcBlock *block, char character)
{
	PDEBUG("Push character %c\n", character);

	KxcLiteral *l = kxcliteral_new(KXC_LITERAL_CHAR);
	l->value.charval = character;

	kxcblock_put_literal(block, l);
}


/**
 *	Put instuction KXCI_PUSH_FLOAT into codeblock
 */
void 
kxcblock_put_float(KxcBlock *block, double floatval)
{
	PDEBUG("Push integer %lg\n", floatval);

	KxcLiteral *l = kxcliteral_new(KXC_LITERAL_FLOAT);
	l->value.floatval = floatval;

	kxcblock_put_literal(block, l);
}

/**
 *	Put instuction KXCI_PUSH_STRING into codeblock
 */
void 
kxcblock_put_string(KxcBlock *block, char *string)
{
	PDEBUG("Push string %s\n", string);

	KxcLiteral *l = kxcliteral_new(KXC_LITERAL_STRING);
	l->value.string = string;

	kxcblock_put_literal(block, l);
}

/**
 *	Put instuction KXCI_PUSH_SYMBOL into codeblock
 */
void 
kxcblock_put_symbol(KxcBlock *block, char *symbolname)
{
	PDEBUG("Push symbol %s\n", symbolname);

	KxcLiteral *l = kxcliteral_new(KXC_LITERAL_SYMBOL);
	l->value.string = symbolname;

	kxcblock_put_literal(block, l);
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
			case KXCI_RETURN_STACK_TOP:
			case KXCI_RETURN_SELF:
			case KXCI_LONGRETURN:
				return;
		}
	}

	PDEBUG("Pop\n");
	i = kxcinstruction_new(KXCI_POP);
	kxcblock_add_instruction(block,i);
}

void
kxcblock_put_return(KxcBlock *block)
{
	if (block->type == KXCBLOCK_TYPE_METHOD) {
		PDEBUG("Return\n");
		KxcInstruction *i = kxcinstruction_new(KXCI_RETURN_STACK_TOP);
		kxcblock_add_instruction(block,i);
	} else {
		KxcInstruction *i = kxcinstruction_new(KXCI_LONGRETURN);
		kxcblock_add_instruction(block,i);
	}
}

/**
 *	Put instuction KXCI_RETURN_SELF into codeblock
 */
void 
kxcblock_put_returnself(KxcBlock *block)
{
	PDEBUG("Return self\n");
	KxcInstruction *i = kxcinstruction_new(KXCI_RETURN_SELF);
	kxcblock_add_instruction(block,i);
}

void 
kxcblock_put_endofblock(KxcBlock *block)
{
	PDEBUG("End-Of-Block\n");
	KxcInstruction *i = kxcinstruction_new(KXCI_RETURN_STACK_TOP);
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
	int size=1; // count of symbols
	int t;
	for (t=0;t<block->symbols->size;t++) {
		size += 1+strlen((char*)block->symbols->items[t]);
	}
	return size;
}

static int 
kxcblock_bytecode_literals_size(KxcBlock *block) 
{
	int size = 1;
	int t;
	for(t=0;t<block->literals->size;t++) {
		KxcLiteral *literal = block->literals->items[t];
		size += 1;
		switch(literal->type) {
			case KXC_LITERAL_STRING:
			case KXC_LITERAL_SYMBOL:
				size += strlen(literal->value.string) + 1;
				break;

			case KXC_LITERAL_INTEGER:
			case KXC_LITERAL_CHAR:
				size += sizeof(int);
				break;

			case KXC_LITERAL_FLOAT:
				size += sizeof(double);
				break;
		}
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

static int
kxcblock_bytecode_created_slots_size(KxcBlock *block)
{
	int size=1;
	if (block->created_slots) {
		size+=block->created_slots->size;
	}
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
	size += kxcblock_bytecode_literals_size(block);
	size += block->locals_count + 1; // 1 byte for count of local slots
	size += 1; // 1 byte for count of params

	size += kxcblock_bytecode_code_size(block);
	size += kxcblock_bytecode_lineno_size(block);
	size += kxcblock_bytecode_created_slots_size(block);

	size++; // number of subblocks
	int t;
	// Size of subblocks
	for (t=0;t<block->subblocks->size;t++) {
		size += kxcblock_bytecode_size(block->subblocks->items[t]);
	}
	return size;
}

static void 
kxcblock_bytecode_literals_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->literals->size);
	int t;
	for(t=0;t<block->literals->size;t++) {
		KxcLiteral *literal = block->literals->items[t];
		BYTECODE_WRITE_CHAR(literal->type);
		switch(literal->type) {
			case KXC_LITERAL_STRING:
			case KXC_LITERAL_SYMBOL:
				BYTECODE_WRITE_STRING(literal->value.string);
				break;
			case KXC_LITERAL_INTEGER:
				BYTECODE_WRITE_INT(literal->value.integer);
				break;
			case KXC_LITERAL_CHAR:
				BYTECODE_WRITE_INT(literal->value.charval);
				break;
			case KXC_LITERAL_FLOAT:
				BYTECODE_WRITE_DOUBLE(literal->value.floatval);
				break;

		}
	}
}

/**
 * Write bytecode of symbletable, 
 * If codeblock is block, only byte with value -1 is written
 * bytecode pointer is moved after symboltable
 */ 
static void 
kxcblock_bytecode_symboltable_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->symbols->size);
	int t;
	for(t=0;t<block->symbols->size;t++) {
		BYTECODE_WRITE_STRING(block->symbols->items[t]);

	}
}

static void
kxcblock_bytecode_params_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->params_count);
}

static void
kxcblock_bytecode_locals_write(KxcBlock *block, char **bytecode) 
{
	BYTECODE_WRITE_CHAR(block->locals_count);
	memcpy(*bytecode, block->locals, sizeof(char) * block->locals_count);
	*bytecode += block->locals_count;
	
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

static void
kxcblock_bytecode_created_slots_write(KxcBlock *block, char **bytecode)
{
	int size;
	if (block->created_slots) {
		size = block->created_slots->size;
	} else {
		size = 0;
	}
	BYTECODE_WRITE_CHAR(size);

	int t;
	for (t=0;t<size;t++) {
		char lindex = (long) block->created_slots->items[t];
		BYTECODE_WRITE_CHAR(lindex);
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
	kxcblock_bytecode_literals_write(block,bytecode);	
	kxcblock_bytecode_params_write(block, bytecode);
	kxcblock_bytecode_locals_write(block, bytecode);
	kxcblock_bytecode_lineno_write(block, bytecode);
	kxcblock_bytecode_created_slots_write(block, bytecode);
	kxcblock_bytecode_code_write(block,bytecode);
	
	BYTECODE_WRITE_CHAR(block->subblocks->size);
	int t;
	for (t=0;t<block->subblocks->size;t++) {
		kxcblock_bytecode_write(block->subblocks->items[t],bytecode);
	}
}

void 
kxcblock_remove_instruction(KxcBlock *block, int position)
{
	KxcInstruction *i = block->code->items[position];
	list_fast_remove(block->code, position);
	kxcinstruction_free(i);
}
