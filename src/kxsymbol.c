/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kxsymbol.h"
#include "kxcore.h"
#include "kxobject.h"
#include "kxstring.h"

KxObjectExtension kxsymbol_extension;


static KxSymbol *kxsymbol_clone(KxSymbol *self);
static void kxsymbol_free(KxSymbol *self);


void kxsymbol_init_extension() 
{
	kxobjectext_init(&kxsymbol_extension);
	kxsymbol_extension.type_name = "Symbol";
	kxsymbol_extension.clone = kxsymbol_clone;
	kxsymbol_extension.free = kxsymbol_free;
}

KxSymbol *
kxsymbol_new_prototype(KxCore *core)
{
	KxObject *symbol = kxcore_clone_base_object(core);
	symbol->extension = &kxsymbol_extension;
	return symbol;
}



KxSymbol *
kxsymbol_clone_with(KxSymbol *self, char *symbolname)
{
	KxObject *symbol = kxobject_raw_clone(self);
	symbol->data.ptr = strdup(symbolname);

	return symbol;
}



static KxSymbol 
*kxsymbol_clone(KxSymbol *self) 
{
	// Symbol with same string value can be only one
	return self;	
}

static void 
kxsymbol_free(KxSymbol *self) 
{
	if (self->data.ptr) {
		kxcore_symbol_remove(KXCORE,self);
		free(self->data.ptr);
	}
}

KxObject *
kxsymbol_as_string(KxSymbol *self, KxMessage *message)
{
	return KXSTRING(self->data.ptr);
}

KxObject *kxstring_lt(KxString *self, KxMessage *message);


void 
kxsymbol_add_method_table(KxSymbol *self)
{
	KxMethodTable table[] = {
		{"<",1, kxstring_lt },
		{"asString",0, kxsymbol_as_string },
		{"asSymbol",0, kxcfunction_returnself },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}


