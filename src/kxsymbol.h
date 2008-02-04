/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


#ifndef __KXSYMBOL_H
#define __KXSYMBOL_H

#include "kxcore.h"

#define IS_KXSYMBOL(self) ((self)->extension==&kxsymbol_extension)
#define KXSYMBOL_AS_CSTRING(symbol) (char*) (symbol)->data.ptr

#define KXCHECK_SYMBOL(object) if (!IS_KXSYMBOL(object)) { KXTHROW(kxobject_type_error( (object), &kxsymbol_extension)); }

typedef struct KxObject KxSymbol;


void kxsymbol_init_extension();

KxSymbol *kxsymbol_new_prototype(KxCore *core);


KxSymbol *kxsymbol_clone_with(KxSymbol *self, char *symbolname);


extern KxObjectExtension kxsymbol_extension;

void kxsymbol_add_method_table(KxSymbol *self);

#endif // __KXSYMBOL_H
