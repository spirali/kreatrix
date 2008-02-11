#ifndef __KXDECOMPILER_H
#define __KXDECOMPILER_H

#include "compiler/kxinstr.h"
#include "kxcodeblock.h"

typedef struct KxInstructionWrapper KxInstructionWrapper;

struct KxInstructionWrapper {
	KxInstructionType type;
	char params[2];
};


List *kxdecompiler_instructions_list(KxCodeBlock *self);
void kxdecompiler_dump_instructions(List *instructions);

KxInstructionWrapper *kxinstructionwrapper_new();
void kxinstructionwrapper_free(KxInstructionWrapper *self);


#endif // __KXDECOMPILER_H
