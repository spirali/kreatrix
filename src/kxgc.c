/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdio.h>

#include "kxgc.h"
#include "kxobject.h"

extern int kx_verbose;

void 
kxgc_collect(KxCore *core) 
{

	core->base_object->gc_mark = 0;

	kxcore_mark(core);

	KxObject *base_object = core->base_object;
	

	KxObject *obj = base_object->gc_next;

	// --- DEBUG ---
	if (kx_verbose) {
		printf("--------------COLLECT---------------\n");
		while(obj != base_object) {
			if (!obj->gc_mark) {
				printf("COLLECT::");
				kxobject_dump(obj);
			}
			obj = obj->gc_next;
		}
		obj = base_object->gc_next;
		printf("------------END_OF_COLLECT----------\n");
	}
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
		if (!obj->gc_mark) {
			REF_ADD(obj);
			kxobject_clean(obj);
			KxObject *next = obj->gc_next;
			if (obj->ref_count != 1) {
				printf("!-! Invalid collection ");
				kxobject_dump(obj);
			}
			REF_REMOVE(obj);
			obj = next;
		} else {
			obj->gc_mark = 0;
			obj = obj->gc_next;
		}
	}

}

void 
kxgc_dumpall(KxCore *core)
{
	printf("--------------DUMP---------------\n");

	KxObject *base_object = core->base_object;
	
	KxObject *obj = base_object->gc_next;
	while(obj != base_object) {
		printf("DUMP::");
		kxobject_dump(obj);
		obj = obj->gc_next;
	}

	printf("----------END_OF_DUMP------------\n");

}
