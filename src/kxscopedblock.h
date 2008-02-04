/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXSCOPEDBLOCK_H
#define __KXSCOPEDBLOCK_H


#include "kxcodeblock.h"

#define IS_KXSCOPEDBLOCK(kxobject) ((kxobject)->extension == &kxscopedblock_extension)

#define KXSCOPEDBLOCK_DATA(kxscopedblock) ((KxScopedBlockData*) ((kxscopedblock)->data.ptr))


typedef struct KxObject KxScopedBlock;

typedef struct KxScopedBlockData KxScopedBlockData;

struct KxActivation;

struct KxScopedBlockData {
	KxCodeBlock *codeblock;
	struct KxActivation *scope;
	int is_running;
};

void kxscopedblock_init_extension();
KxObject *kxscopedblock_new_prototype(KxCore *core);

KxScopedBlock *kxscopedblock_new(KxCore *core, KxCodeBlock *codeblock, struct KxActivation *scope);

KxObject * kxscopedblock_run(KxScopedBlock *self, KxMessage *message);

extern KxObjectExtension kxscopedblock_extension;

#endif 
