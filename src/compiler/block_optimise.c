/*
   block_optimise.c
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

#include "block_optimise.h"

static void kxcblock_optimise_remove_pops(KxcBlock *block)
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
