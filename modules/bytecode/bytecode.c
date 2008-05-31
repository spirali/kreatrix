
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxlist.h"
#include "kxinstructionobject.h"
#include "compiler/kxinstr.h"


static void kxcodeblock_extra_slots(KxObject *self);
static void kxbytecode_add_constants(KxObject *self);

KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxinstructionobject_extension_init();

	KxObject *instructionobject = kxinstructionobject_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, instructionobject);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Instruction"), instructionobject);

	KxObject *codeblock;
	codeblock = kxcore_get_basic_prototype(KXCORE, KXPROTO_CODEBLOCK);
	kxcodeblock_extra_slots(codeblock);

	kxbytecode_add_constants(instructionobject);
	
	KXRETURN(self);
}


void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxinstructionobject_extension);
}

static KxObject *
kxcodeblock_instructions_list(KxObject *self, KxMessage *message)
{
	List *list = kxdecompiler_instructions_list(self);
	List *list2 = list_new_size(list->size);
	int t;

	for (t=0;t<list->size;t++) {
		list2->items[t] = kxinstructionobject_new_from(KXCORE, self, list->items[t]);
	}
	list_free(list);

	return KXLIST(list2);
}

static void
kxbytecode_add_constants(KxObject *self) 
{
	int t;
	List *list = list_new();

	for (t=0; t < KXCI_INSTRUCTIONS_COUNT; t++) {
		KxInstructionInfo *info = &kxinstructions_info[t];
		if (strcmp(info->name, "reserved")) {
			int len = strlen(info->name);
			char str[len];
			int s;
			for (s=0;s<len;s++) {
				str[s] = toupper(info->name[s]);
			}
			str[len] = 0;
			KxSymbol *symbol = kxcore_get_symbol(KXCORE, str);
			REF_ADD(symbol);
			list_append(list, symbol);
		    kxobject_set_slot_no_ref2(self,symbol, KXINTEGER(t));
		}
	}

	list_collapse(list);
	KxList *listobj = KXLIST(list);
	kxobject_set_slot_no_ref2(self, kxcore_get_symbol(KXCORE, "allTypes"), listobj);
}

static void 
kxcodeblock_extra_slots(KxObject *self) 
{
	KxMethodTable table[] = {
		{"instructionsList",0, kxcodeblock_instructions_list },
		{NULL,0,NULL}
	};
	kxobject_add_methods(self, table);
}
