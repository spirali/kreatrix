/*
   kxobject_struct.h
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
	
#ifndef __KXOBJECT_STRUCT_H
#define __KXOBJECT_STRUCT_H

#include "utils/hash.h"
#include "kxobjectext.h"


#define kxobject_flag_set(self, flag) ((self)->flags |= (flag))
#define kxobject_flag_reset(self, flag) ((self)->flags &= (~(flag)))
#define kxobject_flag_test(self, flag) ((self)->flags & (flag))

#define KXOBJECT_FLAG_GC                  0x01
#define KXOBJECT_FLAG_PROFILE_REPAIR_MARK 0x02

#define kxobject_recursive_mark_set(self)   ((self)->recursive_mark = 1)
#define kxobject_recursive_mark_reset(self) ((self)->recursive_mark = 0)
#define kxobject_recursive_mark_test(self)  ((self)->recursive_mark)

#define KXOBJECT_SIMPLE_SLOTS_LIMIT 5

typedef struct KxObject KxObject;
typedef struct KxParentSlot KxParentSlot;
typedef struct KxSlot KxSlot;
typedef struct KxObjectProfile KxObjectProfile;

struct KxCore;

struct KxParentSlot {
	KxObject *parent;
	KxParentSlot *next;
};

typedef enum {
	KXOBJECT_INSTANCE = 0,
	KXOBJECT_PROTOTYPE,
	KXOBJECT_SINGLETON,
} KxObjectPType;

struct KxObjectProfile {
	List *slots_symbols;
	List *child_prototypes;
};

struct KxObjectData2 {
	void *ptr1;
	void *ptr2;
};

struct KxObject {
	int ref_count;

	KxSlot *slots;
	int slots_capacity;

	KxParentSlot parent_slot; // Linked list with parent slots
	//HashTable *slots;          // Hash table with slots
	
	int recursive_mark; // Used to avoid lookup loops
	union {
		void *ptr;
		double doubleval;
		int intval;
		unsigned char charval;
		struct KxObjectData2 data2;
	} data;

	#ifdef KX_INLINE_CACHE
	KxObjectPType ptype;
	KxObjectProfile *profile;
	#endif // KX_INLINE_CACHE

	int flags;

    // Garbage collector atrributes
	KxObject *gc_prev; // Previous object in linked list of all objects
	KxObject *gc_next; // Next object in linked list of all objects

	KxObjectExtension *extension;

	#ifdef KX_MULTI_STATE
	struct KxCore *core;
	#endif
};

struct KxSlot {
	KxObject *key;
	KxObject *value;
	int flags;
};


#endif // __KXOBJECT_H
