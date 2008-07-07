
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block_optimise.h"

typedef struct KxInstructionReplacingRule KxInstructionReplacingRule;

struct KxInstructionReplacingRule {
	int originalInstruction;
	char *messageName;
	int replacingInstruction;
	int countOfParameters;
};

static KxInstructionReplacingRule replacing_rules[] = {
	{ KXCI_KEYWORD_MSG, "ifTrue:", KXCI_IFTRUE, 0 },
	{ KXCI_KEYWORD_MSG, "ifFalse:", KXCI_IFFALSE, 0 },
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

static int
kxcblock_insert_instructions(KxcBlock *block, int position, KxcBlock *source)
{
	int t;
	int bytes = 0;

	int start_id_of_locals = block->locals_count;
	kxcblock_append_locals(block, source);
	kxcblock_insert_linenumbers(block, source, kxcblock_get_line_index_from_position(block, position));

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
kxcblock_optimise_replace_instructions(KxcBlock *block)
{
	int t;

	int prev_is_static_closure = 0;


	for (t=0;t<block->code->size;t++) {
		KxcInstruction *i = block->code->items[t];

		if (prev_is_static_closure) {
			if (i->type == KXCI_PUSH_BLOCK) {
				continue;
			}
			prev_is_static_closure = 0;

			KxcInstruction *prev = block->code->items[t - 1];
			KxcBlock *closure = kxcblock_get_subblock_at(block, prev->value.codeblock);
			int closure_params = closure->params_count;
			
			//TODO: Expand codeblocks with subblocks
			if (closure->subblocks->size > 0) 
				continue;

			KxInstructionReplacingRule *rule = replacing_rules;
			char *message_name = kxcblock_get_symbol_at(block, i->value.msg.symbol);
			while(rule->messageName) {
				if (i->type == rule->originalInstruction 
						&& closure->params_count == rule->countOfParameters
						&& !strcmp(message_name, rule->messageName)) 
				{
					i->value.extra.codeblock = prev->value.codeblock;
					kxcblock_remove_instruction(block, t - 1);
					int bytes = kxcblock_insert_instructions(block, t, closure);
					i->type = rule->replacingInstruction;
					i->value.extra.jump = bytes;
					kxcblock_optimise_replace_instructions(block);
					return;
				}
				rule++;
			}
		} else {
			if (i->type == KXCI_PUSH_BLOCK) {
				prev_is_static_closure = 1;
				continue;
			}

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
