
#include <stdio.h>
#include <stdlib.h>

#include "block_optimise.h"

static void kxcblock_optimise_remove_pops(KxcBlock *block)
{
	int t;

	// start from 1, because first instruction cant be pop
	for (t=1;t<block->code->size;t++) {
		KxcInstruction *i = block->code->items[t];

		if (i->type == KXCI_POP) {
			KxcInstruction *prev = block->code->items[t-1];
			if (prev->type >= KXCI_PUSH_LOCAL0 && prev->type <= KXCI_PUSH_LOCALN) {
				list_fast_remove(block->code, t);
				list_fast_remove(block->code, t - 1);
				t--;
			}
		}
	}
}

static void kxcblock_optimise(KxcBlock *block)
{
	kxcblock_optimise_remove_pops(block);
}

static void kxcblock_optimise_run(KxcBlock *block)
{
	int t;
	for (t=0;t<block->subblocks->size;t++) {
		kxcblock_optimise_run((KxcBlock *) block->subblocks->items[t]);
	}
	kxcblock_optimise(block);
}

void kxcblock_optimise_root_block(KxcBlock *block)
{
	kxcblock_optimise_run(block);
}
