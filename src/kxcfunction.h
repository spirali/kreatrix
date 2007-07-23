/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXCFUNCTION_H
#define __KXCFUNCTION_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxstack.h"
#include "kxsymbol.h"
#include "kxmessage.h"
#include "kxtypes.h"

#define RETURN #error macro was remove
#define RETURN_BOOLEAN #error macro was remove
#define THROW #error macro was remove
#define THROW_EXCEPTION #error macro was remove

/** NEW **/
#define KXCHECK(kxobject) if ((kxobject) == NULL) return NULL
#define KXTHROW(kxobject) { kxstack_throw_object(KXCORE->stack, (kxobject)); return NULL; }
#define KXRETURN(kxobject) { REF_ADD(kxobject); return kxobject; }
#define KXRETURN_BOOLEAN(condition) { if (condition) KXRETURN(KXCORE->object_true) else KXRETURN(KXCORE->object_false); }

#define KXRETURN_TRUE KXRETURN(KXCORE->object_true)
#define KXRETURN_FALSE KXRETURN(KXCORE->object_false)

#define KXTHROW_WITH_STACK(kxobject, stack) { kxstack_throw_object((stack), (kxobject)); return NULL; }

#define KXCFUNCTION_OBJECTS ((KxCFunctionData*) KXCORE->stack->running_cfunction->data.ptr)->objects;

typedef struct KxObject KxCFunction;

typedef struct KxCFunctionData KxCFunctionData;


typedef KxObject * (KxExternFunction)(struct KxObject *self, KxMessage *message);

struct KxCFunctionData {
	KxObjectExtension *type_extension;
	int params_count;
	KxExternFunction *cfunction;
	KxObject **objects;
};

KxObject * kxcfunction_new_prototype(KxCore *core);
void kxcfunction_init_extenstion();
KxCFunction * kxcfuntion_clone_with(KxCFunction *self, KxObjectExtension *type_ext, int params_count, KxExternFunction *cfunction);
KxObject *kxcfunction_returnself(KxObject *self, KxMessage *message);
void kxcfunction_objects_set_one(KxCFunction *self, KxSymbol *symbol);
void kxcfunction_objects_set_array(KxCFunction *self, KxSymbol **array, int size);

extern KxObjectExtension kxcfunction_extension;

#endif
