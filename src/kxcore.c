/**********************************************************************
 *
 *  kxcore.c - Core of VM
 *
 *	$Id$
 *
 *  Implementation of KxCore
 *  KxCore represents instantion of VM, 
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <config.h>

#include "utils/utils.h"
#include "kxcore.h"
#include "kxbaseobject.h"
#include "kxsymbol.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxobject.h"
#include "kxstring.h"
#include "kxcodeblock.h"
#include "kxactivation.h"
#include "kxlobby.h"
#include "kxgc.h"
#include "kxboolean.h"
#include "kxscopedblock.h"
#include "kxexception.h"
#include "kxlist.h"
#include "kxnil.h"
#include "kxfile.h"
#include "kxbytearray.h"
#include "kxcharacter.h"
#include "kxdictionary.h"
#include "kxdynlib.h"
#include "kxmodule.h"
#include "kxfloat.h"
#include "kxset.h"
#include "kxarray2d.h"


/*#ifdef KX_DEBUG_GC
	#define GC_COUNTDOWN_INIT_VALUE 1 // DEBUG-GC option
#else
	#define GC_COUNTDOWN_INIT_VALUE 200000
#endif*/
#define GC_DEFAULT_TRESHOLD 1700
#define GC_TRESHOLD_GROW 120

#ifdef KX_THREADS_SUPPORT
#define KX_DEFAULT_YIELD_INTERVAL 210
#endif // KX_THREADS_SUPPORT

extern int kx_verbose;

static char *core_dictionary[KXDICTIONARY_SIZE] = 
{ "self","type","message","==","print","value","value:","value:value:","value:value:value:",
  "valueWithList:","init","doesNotUnderstand:parameters:","asString", "hash", "key",
 };

static void 
kxcore_add_basic_prototype(KxCore *self, KxBasicPrototypes proto, KxObject *object) 
{
	self->basic_prototypes[proto] = object;
}

void
kxcore_register_simple_exception(KxCore *core, char *group, char *name, char *type)
{
	KxException *excp = kxcore_clone_basic_prototype(core, KXPROTO_EXCEPTION);
	kxobject_set_type(excp,type);
	kxcore_register_exception(core,group,name, excp);
	REF_REMOVE(excp);

}

static void 
kxcore_std_exceptions(KxCore *core)
{
	kxcore_register_simple_exception(core, "vm", "DoesNotUnderstand", "DoesNotUnderstandException");
	kxcore_register_simple_exception(core, "vm", "IOException", "IOException");
}

KxCore *
kxcore_new() 
{
	KxCore *core = calloc(1,sizeof(KxCore));
	ALLOCTEST(core);

	#ifdef KX_THREADS_SUPPORT
		core->yield_counter = KX_DEFAULT_YIELD_INTERVAL;
		core->yield_interval = KX_DEFAULT_YIELD_INTERVAL;
	#endif // KX_THREADS_SUPPORT

	core->objects_gc_treshold = GC_DEFAULT_TRESHOLD;
	core->objects_count = 1;

	core->symbol_table = list_new();
	core->stack_list = list_new();

	core->prototypes = dictionary_new();
	core->registered_exceptions = dictionary_new();

	/** Init base object */
	core->base_object = kxbaseobject_new(core);

	// base object is begin of GC-Object circle list
	core->base_object->gc_prev = core->base_object;
	core->base_object->gc_next = core->base_object;
	/** base object inited */
	

	/* Prototypes */
	kxcore_add_basic_prototype(core, KXPROTO_SYMBOL, kxsymbol_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_CFUNCTION, kxcfunction_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_INTEGER, kxinteger_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_STRING, kxstring_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_CODEBLOCK, kxcodeblock_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_SCOPEDBLOCK, kxscopedblock_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_EXCEPTION, kxexception_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_LIST, kxlist_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_FILE, kxfile_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_BYTEARRAY, kxbytearray_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_CHARACTER, kxcharacter_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_DICTIONARY, kxdictionary_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_DYNLIB, kxdynlib_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_MODULE, kxmodule_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_FLOAT, kxfloat_new_prototype(core));
	kxcore_add_basic_prototype(core, KXPROTO_SET, kxset_new_prototype(core));

	kxsymbol_add_method_table(core->basic_prototypes[KXPROTO_SYMBOL]);

	/* True */
	core->object_true = kxboolean_new_true(core);

	/* False */
	core->object_false = kxboolean_new_false(core);

	/* Nil */
	core->object_nil = kxnil_new(core);

	/* stdout */
	core->object_stdout = kxfile_stdout(core);

	/* stdin */
	core->object_stdin = kxfile_stdin(core);

	/* stderr */
	core->object_stderr = kxfile_stderr(core);

	// Bootstrap problem: Methods must be added into baseobject after symbol_prototype is inited
	kxbaseobject_add_method_table(core->base_object);

	/* Init basic symbols */
	int t;
	for(t=0;t<KXDICTIONARY_SIZE;t++) {
		core->dictionary[t] = kxcore_get_symbol(core,core_dictionary[t]);
	}

	/** Must be after nil is inited */
	kxcore_add_basic_prototype(core, KXPROTO_ARRAY2D, kxarray2d_new_prototype(core));

	/* Lobby */
	core->lobby = kxlobby_new(core);

	kxobject_set_parent(core->base_object, core->lobby);
	
	kxcore_std_exceptions(core);



	return core;
}


void 
kxcore_add_prototype(KxCore *self, KxObject *prototype)
{
	REF_ADD(prototype);
	dictionary_add(self->prototypes,prototype->extension, prototype);
}

KxObject * 
kxcore_get_prototype(KxCore *self, KxObjectExtension *prototype_extension)
{
	KxObject *obj = (KxObject*) dictionary_get(self->prototypes,prototype_extension);
	if (obj == NULL) {
		fprintf(stderr,"Fatal error: Prototype '%s' not found\n", prototype_extension->type_name);
		exit(-1);
	}
	return obj;
}

KxObject * 
kxcore_clone_prototype(KxCore *self, KxObjectExtension *prototype_extension)
{
	KxObject *obj = kxcore_get_prototype(self, prototype_extension);
	return kxobject_clone(obj);
}


void
kxcore_remove_prototype(KxCore *self, KxObjectExtension *prototype_extension)
{
	KxObject *object = kxcore_get_prototype(self, prototype_extension);
	REF_REMOVE(object);
	dictionary_remove(self->prototypes, prototype_extension);
}


static void 
kxcore_registered_exception_clean(Dictionary *dict)
{
	dictionary_foreach_value(dict, kxobject_ref_remove);
	dictionary_free(dict);
}


void
kxcore_free(KxCore *self) 
{
	kxgc_collect(self);

	kxobject_clean(self->lobby);



	REF_REMOVE(self->lobby);



	int t;
	for(t=0;t<KXDICTIONARY_SIZE;t++) {
		REF_REMOVE(self->dictionary[t]);
	}

	for (t=0;t<KXPROTOS_COUNT;t++) {
		kxobject_clean(self->basic_prototypes[t]);
	}

	for (t=0;t<KXPROTOS_COUNT;t++) {
		REF_REMOVE(self->basic_prototypes[t]);
	}

	if (dictionary_size(self->prototypes) != 0) {
		fprintf(stderr,"Warning! Prototypes dictionary isn't empty\n");
	}


	REF_REMOVE(self->object_stdout);
	REF_REMOVE(self->object_stdin);
	REF_REMOVE(self->object_stderr);

	REF_REMOVE(self->object_true);
	REF_REMOVE(self->object_false);
	REF_REMOVE(self->object_nil);

	kxobject_slots_clean(self->base_object);
	kxobject_remove_all_parents(self->base_object);

	dictionary_foreach_value(self->registered_exceptions, (ForeachFcn*) kxcore_registered_exception_clean);
	
	dictionary_free(self->registered_exceptions);

	if (kx_verbose)
		kxgc_dumpall(self);
	//kxobject_dump(self->base_object);
	REF_REMOVE(self->base_object);

	list_free(self->stack_list);

	list_free(self->symbol_table);
	dictionary_free(self->prototypes);


	if (kx_verbose)
		printf("core->objects_count = %i\n", self->objects_count);
	free(self);
}

KxObject *
kxcore_get_basic_prototype(KxCore *core, KxBasicPrototypes proto) 
{
	return core->basic_prototypes[proto];
}

KxObject *
kxcore_clone_basic_prototype(KxCore *core, KxBasicPrototypes proto) 
{
	return kxobject_clone(core->basic_prototypes[proto]);
}

KxObject *
kxcore_clone_base_object(KxCore *self) 
{
	return kxobject_raw_clone(self->base_object);
}

static KxObject * 
kxcore_find_in_symboltable(KxCore *self, char *symbol_name)
{
	List *table = self->symbol_table;
	int t;
	for (t=0;t<table->size;t++)
	{
		if (!strcmp(KXSYMBOL_AS_CSTRING((KxObject *) table->items[t]), symbol_name))
			return table->items[t];
	}
	return NULL;
}

static void
kxcore_symbol_into_table(KxCore *self, KxSymbol *symbol)
{
	List *table = self->symbol_table;
	list_append(table, symbol);
}

KxObject *
kxcore_get_symbol(KxCore *self, char *symbol_name)
{
	KxObject *symbol = kxcore_find_in_symboltable(self, symbol_name);
	if (symbol) {
		REF_ADD(symbol);
		return symbol;
	}
	symbol = kxsymbol_clone_with(self->basic_prototypes[KXPROTO_SYMBOL], symbol_name);
	kxcore_symbol_into_table(self, symbol);

	return symbol;
}


void
kxcore_symbol_remove(KxCore *self, KxSymbol *symbol)
{
	int t;

	for (t=0;t<self->symbol_table->size;t++) {
		if (self->symbol_table->items[t] == symbol) {
			list_remove(self->symbol_table,t);
			return;
		}
	}
	
	fprintf(stderr,
	"Fatal Error: kxcore_symbol_remove: symbol not found\nSymbol name: %s\n", 
	(char*) symbol->data.ptr);

	exit(-1);
}

static void
kxcore_mark_registered_exceptions(Dictionary *dict)
{
	dictionary_foreach_value(dict, (ForeachFcn*)kxobject_mark);
}

void
kxcore_mark(KxCore *self) 
{
	int t;
	for (t=0;t<KXPROTOS_COUNT;t++)
		kxobject_mark(self->basic_prototypes[t]);
	for (t=0;t<KXDICTIONARY_SIZE;t++)
		kxobject_mark(self->dictionary[t]);

	kxobject_mark(self->object_true);
	kxobject_mark(self->object_false);
	kxobject_mark(self->object_nil);
	kxobject_mark(self->lobby);


	kxobject_mark(self->object_stdout);
	kxobject_mark(self->object_stdin);
	kxobject_mark(self->object_stderr);

	list_foreach(self->stack_list, (ListForeachFcn*) kxstack_mark);
	dictionary_foreach_value(self->prototypes, (ListForeachFcn*) kxobject_mark);
	dictionary_foreach_value(self->registered_exceptions, (ForeachFcn*) kxcore_mark_registered_exceptions);
	
}


void 
kxcore_register_stack(KxCore *self, KxStack *stack)
{
	list_append(self->stack_list, stack);
}

void
kxcore_unregister_and_free_all_stacks(KxCore *core)
{
	int t;
	for (t=0;t<core->stack_list->size;t++) {
		kxstack_free((KxStack*)core->stack_list->items[t]);
	}
	list_clean(core->stack_list);
}

// Stack isn't freed during unregistration
void
kxcore_unregister_stack(KxCore *self, KxStack *stack)
{
	List *list = self->stack_list;
	int t;
	for (t=0;t<list->size;++t) {
		if (list->items[t] == stack) {
			list_remove(list, t);
			return;
		}
	}
	printf("Warning: Trying unregister unknown stack\n");
}

static
void kxcore_gc_set_treshold(KxCore *core) 
{
	core->objects_gc_treshold = (3*core->objects_count) >> 1;

	#ifndef KX_DEBUG_GC
	/* if KX_DEBUG_GC is enabled, there is no need of this info */
	if (kx_verbose) {
		printf("gc_properities: now=%i treadshold=%i\n",
			core->objects_count, core->objects_gc_treshold);
	}
	#endif
}

void 
kxcore_collect_now(KxCore *core)
{
	int objects_count = core->objects_count;
	kxgc_collect(core);

	if (objects_count == core->objects_count) {
		/* GC collect no objects */
		kxcore_gc_set_treshold(core);
	} else {
		if ((core->objects_count+GC_TRESHOLD_GROW) >= core->objects_gc_treshold)
			core->objects_gc_treshold = core->objects_count + GC_TRESHOLD_GROW;
	/*		printf("gc_properities: now=%i treadshold=%i\n",
			core->objects_count, core->objects_gc_treshold);*/
	
	}
}

// TODO: lower treshold when objects_count is too low
void 
kxcore_gc_check(KxCore *core)
{
	#ifdef KX_DEBUG_GC
	/* Debug GC check - run gc without test*/
	kxcore_collect_now(core);
	#else 
	/* Normal GC check */
	if (core->objects_count > core->objects_gc_treshold) {
		kxcore_collect_now(core);
	} /*else if (core->objects_count < core->objects_gc_treshold >> 2) {
		core->objects_gc_treshold >>= 1;
	}*/
	#endif
}




void kxcore_switch_to_stack(KxCore *core, KxStack *stack)
{
/*	if (core->stack != stack) { printf("=>=>SWITCH TO: %p\n", stack); }
	else { printf("=>=>\n"); }*/
	core->stack = stack;
}

#ifdef KX_THREADS_SUPPORT

void kxcore_reset_yield_counter(KxCore *core) 
{
	core->yield_counter = core->yield_interval;
}

#endif // KX_THREADS_SUPPORT

void 
kxcore_register_exception(KxCore *core, char *group, char *name, KxObject *exception)
{
	Dictionary *dict;

	dict = dictionary_get_with_compare(core->registered_exceptions, group, (CompareFcn*)strcmp);
	if (dict == NULL) {
		dict = dictionary_new();
		dictionary_add(core->registered_exceptions, group, dict);
	} else {
		KxObject *excp = dictionary_get_with_compare(dict, name, (CompareFcn*)strcmp);
		if (excp) {
			REF_REMOVE(excp);
		}
	}
	REF_ADD(exception);
	dictionary_add_with_compare(dict, name, exception, (CompareFcn*) strcmp);
}

KxObject*
kxcore_get_registered_exception(KxCore *core, char *group, char *name) 
{
	Dictionary *dict;

	dict = dictionary_get_with_compare(core->registered_exceptions, group, (CompareFcn*)strcmp);
	if (dict != NULL) {
		return dictionary_get_with_compare(dict, name, (CompareFcn*)strcmp);
	}
	return NULL;

}

KxObject *
kxcore_clone_registered_exception(KxCore *core, char *group, char *name) 
{

	KxObject *excp = kxcore_get_registered_exception(core, group, name);
	if (excp == NULL) {
		return NULL;
	}	
	return excp;
}


KxObject *kxcore_clone_registered_exception_text(KxCore *core, char *group, char *name, char *form, ...)
{
	char buf[1024];

	va_list arg;
	va_start(arg,form);
	vsnprintf(buf,1024, form, arg);
	va_end(arg);

	KxObject *excp = kxcore_get_registered_exception(core, group, name);
	if (excp == NULL) {
		return kxexception_new_with_text(core,"Registered exception group=%s name=%s not found\n", group, name);
	}	
	KxObject *obj =  kxobject_clone(excp);
	KxString *str = kxstring_new_with(core, buf);
	kxobject_set_slot(obj, core->dictionary[KXDICT_MESSAGE], str);
	REF_REMOVE(str);
	return obj;
}
