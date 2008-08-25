
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block_optimise.h"

typedef struct KxInstructionReplacingRule KxInstructionReplacingRule;
typedef void(ReplaceFcn)(
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule);

struct KxInstructionReplacingRule {
	int originalInstruction;
	char *messageName;
	int replacingInstruction;
	int countOfClosures;
	int countOfParameters[2];
	ReplaceFcn *replace_function;
};

static void kxcblock_replace_instructions_condition
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);
static void kxcblock_replace_instructions_condition_else
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);
static void kxcblock_replace_instructions_foreach
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);
static void kxcblock_replace_instructions_foreach_local
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);
static void kxcblock_replace_instructions_cycle
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);
static void kxcblock_replace_instructions_while_cycle
	(KxcBlock *, KxcInstruction *, int, KxcBlock **, KxcInstruction **, KxInstructionReplacingRule *);


static KxInstructionReplacingRule replacing_rules[] = {
	{ KXCI_KEYWORD_MSG, "ifTrue:",  KXCI_IFTRUE,  1, {0}, kxcblock_replace_instructions_condition },
	{ KXCI_KEYWORD_MSG, "ifFalse:", KXCI_IFFALSE, 1, {0}, kxcblock_replace_instructions_condition  },
	{ KXCI_KEYWORD_MSG, "ifTrue:ifFalse:", KXCI_IFTRUE_IFFALSE, 2, {0,0}, kxcblock_replace_instructions_condition_else  },
	{ KXCI_KEYWORD_MSG, "ifFalse:ifTrue:", KXCI_IFFALSE_IFTRUE, 2, {0,0}, kxcblock_replace_instructions_condition_else  },
	{ KXCI_KEYWORD_MSG, "foreach:", KXCI_FOREACH, 1, {1}, kxcblock_replace_instructions_foreach  },
	{ KXCI_LOCAL_KEYWORD_MSG, "foreach:", KXCI_FOREACH, 1, {1}, kxcblock_replace_instructions_foreach_local  },
	{ KXCI_KEYWORD_MSG, "to:do:", KXCI_TODO, 1, {1}, kxcblock_replace_instructions_cycle  },
    { KXCI_KEYWORD_MSG, "repeat:", KXCI_REPEAT, 1, {0}, kxcblock_replace_instructions_cycle  },
	{ KXCI_KEYWORD_MSG, "to:by:do:", KXCI_TOBYDO, 1, {1}, kxcblock_replace_instructions_cycle  },
	{ KXCI_UNARY_MSG,   "whileTrue",  KXCI_JUMP_IFTRUE,  1, {0}, kxcblock_replace_instructions_while_cycle },
	{ KXCI_UNARY_MSG,   "whileFalse",  KXCI_JUMP_IFFALSE,  1, {0}, kxcblock_replace_instructions_while_cycle },
	{ 0, NULL, 0 }
};

static void 
kxcblock_optimise_remove_pops(KxcBlock *block)
{
	int t;

	// start from 1, because first instruction cant be pop
	for (t=1;t<block->code->size;t++) {
		KxcInstruction *i = block->code->items[t];

		if (i->type == KXCI_POP) {
			KxcInstruction *prev = block->code->items[t-1];
			if (prev->type == KXCI_PUSH_LOCAL || prev->type == KXCI_PUSH_OUTER_LOCAL) {
				kxcblock_remove_instruction(block, t);
				kxcblock_remove_instruction(block, t - 1);
				t--;
			}
		}
	}
}

static int
kxcblock_get_line_index_from_position(KxcBlock *block, int position)
{
	int index = 0;
	int t;
	for (t=0; t < position; t++) {
		KxcInstruction *i = block->code->items[t];
		if (kxcinstruction_has_linenumber(i->type)) {
			index++;
		}
	}
	return index;
}

static void
kxcblock_insert_foreign_subblocks(KxcBlock *block, KxcBlock *source, int position)
{
	int t;
	for (t=0; t < source->subblocks->size; t ++) {
		KxcForeignBlock *fblock = kxcforeignblock_create(t);
		kxcforeignblock_add_into_path(fblock, position);
		list_append(block->foreign_blocks, fblock);
	}

	for (t=0; t < source->foreign_blocks->size; t++) {
		KxcForeignBlock *fblock = kxcforeignblock_copy(source->foreign_blocks->items[t]);
		kxcforeignblock_add_into_path(fblock, position);
		list_append(block->foreign_blocks, fblock);
	}
}

static int
kxcblock_insert_instructions(KxcBlock *block, int position, KxcBlock *source, int source_position)
{
	int t;
	int bytes = 0;

	int start_id_of_locals = block->locals_count;

	int start_of_subblocks = block->subblocks->size;

	kxcblock_append_locals(block, source);
	kxcblock_insert_linenumbers(block, source, kxcblock_get_line_index_from_position(block, position));
	
	kxcblock_insert_foreign_subblocks(block, source, source_position);

	for (t=0;t<source->code->size;t++) {
		KxcInstruction *i = kxcinstruction_copy(source->code->items[t]);
		
		int skip = 0;
		switch(i->type) {
			case KXCI_RETURN_STACK_TOP:
				skip = 1;
				break;
			case KXCI_PUSH_LITERAL: {
					KxcLiteral *literal = kxcliteral_copy(kxcblock_get_literal_at(source, i->value.literal));
					i->value.literal = kxcblock_add_literal(block, literal);
				}
				break;

			case KXCI_LOCAL_UNARY_MSG:
			case KXCI_RESEND_UNARY_MSG:
			case KXCI_BINARY_MSG:
			case KXCI_KEYWORD_MSG: 
			case KXCI_LOCAL_KEYWORD_MSG: 
			case KXCI_RESEND_KEYWORD_MSG: 
			case KXCI_UNARY_MSG: {
				char *symbolname = strdup(kxcblock_get_symbol_at(source, i->value.msg.symbol));
				i->value.msg.symbol = kxcblock_get_symbol(block, symbolname);
			} break;

			case KXCI_PUSH_LOCAL:
			case KXCI_UPDATE_LOCAL:
				i->value.local.index += start_id_of_locals;
			break;

			case KXCI_PUSH_OUTER_LOCAL: 
				i->value.local.scope--;
				if (i->value.local.scope == 0) {
					i->type = KXCI_PUSH_LOCAL;
				}
				break;

			case KXCI_UPDATE_OUTER_LOCAL: 
				i->value.local.scope--;
				if (i->value.local.scope == 0) {
					i->type = KXCI_UPDATE_LOCAL;
				}
				break;

			case KXCI_LONGRETURN:
				if (block->type == KXCBLOCK_TYPE_METHOD)
					i->type = KXCI_RETURN_STACK_TOP;
				break;

			case KXCI_PUSH_BLOCK:
					i->value.codeblock += start_of_subblocks;
				break;

			case KXCI_IFTRUE:
			case KXCI_IFFALSE_IFTRUE:
			case KXCI_IFFALSE:
			case KXCI_IFTRUE_IFFALSE:
					i->value.condition.codeblock += start_of_subblocks;
				break;

			case KXCI_FOREACH:
					i->value.foreach.codeblock += start_of_subblocks;
				break;
		}
		
		if (!skip) {
			KxInstructionInfo *ii = &kxinstructions_info[i->type];
			bytes += 1 + ii->params_count;
			list_insert_at(block->code, i, position);
			position++;
		} else {
			kxcinstruction_free(i);
		}
	}

	return bytes;
}

static void
kxcblock_replace_instructions_condition(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	int codeblock = prev_instructions[0]->value.codeblock;

	instruction->value.condition.codeblock = codeblock;
	kxcblock_remove_instruction(block, position - 1);
	int bytes = kxcblock_insert_instructions(block, position, closures[0], codeblock);
	instruction->type = rule->replacingInstruction;
	instruction->value.condition.jump = bytes;
}

static void
kxcblock_replace_instructions_while_cycle(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	int codeblock = prev_instructions[0]->value.codeblock;
	kxcblock_remove_instruction(block, position - 1);
	int bytes = kxcblock_insert_instructions(block, position - 1, closures[0], codeblock);
	instruction->type = rule->replacingInstruction;
	instruction->value.jump = - (bytes + kxinstructions_info[instruction->type].params_count + 1);
}

static void
kxcblock_replace_instructions_condition_else(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	int codeblock = prev_instructions[0]->value.codeblock;

	instruction->value.condition.codeblock = codeblock;

	/* Remove PUSH_BLOCKS */
	kxcblock_remove_instruction(block, position - 1);
	kxcblock_remove_instruction(block, position - 2);

	position--;

	int bytes1 = kxcblock_insert_instructions(block, position, closures[1], codeblock + 1);

	KxcInstruction *jump_instr = kxcinstruction_new(KXCI_JUMP);
	jump_instr->value.jump = bytes1;

	kxcblock_insert_instruction(block, jump_instr, position);

	int bytes2 = kxcblock_insert_instructions(block, position, closures[0], codeblock);
	bytes2 += kxinstructions_info[KXCI_JUMP].params_count + 1;

	instruction->type = rule->replacingInstruction;
	instruction->value.condition.jump = bytes2;
	instruction->value.condition.jump2 = bytes1 + bytes2;
}

static void
kxcblock_replace_instructions_foreach(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	int codeblock = prev_instructions[0]->value.codeblock;
	instruction->value.foreach.codeblock = codeblock;
	kxcblock_remove_instruction(block, position - 1);

	KxcInstruction *nextiter_instr = kxcinstruction_new(KXCI_NEXTITER);

	instruction->value.foreach.local = block->locals_count;
	nextiter_instr->value.foreach.local = block->locals_count;

	kxcblock_insert_instruction(block, nextiter_instr, position);

	int bytes = kxcblock_insert_instructions(block, position, closures[0], codeblock);

	
	int jumpsize = bytes + kxinstructions_info[KXCI_NEXTITER].params_count + 1;
	nextiter_instr->value.foreach.jump = - jumpsize;

	instruction->type = rule->replacingInstruction;
	instruction->value.foreach.jump = jumpsize;
}

/**
 *  This function serves as replace function for KXCI_REPEAT, KXCI_TODO and KXCI_TODOBY
 */
static void
kxcblock_replace_instructions_cycle(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	int codeblock = prev_instructions[0]->value.codeblock;
	instruction->value.cycle.codeblock = codeblock;
	kxcblock_remove_instruction(block, position - 1);

	/* KXCI_XYZ_END instruction follow after KXCI_XYZ */
	int end_instrucion_id = rule->replacingInstruction + 1;
	KxcInstruction *end_instr = kxcinstruction_new(end_instrucion_id);

	/* This will be ignored if instruction is KXCI_REPEAT (KXCI_REPEAT_END) beceause 
	 * value.cycle.local (value.cycle_end.local)
	 * is last parameter and this istruction has only two parameters */
	instruction->value.cycle.local = block->locals_count;
	end_instr->value.cycle_end.local = block->locals_count;

	kxcblock_insert_instruction(block, end_instr, position);

	int bytes = kxcblock_insert_instructions(block, position, closures[0], codeblock);

	int jumpsize = bytes + kxinstructions_info[end_instrucion_id].params_count + 1;
	end_instr->value.cycle_end.jump = - jumpsize;

	instruction->type = rule->replacingInstruction;
	instruction->value.cycle.jump = jumpsize;
}


static void
kxcblock_replace_instructions_foreach_local(	
	KxcBlock *block,
	KxcInstruction *instruction,
	int position, 
	KxcBlock **closures, 
	KxcInstruction **prev_instructions,
	KxInstructionReplacingRule *rule)

{
	KxcInstruction *push_self = kxcinstruction_new(KXCI_PUSH_SELF);
	kxcblock_insert_instruction(block, push_self, position - 1);
	kxcblock_replace_instructions_foreach(block, instruction, position + 1, closures, prev_instructions, rule);
}

static int
kxcblock_optimise_check_subblocks(KxcBlock *block)
{
	int t;
	for (t = 0; t < block->subblocks->size; t ++ ) {
		KxcBlock *subblock = block->subblocks->items[t];
		if (!kxcblock_optimise_check_subblocks(subblock)) 
			return 0;
		
	}

	int s;
	for (s = 0; s < block->code->size; s++) {
		KxcInstruction *i = block->code->items[s];
		if (i->type == KXCI_PUSH_OUTER_LOCAL || i->type == KXCI_UPDATE_OUTER_LOCAL) {
			return 0;
		}
	}

	return 1;
}

static void 
kxcblock_optimise_replace_instructions(KxcBlock *block)
{
	int t;

	int prev_is_static_closure = 0;


	for (t=0;t<block->code->size;t++) {
		KxcInstruction *i = block->code->items[t];

		int closures_count = 0;
		while (i->type == KXCI_PUSH_BLOCK) {
			closures_count++;
			i = block->code->items[++t];
		}
		if (closures_count == 0) {
			continue;
		}
	
		KxcInstruction *previ[closures_count];
		KxcBlock *closures[closures_count];

		int s;
		for (s = 0; s < closures_count; s++) {
			previ[s] = block->code->items[t - closures_count + s];
			closures[s] = kxcblock_get_subblock_at(block, previ[s]->value.codeblock);

			if (closures[s]->subblocks->size > 0) {
				int i;
				int size = closures[s]->subblocks->size;
				for (i = 0; i < size; i++) {
					if (!kxcblock_optimise_check_subblocks(kxcblock_get_subblock_at(closures[s], i))) {
						break;
					}
				}
				if (i != size) {
					break;
				}
			}
		}
			
		if (s != closures_count) {
			continue;
		}


		KxInstructionReplacingRule *rule = replacing_rules;
		char *message_name = kxcblock_get_symbol_at(block, i->value.msg.symbol);

	// 	printf("%i %i %s\n", closures_count, i->type, message_name);
		while(rule->messageName) {
			if (i->type == rule->originalInstruction 
					&& closures_count >= rule->countOfClosures
					&& !strcmp(message_name, rule->messageName)) 
			{
				int delta = closures_count - rule->countOfClosures;

				for (s = 0; s < rule->countOfClosures;s++) {
					if (closures[s + delta]->params_count != rule->countOfParameters[s]) {
						break;
					}
				}

				if (s != rule->countOfClosures) {
					break;
				}

				rule->replace_function(block, i, t, closures + delta, previ + delta, rule);

				kxcblock_optimise_replace_instructions(block);
				return;
			}
			rule++;
		}
	}
}

static void 
kxcblock_optimise(KxcBlock *block)
{
	kxcblock_optimise_remove_pops(block);

	// Should be last optimazition, because removing instruction can demage jumps
	kxcblock_optimise_replace_instructions(block);
}

static void 
kxcblock_optimise_run(KxcBlock *block)
{
	int t;
	for (t=0;t<block->subblocks->size;t++) {
		kxcblock_optimise_run((KxcBlock *) block->subblocks->items[t]);
	}
	kxcblock_optimise(block);
}

void 
kxcblock_optimise_root_block(KxcBlock *block)
{
	kxcblock_optimise_run(block);
}
