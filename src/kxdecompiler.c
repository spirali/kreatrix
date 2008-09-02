/*
   kxdecompiler.c
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

#include "kxdecompiler.h"
#include "compiler/kxinstr.h"

KxInstructionWrapper *
kxinstructionwrapper_new()
{
	KxInstructionWrapper *self = kxmalloc(sizeof(KxInstructionWrapper));
	ALLOCTEST(self);
	return self;
}

void 
kxinstructionwrapper_free(KxInstructionWrapper *self)
{
	kxfree(self);
}

static void
kxinstructionwrapper_dump(KxInstructionWrapper *self)
{
	KxInstructionInfo *info = &kxinstructions_info[self->type];
	switch(info->params_count) {
		case 0:
			printf("%s\n", info->name);
			break;
		case 1: 
			printf("%s %i\n", info->name, self->params[0]);
			break;
		case 2:
			printf("%s %i %i\n", info->name, self->params[0], self->params[1]);
			break;
	}
}

List *
kxdecompiler_instructions_list(KxCodeBlock *self)
{
	KxCodeBlockData *data = KXCODEBLOCK_DATA(self);
	char *code = data->code;

	List *list = list_new();

	if (code == NULL) {
		return list;
	}
	
	for(;;) {
		int instruction = *(code++);

		if (instruction == KXCI_INSTRUCTIONS_COUNT) {
			return list;
		}

		KxInstructionInfo *info = &kxinstructions_info[instruction];
		KxInstructionWrapper *iw = kxinstructionwrapper_new();
		iw->type = instruction;
		
		int t;
		for(t=0;t<info->params_count;t++) {
			iw->params[t] = *(code++);
		}
	//	kxinstructionwrapper_dump(iw);
		list_append(list, iw);

	}
}


void
kxdecompiler_dump_instructions(List *instructions)
{
	printf("Instructions dump: -------\n");
	int t;
	for (t=0;t<instructions->size;t++) {
		KxInstructionWrapper *iw = instructions->items[t];
		kxinstructionwrapper_dump(iw);
	}
	printf("------\n");
}
