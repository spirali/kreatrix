/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __BASEOBJECT_H
#define __BASEOBJECT_H

#include "kxobject.h"

KxObject *kxbaseobject_new(KxCore *core);

void kxbaseobject_add_method_table(KxObject *self);


#endif
