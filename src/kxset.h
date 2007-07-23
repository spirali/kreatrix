/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXSET_H
#define __KXSET_H

#include "kxcore.h"
#include "kxobject_struct.h"

typedef struct KxObject KxSet;

#define IS_KXSET(kxobject) ((kxobject)->extension == &kxset_extension)

//#define KXSET(set) (kxset_new_with(KXCORE,set))

void kxset_init_extension();

KxSet *kxset_new_prototype(KxCore *core);


extern KxObjectExtension kxset_extension;


#endif // __KXSET_H
