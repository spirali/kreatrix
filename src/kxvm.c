/*
   kxvm.c
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

	#ifdef KX_HOTSPOT
		if (!strcmp("hotspot",param)) {
			KXRETURN_TRUE;
		}
	#endif // KX_HOTSPOT


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

	#ifdef KX_HOTSPOT
	list_append(list, KXSTRING("hotspot"));
	#endif // KX_HOTSPOT


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

static KxObject *
kxvm_all_objects(KxObject *self, KxMessage *message)
{
	List *list = kxcore_list_with_all_objects(KXCORE);
	return KXLIST(list);
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
		{"allObjects", 0, kxvm_all_objects},
		{NULL,0, NULL}
	};

	kxobject_add_methods(self, table);

	kxobject_set_slot_no_ref2(self, kxcore_get_symbol(core,"version"), KXSTRING(PACKAGE_VERSION));

	return self;
}

