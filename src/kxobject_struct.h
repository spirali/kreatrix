/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


#ifndef __KXOBJECT_STRUCT_H
#define __KXOBJECT_STRUCT_H

#include "utils/hash.h"
#include "kxobjectext.h"


#define KXOBJECT_SLOTSEARCH_MARK(self) (self)->slot_search_mark

#define KXOBJECT_SET_SLOTSEARCH_MARK(self) (self)->slot_search_mark = 1

#define KXOBJECT_RESET_SLOTSEARCH_MARK(self) (self)->slot_search_mark = 0


#define KXOBJECT_HAS_GC_MARK(self) ((self)->gc_mark)

typedef struct KxObject KxObject;
typedef struct KxParentSlot KxParentSlot;
typedef struct KxSlot KxSlot;

struct KxCore;

#define KXCORE ((self)->core)
#define KXCORE_FROM(object) ((object)->core)

#define KXSTACK KXCORE->stack
#define KXSTACK_FROM(object) KXCORE_FROM(object)->stack

struct KxParentSlot {
	KxObject *parent;
	KxParentSlot *next;
};

struct KxObject {
	KxSlot *slots;
	int slots_capacity;
	int slots_count;

	KxParentSlot parent_slot; // Linked list with parent slots
	//HashTable *slots;          // Hash table with slots
	
	
	union {
		void *ptr;
		double doubleval;
		int intval;
		char charval;
	} data;

    // Garbage collector atrributes
	KxObject *gc_prev; // Previous object in linked list of all objects
	KxObject *gc_next; // Next object in linked list of all objects
	int gc_mark;

	int ref_count;

	int slot_search_mark;

	KxObjectExtension *extension;

	struct KxCore *core;
};

struct KxSlot {
	KxObject *key;
	KxObject *value;
	int flags;
};


#endif // __KXOBJECT_H
