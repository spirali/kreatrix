
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
