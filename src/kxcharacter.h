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

#define KXCHARACTER(chr) kxcharacter_new_with(KXCORE, chr)

void kxcharacter_init_extension();

KxObject *kxcharacter_new_prototype(KxCore *core);

KxCharacter * kxcharacter_new_with(KxCore *core, char ch);


KxObjectExtension kxcharacter_extension;

#endif // __KXCHARACTER_H
