/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXCHARACTER_H
#define __KXCHARACTER_H

#include "kxcore.h"
#include "kxobject_struct.h"

typedef struct KxObject KxCharacter;

#define IS_KXCHARACTER(kxobject) ((kxobject)->extension == &kxcharacter_extension)

#define KXCHARACTER_VALUE(kxcharacter) (char) (kxcharacter)->data.charval

#define KXCHARACTER(chr) kxcore_get_char(KXCORE, chr)

#define KXPARAM_TO_CHAR(c, param_id) \
	unsigned char c; { KxObject *tmp = message->params[param_id]; if (IS_KXCHARACTER(tmp)) \
	{ c = KXINTEGER_VALUE(tmp); } else { kxobject_type_error(tmp, &kxcharacter_extension, param_id); return NULL; }} 


void kxcharacter_init_extension();

KxObject *kxcharacter_new_prototype(KxCore *core);

KxCharacter * kxcharacter_new_with(KxCore *core, unsigned char ch);


KxObjectExtension kxcharacter_extension;

#endif // __KXCHARACTER_H
