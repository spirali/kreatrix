/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXBYTEARRAY_H
#define __KXBYTEARRAY_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "utils/bytearray.h"

typedef struct KxObject KxByteArray;


#define IS_KXBYTEARRAY(kxobject) ((kxobject)->extension == &kxbytearray_extension)

#define KXBYTEARRAY(ba) kxbytearray_new_with(KXCORE, ba)

#define KXBYTEARRAY_DATA(kxbytearray) (ByteArray*) kxbytearray->data.ptr

#define KXPARAM_TO_BYTEARRAY(ba, param_id) \
	ByteArray * ba; { KxObject *tmp = message->params[param_id]; if (IS_KXBYTEARRAY(tmp)) \
	{ ba = KXBYTEARRAY_DATA(tmp); } else { kxbytearray_type_error(KXCORE); return NULL; }} 


void kxbytearray_init_extension();

KxByteArray *kxbytearray_new_prototype(KxCore *core);
KxByteArray *kxbytearray_new_with(KxCore *core, ByteArray *bytearray);


void kxbytearray_type_error(KxCore *core);

extern KxObjectExtension kxbytearray_extension;




#endif // __KXBYTECODE_H
