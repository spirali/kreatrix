/*************************
 *
 *  InstructionObject
 *
 */

#ifndef __KX_OBJECT_INSTRUCTIONOBJECT_H
#define __KX_OBJECT_INSTRUCTIONOBJECT_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxdecompiler.h"

#define KXINSTRUCTIONOBJECT_DATA(self) ((KxInstructionObjectData*) (self)->data.ptr)

#define IS_KXINSTRUCTIONOBJECT(self) ((self)->extension == &kxinstructionobject_extension)

typedef struct KxObject KxInstructionObject;
typedef struct KxInstructionObjectData KxInstructionObjectData;

struct KxInstructionObjectData {
	KxInstructionWrapper *instruction;
	KxCodeBlock *codeblock;
};

KxInstructionObject *kxinstructionobject_new_prototype(KxCore *core);
KxObject *kxinstructionobject_new_from(KxCore *core, KxCodeBlock *codeblock, KxInstructionWrapper *instruction);


void kxinstructionobject_extension_init();
extern KxObjectExtension kxinstructionobject_extension;


#endif // __KX_OBJECT_INSTRUCTIONOBJECT_H
