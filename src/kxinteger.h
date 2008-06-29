/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXINTEGER_H
#define __KXINTEGER_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxstack.h"

typedef struct KxObject KxInteger;

#define IS_KXINTEGER(kxobject) ((kxobject)->extension == &kxinteger_extension)

#define KXINTEGER_VALUE(kxinteger) (KxInt) (kxinteger)->data.intval


#define KXPARAM_TO_LONG(integer, param_id) \
	long integer; { KxObject *tmp = message->params[param_id]; if (IS_KXINTEGER(tmp)) \
	{ integer = KXINTEGER_VALUE(tmp); } else { kxinteger_type_error(KXCORE); return NULL; }} 

#define KXPARAM_TO_INT(integer, param_id) \
	int integer; { KxObject *tmp = message->params[param_id]; if (IS_KXINTEGER(tmp)) \
	{ integer = KXINTEGER_VALUE(tmp); } else { kxobject_type_error(tmp,&kxinteger_extension); return NULL; }} 


#define KXINTEGER(integer) kxcore_get_integer(KXCORE,integer)

typedef long KxInt;

extern KxObjectExtension kxinteger_extension;

void kxinteger_init_extension();

KxObject *kxinteger_new_prototype(KxCore *core);

KxInteger *kxinteger_clone_with(KxInteger *self, KxInt value);

KxInteger *kxinteger_new_with(KxCore *core, KxInt value);

void kxinteger_type_error(KxCore *core);

#endif
