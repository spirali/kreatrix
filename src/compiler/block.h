
#ifndef __BLOCK_H
#define __BLOCK_H

#define KXCBLOCK_TYPE_METHOD 0
#define KXCBLOCK_TYPE_BLOCK  1

#include "../utils/list.h"
#include "kxinstr.h"

typedef struct KxcInstruction KxcInstruction;
typedef struct KxcLocal KxcLocal;
typedef struct KxcMessage KxcMessage;
typedef struct KxcExtra KxcExtra;
typedef struct KxcLiteral KxcLiteral;


struct KxcLocal { 
	int index;
	int scope; 
};

struct KxcMessage {
	int symbol;
	int params_count;
};

struct KxcExtra {
	int codeblock;
	int jump;
};

struct KxcLiteral {
	KxcLiteralType type;
	union {
		int integer;
		int symbol;
		int charval;
		double floatval;
		char *string;
	} value;
};

struct KxcInstruction {
	KxInstructionType type;
	union {
		int symbol;
		int codeblock;
		int list_size;
		int literal;
		KxcLocal local;
		KxcMessage msg;
		KxcExtra extra;
	} value;
};

typedef struct KxcBlock KxcBlock;

struct KxcBlock {
	
	int type; // values: KXCBLOCK_TYPE_*
	KxcBlock *parent_block;

	char *locals;

	int params_count;
	int locals_count;

	struct List *symbols;

	struct List *literals;

	struct List *code; // List of instructions

	struct List *subblocks;

	struct List *message_linenumbers;

};

struct KxcBlock * kxcblock_new(int type,KxcBlock *parent, List *parameters, List *localslots, List *errors);
void kxcblock_free(KxcBlock *block);

void kxcblock_put_message(KxcBlock *block, KxInstructionType msgtype, char *messagename, int lineno);

KxcBlock* kxcblock_put_new_block(KxcBlock *block,int type, List *parameters, List *localslots, List *errors);

void kxcblock_end_of_code(KxcBlock *block);

void kxcblock_end_of_main_block(KxcBlock *block);

int kxcblock_find_local(KxcBlock *block, char *messagename, KxcLocal *local);


void kxcblock_put_pop(KxcBlock *block);

void kxcblock_put_string(KxcBlock *block, char *string);
void kxcblock_put_symbol(KxcBlock *block, char *symbolname);
void kxcblock_put_integer(KxcBlock *block, int integer);
void kxcblock_put_return(KxcBlock *block);
void kxcblock_put_returnself(KxcBlock *block);
void kxcblock_put_endofblock(KxcBlock *block);
void kxcblock_put_list(KxcBlock *block, int list_size);
void kxcblock_put_float(KxcBlock *block, double floatval);
void kxcblock_put_character(KxcBlock *block, char character);


int kxcblock_bytecode_size(KxcBlock *block);
void kxcblock_bytecode_write(KxcBlock *block, char **bytecode);

void kxcblock_push_local(KxcBlock *block, KxcLocal *local);

void kxcblock_remove_instruction(KxcBlock *block, int position);

char *kxcblock_get_symbol_at(KxcBlock *block, int position);
KxcBlock *kxcblock_get_subblock_at(KxcBlock *block, int position);
KxcLiteral *kxcblock_get_literal_at(KxcBlock *block, int position);

KxcInstruction *kxcinstruction_copy(KxcInstruction *instruction);
void kxcinstruction_free(KxcInstruction *instruction);

int kxcblock_add_literal(KxcBlock *block, KxcLiteral *literal);
KxcLiteral *kxcliteral_copy(KxcLiteral *literal);
int kxcblock_get_symbol(KxcBlock *block, char *symbolname);
void kxcblock_append_locals(KxcBlock *block, KxcBlock *source);

void kxcblock_insert_linenumbers(KxcBlock *block, KxcBlock *source, int position);


#endif
