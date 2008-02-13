
#include <stdlib.h>
#include <stdio.h>

#include <kxmessage.h>
#include <kxobject.h>
#include <kxcfunction.h>

#include "kxinstructionobject.h"
#include "kxcore.h"
#include "kxinteger.h"
#include "compiler/kxinstr.h"
#include "kxexception.h"

KxObjectExtension kxinstructionobject_extension;

static void
kxinstructionobject_add_method_table(KxInstructionObject *self);

static void
kxinstructionobject_free(KxInstructionObject *self)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);

	kxinstructionwrapper_free(data->instruction);

	REF_REMOVE(data->codeblock);

	kxfree(data);
}

static void
kxinstructionobject_mark(KxInstructionObject *self)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	kxobject_mark(data->codeblock);
}

void kxinstructionobject_extension_init() {
	kxobjectext_init(&kxinstructionobject_extension);
	kxinstructionobject_extension.type_name = "Instruction";

	kxinstructionobject_extension.free = kxinstructionobject_free;
	kxinstructionobject_extension.mark = kxinstructionobject_mark;
}


static KxInstructionObjectData *
kxinstructionobjectdata_new(KxCodeBlock *codeblock, KxInstructionWrapper *instruction)
{
	KxInstructionObjectData *data = kxmalloc(sizeof(KxInstructionObjectData));
	ALLOCTEST(data);


	REF_ADD(codeblock);
	data->codeblock = codeblock;

	data->instruction = instruction;
	return data;
}

KxObject *
kxinstructionobject_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxinstructionobject_extension;
	
	KxInstructionWrapper *instruction = kxinstructionwrapper_new();
	instruction->type = KXCI_RETURN_SELF;
	KxObject *codeblock = kxcore_get_basic_prototype(core, KXPROTO_CODEBLOCK);
	self->data.ptr = kxinstructionobjectdata_new(codeblock, instruction);
	
	kxinstructionobject_add_method_table(self);
	return self;
}

KxObject *
kxinstructionobject_new_from(KxCore *core, KxCodeBlock *codeblock, KxInstructionWrapper *instruction)
{
	KxObject *self = kxcore_get_prototype(core, &kxinstructionobject_extension);
	KxObject *clone = kxobject_raw_clone(self);

	clone->data.ptr = kxinstructionobjectdata_new(codeblock, instruction);
	
	return clone;
}


static KxObject *
kxinstructionobject_instruction_type(KxInstructionObject *self, KxMessage *message)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	return KXINTEGER(data->instruction->type);
}



static KxObject *
kxinstructionobject_codeblock(KxInstructionObject *self, KxMessage *message)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	KXRETURN(data->codeblock);
}

static KxObject *
kxinstructionobject_parameters_count(KxInstructionObject *self, KxMessage *message)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	KxInstructionInfo *info = &kxinstructions_info[data->instruction->type];
	return KXINTEGER(info->params_count);
}

static KxObject *
kxinstructionobject_name(KxInstructionObject *self, KxMessage *message)
{
	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	KxInstructionInfo *info = &kxinstructions_info[data->instruction->type];
	return KXSTRING(info->name);
}

static KxObject *
kxinstructionobject_parameter_at(KxInstructionObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(index, 0);

	KxInstructionObjectData *data = KXINSTRUCTIONOBJECT_DATA(self);
	KxInstructionInfo *info = &kxinstructions_info[data->instruction->type];

	if (index < 0 || index >= info->params_count) {
		KxException *excp = kxexception_new_with_text(KXCORE, "Index is out of bounds");
		KXTHROW(excp);
	}

	return KXINTEGER(data->instruction->params[index]);
}


static void
kxinstructionobject_add_method_table(KxInstructionObject *self)
{
	KxMethodTable table[] = {
		{"instructionType",0, kxinstructionobject_instruction_type },
		{"codeblock",0, kxinstructionobject_codeblock },
		{"name",0, kxinstructionobject_name },
		{"parameterAt:",1, kxinstructionobject_parameter_at },
		{"parametersCount",0, kxinstructionobject_parameters_count },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

