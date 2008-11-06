/*
   block.h
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
	

#ifndef __BLOCK_H
#define __BLOCK_H

#define KXCBLOCK_TYPE_METHOD 0
#define KXCBLOCK_TYPE_BLOCK  1

#include "../utils/list.h"
#include "kxinstr.h"

typedef struct KxcInstruction KxcInstruction;
typedef struct KxcLocal KxcLocal;
typedef struct KxcMessage KxcMessage;
typedef struct KxcCondition KxcCondition;
typedef struct KxcForeach KxcForeach;
typedef struct KxcCycle KxcCycle;
typedef struct KxcCycleEnd KxcCycleEnd;
typedef struct KxcLiteral KxcLiteral;
typedef struct KxcBlock KxcBlock;
typedef struct KxcForeignBlock KxcForeignBlock;

struct KxcLocal { 
	int index;
	int scope; 
};

struct KxcMessage {
	int symbol;
	int params_count;
};

struct KxcCondition {
	int codeblock;
	int jump;
	int jump2;
};

struct KxcForeach  {
	int jump;
	int local;
	int codeblock;
};

struct KxcCycle {
	int jump;
	int codeblock;
	int local;
};

struct KxcCycleEnd {
	int jump;
	int local;
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
		int jump;
		KxcLocal local;
		KxcMessage msg;
		KxcCondition condition;
		KxcForeach foreach;
		KxcCycle cycle;
		KxcCycleEnd cycle_end;
	} value;
};

/* Foreign block is KxcBlock that is not defined directly in block but in block's child */
struct KxcForeignBlock {
	List *child_path;
	int position;
};


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

	struct List *foreign_blocks; 

	struct List *created_slots;
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

char* kxcblock_get_symbol_at(KxcBlock *block, int position);
KxcBlock* kxcblock_get_subblock_at(KxcBlock *block, int position);
KxcLiteral* kxcblock_get_literal_at(KxcBlock *block, int position);

KxcInstruction* kxcinstruction_new(KxInstructionType type);
KxcInstruction* kxcinstruction_copy(KxcInstruction *instruction);
void kxcinstruction_free(KxcInstruction *instruction);
void kxcblock_insert_instruction(KxcBlock *block, KxcInstruction *instruction, int position);

int kxcblock_add_literal(KxcBlock *block, KxcLiteral *literal);
KxcLiteral* kxcliteral_copy(KxcLiteral *literal);
int kxcblock_get_symbol(KxcBlock *block, char *symbolname);
void kxcblock_append_locals(KxcBlock *block, KxcBlock *source);

void kxcblock_insert_linenumbers(KxcBlock *block, KxcBlock *source, int position);

KxcForeignBlock * kxcforeignblock_create(int position);
void kxcforeignblock_free(KxcForeignBlock *fblock);
void kxcforeignblock_add_into_path(KxcForeignBlock *fblock, int subblock);
KxcForeignBlock *kxcforeignblock_copy(KxcForeignBlock *fblock);
KxcBlock *kxcblock_get_foreign_block(KxcBlock *block, KxcForeignBlock *fblock);
void kxcblock_remove_linenumber_for_instruction(KxcBlock *block, int iposition);
int kxcblock_get_line_index_from_position(KxcBlock *block, int position);
int kxcblock_size_of_instructions(KxcBlock *block);


#endif
