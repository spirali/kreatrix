/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXDICTIONARY_H
#define __KXDICTIONARY_H

#include "kxcore.h"
#include "kxobject_struct.h"

typedef struct KxObject KxDictionary;

#define IS_KXDICTIONARY(kxobject) ((kxobject)->extension == &kxdictionary_extension)

#define KXDICTIONARY(dictionary) (kxdictionary_new_with(KXCORE,dictionary))

void kxdictionary_init_extension();

KxDictionary *kxdictionary_new_prototype(KxCore *core);


extern KxObjectExtension kxdictionary_extension;


#endif // __KXDICTIONARY_H
