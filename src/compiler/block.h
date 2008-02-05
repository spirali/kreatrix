
#ifndef __BLOCK_H
#define __BLOCK_H

#define KXCBLOCK_TYPE_METHOD 0
#define KXCBLOCK_TYPE_BLOCK  1

#include "../utils/list.h"
#include "kxinstr.h"

typedef struct KxcInstruction KxcInstruction;

struct KxcInstruction {
	KxInstructionType type;
	union {
		int symbol;
		int codeblock;
		int list_size;
		char *string;
		int charval;
		int integer;
        double floatval;
		char local;
	} value;
};

typedef struct KxcBlock KxcBlock;

struct KxcBlock {
	
	int type; // values: KXCBLOCK_TYPE_*
	KxcBlock *parent_block;

	/*char *params;*/
	
	int locals_start_pos;

	char *locals;

	int params_count;
	int locals_count;

	struct List *symbols;

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

int kxcblock_local_pos(KxcBlock *block, char *messagename);


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

void kxcblock_push_local(KxcBlock *block, int pos);


#endif
