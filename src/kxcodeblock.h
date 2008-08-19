/*
   kxcodeblock.h
   Copyright (C) 2007, 2008  Stanislav Bohm

   This file is part of Kreatrix.

   Kreatrix is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Kreatrix is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Kreatrix; see the file COPYING. If not, see 
   <http://www.gnu.org/licenses/>.
*/
	
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
typedef struct KxCodeBlockInlineCache KxCodeBlockInlineCache;

typedef enum { KXCODEBLOCK_METHOD, KXCODEBLOCK_BLOCK } KxCodeBlockType;

#ifdef KX_HOTSPOT
	#define KXCODEBLOCK_RUN_COUNTER_START_VALUE -100
#endif 

struct KxCodeBlockData {
	
	#ifdef KX_HOTSPOT
	int run_counter;
	#endif

	KxCodeBlockType type;


	KxObject **literals;
	KxSymbol **symbol_frame;

	KxObject **prealocated_locals;

	int symbol_frame_size;
	int literals_count;

	char *code;

	int params_count;
	int locals_count;
	KxSymbol **locals_symbols;

	KxCodeBlockInlineCache *inline_cache;
	int inline_cache_size;
	KxCodeBlock *next_codeblock_with_inline_cache;
	
	//KxCodeBlock *parent_codeblock; // NULL for method

	//KxObject *scope;

	int subcodeblocks_size;
	KxCodeBlock **subcodeblocks;

	char *source_filename;
	int *message_linenumbers;

	char *created_slots;
};

struct KxCodeBlockInlineCache {
	KxObject *prototype;
	KxObject *cached_object;
	KxObject *message_name;
	KxObject *slot_holder;
};

void kxcodeblock_init_extension();

KxObject *kxcodeblock_new_prototype(KxCore *core);

KxCodeBlock *kxcodeblock_new_from_bytecode(KxCore *core, char **bytecode, char *source_filname);

KxObject * kxcodeblock_activate(KxCodeBlock *self, KxObject *target, KxMessage *message);

KxObject * kxcodeblock_run(KxCodeBlock *self, KxObject *target, KxMessage *message);
KxObject * kxcodeblock_run_scoped(KxCodeBlock *self, struct KxActivation *parent_activation, KxMessage *message);
int kxcodeblock_message_name_id(KxCodeBlock *self, KxSymbol *message_name);
void kxcodeblock_insert_inline_cache_instructions(KxCodeBlock *self);

void kx_inline_cache_repair_prototype(KxObject *prototype);
void kx_inline_cache_repair_prototype_and_name(KxObject *prototype, KxSymbol *message_name);


//KxObject * kxcodeblock_run_with_direct_target(KxCodeBlock *self, KxObject *target, KxMessage *message);

extern KxObjectExtension kxcodeblock_extension;

#endif
