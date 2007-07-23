/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXSTRING_H
#define __KXSTRING_H

#include "kxobject_struct.h"
#include "kxcore.h"
//#include "kxstack.h"

typedef struct KxObject KxString;

#define IS_KXSTRING(kxobject) ((kxobject)->extension == &kxstring_extension)

#define KXSTRING_VALUE(kxinteger) (char*) ((kxinteger)->data.ptr)
#define KXSTRING_TO_CSTRING(kxinteger) (char*) ((kxinteger)->data.ptr)


#define KXPARAM_TO_CSTRING(str, param_id) \
	char * str; { KxObject *tmp = message->params[param_id]; if (IS_KXSTRING(tmp)) \
	{ str = KXSTRING_VALUE(tmp); } else { kxstring_type_error(KXCORE); return NULL; }} 


#define KXSTRING(str) kxstring_new_with(KXCORE,str)


extern KxObjectExtension kxstring_extension;

void kxstring_init_extension();

KxObject *kxstring_new_prototype(KxCore *core);

KxString *kxstring_clone_with(KxString *self, char * value);

KxString *kxstring_new_with(KxCore *core, char *value);

void kxstring_type_error(KxCore *core);

extern KxObjectExtension kxstring_extension;


#endif
