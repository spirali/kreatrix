/*
   kxgc.c
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
	
#include <stdio.h>

#include "kxgc.h"
#include "kxobject.h"

extern int kx_verbose;

void 
kxgc_collect(KxCore *core) 
{
	kxcallback_call(core->callback_gc_start, core);

	kxobject_flag_reset(core->base_object, KXOBJECT_FLAG_GC);

	kxcore_mark(core);

	KxObject *base_object = core->base_object;
	

	KxObject *obj = base_object->gc_next;

	// --- DEBUG ---
	if (kx_verbose) {
		printf("--------------COLLECT---------------\n");
		while(obj != base_object) {
			if (!kxobject_flag_test(obj, KXOBJECT_FLAG_GC)) {
				printf("COLLECT::");
				kxobject_dump(obj);
			}
			obj = obj->gc_next;
		}
		obj = base_object->gc_next;
		printf("------------END_OF_COLLECT----------\n");
	}

	// DEBUG
//	kxobject_slots_dump(core->lobby);
/*	KxSymbol *dsym = kxcore_get_symbol(core, "Association");
	KxObject *assoc = kxobject_get_slot(core->lobby, dsym);
	kxobject_slots_dump(assoc);
	REF_REMOVE(assoc);
	REF_REMOVE(dsym);*/


	// ---- DEBUG end ----

	// count of objects
	/*obj = base_object->gc_next;
	int count = 0;
	while(obj != base_object) {
		count ++;
		obj = obj->gc_next;
	};
	printf("|--> count = %i\n", count);*/

	while(obj != base_object) {
		if (!kxobject_flag_test(obj, KXOBJECT_FLAG_GC)) {
			REF_ADD(obj);
			kxobject_clean(obj);
			KxObject *next = obj->gc_next;
	/*		if (obj->ref_count != 1) {
				printf("!-! Invalid collection ");
				kxobject_dump(obj);
			}*/
			REF_REMOVE(obj);
			obj = next;
		} else {
			kxobject_flag_reset(obj, KXOBJECT_FLAG_GC);
			obj = obj->gc_next;
		}
	}

	kxcallback_call(core->callback_gc_end, core);
}

void
kxgc_cleanall(KxCore *core) 
{
	KxObject *obj;

	KxObject *base_object = core->base_object;
	obj = base_object->gc_next;
	while(obj != base_object) {
		REF_ADD(obj);
		kxobject_clean(obj);
		KxObject *o = obj;
		obj = obj->gc_next;
		REF_REMOVE(o);
	}
}

void 
kxgc_dumpall(KxCore *core)
{
	printf("--------------DUMP---------------\n");

	KxObject *base_object = core->base_object;
	
	KxObject *obj = base_object->gc_next;

//	List *list = list_new();

	while(obj != base_object) {
		printf("DUMP::");
		kxobject_dump(obj);

		/*// DEBUG 
		list_append(list, obj);
		REF_ADD(obj);
		// END OF DEBUG*/

		obj = obj->gc_next;
	}

	printf("----------END_OF_DUMP------------\n");
/*
	list_foreach(list, (ListForeachFcn*) kxobject_clean);
	list_foreach(list, (ListForeachFcn*) kxobject_remove_all_parents);
	int t;
	for (t=0;t<list->size;t++) {
		KxObject *obj = list->items[t];
		if (obj->ref_count > 1)
			kxobject_dump(obj);
		REF_REMOVE(obj);
	}
	list_free(list);
	{
	printf("---\n");
	KxObject *base_object = core->base_object;
	
	KxObject *obj = base_object->gc_next;

	List *list = list_new();

	while(obj != base_object) {
		printf("DUMP::");
		kxobject_dump(obj);
		list_append(list, obj);
		REF_ADD(obj);
		obj = obj->gc_next;
	}
	}
*/
}
