/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <string.h>
#include "kxvm.h"
#include "kxcfunction.h"
#include "kxobject.h"
#include "kxstring.h"
#include "kxgc.h"
#include "kxglobals.h"
#include "kxinteger.h"
#include "kxlist.h"

extern int kx_doc_flag;

static KxObject * 
kxvm_doc_flag(KxObject *self, KxMessage *message) 
{
	KXRETURN_BOOLEAN(kx_doc_flag);
}


static KxObject *
kxvm_quit(KxObject *self, KxMessage *message) 
{
	KxObject *obj = message->params[0];
	REF_ADD(obj);
	kxstack_set_return_object(KXSTACK, obj);
	kxstack_set_return_state(KXSTACK, RET_EXIT);
	return NULL;
}

static KxObject *
kxvm_collect(KxObject *self, KxMessage *message)
{
	/*kxgc_collect(KXCORE);
	kxcore_reset_gc_countdown(KXCORE);*/
	kxcore_collect_now(KXCORE);
	KXRETURN(self);
}

static KxObject *
kxvm_objects_count(KxObject *self, KxMessage *message)
{
	return KXINTEGER(KXCORE->objects_count);
}

static KxObject *
kxvm_gc_treshold(KxObject *self, KxMessage *message)
{
	return KXINTEGER(KXCORE->objects_gc_treshold);
}

static KxObject *
kxvm_has_support(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param, 0);
	if (kx_threads_support && !strcmp("threads",param)) {
		KXRETURN_TRUE;
	}
	
	#ifdef KX_LOG
		if (!strcmp("log",param)) {
			KXRETURN_TRUE;
		}
	#endif // KX_LOG

	KXRETURN_FALSE;
}

static KxObject *
kxvm_support_list(KxObject *self, KxMessage *message)
{
	List *list = list_new();

	if (kx_threads_support) {
		list_append(list, KXSTRING("threads"));
	}
	
	#ifdef KX_LOG
	list_append(list, KXSTRING("log"));
	#endif // KX_LOG

	return KXLIST(list);
}


static KxObject *
kxvm_get_registered_exception(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(group, 0);
	KXPARAM_TO_CSTRING(name, 1);

	KxObject *excp = kxcore_get_registered_exception(KXCORE,group, name);
	if (excp == 0) {
		KXRETURN(KXCORE->object_nil);
	}
	REF_ADD(excp);
	return excp;
}

static KxObject *
kxvm_push_local_import_path(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(filename,0);
	kxcore_push_local_import_path(KXCORE,strdup(filename));
	KXRETURN(self);
}

static KxObject *
kxvm_pop_local_import_path(KxObject *self, KxMessage *message)
{
	kxcore_pop_local_import_path(KXCORE);
	KXRETURN(self);
}

static KxObject *
kxvm_top_local_import_path(KxObject *self, KxMessage *message)
{
	return KXSTRING(kxcore_top_local_import_path(KXCORE));
}

static KxObject *
kxvm_log_write(KxObject *self, KxMessage *message)
{
	#ifndef KX_LOG 
		KXRETURN(self);
	#else 
		KXPARAM_TO_CSTRING(string,0);
		kx_log_write_raw(string);
		KXRETURN(self);
	#endif // KX_LOG
}

KxObject *
kxvm_new(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);

	kxobject_set_slot_no_ref2(self,kxcore_get_symbol(core,"type"),kxstring_new_with(core,"VM")); 


	KxMethodTable table[] = {
		{"docFlag",0, kxvm_doc_flag },
		{"quit:",1, kxvm_quit },
		{"collect",0, kxvm_collect },
		{"hasSupport:",1, kxvm_has_support},
		{"objectsCount",0, kxvm_objects_count},
		{"gcTreshold",0, kxvm_gc_treshold},
		{"inGroup:exception:", 2, kxvm_get_registered_exception},
		{"pushLocalImportPath:", 1, kxvm_push_local_import_path},
		{"popLocalImportPath", 0, kxvm_pop_local_import_path},
		{"topLocalImportPath", 0, kxvm_top_local_import_path},
		{"logWrite:", 1, kxvm_log_write},
		{"supportList", 0, kxvm_support_list},
		{NULL,0, NULL}
	};

	kxobject_add_methods(self, table);

	kxobject_set_slot_no_ref2(self, kxcore_get_symbol(core,"version"), KXSTRING(PACKAGE_VERSION));

	return self;
}

