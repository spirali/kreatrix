/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXCODEBLOCK_H
#define __KXCODEBLOCK_H

#include "kxobject_struct.h"
#include "kxsymbol.h"
#include "kxcore.h"
#include "kxtypes.h"
#include "kxmessage.h"

#define IS_KXCODEBLOCK(kxobject) ((kxobject)->extension == &kxcodeblock_extension)

#define KXCODEBLOCK_DATA(kxcodeblock) ((KxCodeBlockData*) ((kxcodeblock)->data.ptr))

typedef struct KxObject KxCodeBlock;

typedef struct KxCodeBlockData KxCodeBlockData;

typedef enum { KXCODEBLOCK_METHOD, KXCODEBLOCK_BLOCK } KxCodeBlockType;

struct KxCodeBlockData {

	KxCodeBlockType type;

	char *code;

	int params_count;

	int locals_count;
	KxSymbol **locals_symbols;

	KxObject **prealocated_locals;

	int symbol_frame_size;
	KxSymbol **symbol_frame;


	
	//KxCodeBlock *parent_codeblock; // NULL for method

	//KxObject *scope;

	int subcodeblocks_size;
	KxCodeBlock **subcodeblocks;

	char *source_filename;
	int *message_linenumbers;

};

void kxcodeblock_init_extension();

KxObject *kxcodeblock_new_prototype(KxCore *core);

KxCodeBlock *kxcodeblock_new_from_bytecode(KxCore *core, char **bytecode, char *source_filname);

KxObject * kxcodeblock_activate(KxCodeBlock *self, KxObject *target, KxMessage *message);

KxObject * kxcodeblock_run(KxCodeBlock *self, KxObject *target, KxMessage *message);
KxObject * kxcodeblock_run_scoped(KxCodeBlock *self, struct KxActivation *parent_activation, KxMessage *message);


//KxObject * kxcodeblock_run_with_direct_target(KxCodeBlock *self, KxObject *target, KxMessage *message);

extern KxObjectExtension kxcodeblock_extension;

#endif
