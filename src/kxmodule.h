/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXMODULE_H
#define __KXMODULE_H

#include "kxcore.h"

typedef struct KxObject KxModule;

typedef struct KxModuleData KxModuleData;

#define KXMODULE_DATA(kxmodule) (KxModuleData*) (kxmodule)->data.ptr

struct KxModuleData {
	//void *handle;
	List *handle_list;
	char *path;
};

KxObject *kxmodule_new_prototype(KxCore *core);
void kxmodule_init_extension();



extern KxObjectExtension kxmodule_extension;

#endif // __KXMODULE_H
