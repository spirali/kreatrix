/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXDYNLIB_H
#define __KXDYNLIB_H

#include "kxcore.h"

typedef struct KxObject KxDynLib;

typedef struct KxDynLibData KxDynLibData;

struct KxDynLibData {
	void *handle;
};

KxObject *kxdynlib_new_prototype(KxCore *core);
void kxdynlib_init_extension();



extern KxObjectExtension kxdynlib_extension;

#endif // __KXDYNLIB_H
