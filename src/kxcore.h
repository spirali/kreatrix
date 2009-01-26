/*
   kxcore.h
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
	
#ifndef __KXCORE_H
#define __KXCORE_H

#include "kxobject_struct.h"
#include "kxstack.h"
#include "utils/list.h"
#include "utils/dictionary.h"
#include "kxcallback.h"

typedef enum { 
KXPROTO_SYMBOL, KXPROTO_CFUNCTION, KXPROTO_INTEGER, 
KXPROTO_STRING, KXPROTO_CODEBLOCK, 
KXPROTO_SCOPEDBLOCK, KXPROTO_EXCEPTION, KXPROTO_LIST,
KXPROTO_FILE, KXPROTO_BYTEARRAY, KXPROTO_CHARACTER,
KXPROTO_DICTIONARY, KXPROTO_FLOAT,
KXPROTO_MODULE,KXPROTO_SET,KXPROTO_ARRAY2D, KXPROTO_LOCALCONTEXT,
KXPROTO_ITERATOR, KXPROTO_ASSOCIATION,
KXPROTOS_COUNT } KxBasicPrototypes;

typedef enum {

KXDICT_SELF, KXDICT_TYPE, KXDICT_MESSAGE, KXDICT_EQ,
KXDICT_PRINT, KXDICT_VALUE, KXDICT_VALUE1, KXDICT_VALUE2,
KXDICT_VALUE3, KXDICT_VALUE_WITH_LIST, KXDICT_INIT, 
KXDICT_DNU, KXDICT_AS_STRING, KXDICT_HASH, 
KXDICT_KEY, KXDICT_PRINTSTRING,
KXDICT_IFTRUE, KXDICT_IFFALSE, KXDICT_IFTRUE_IFFALSE, KXDICT_IFFALSE_IFTRUE,
KXDICT_FOREACH, KXDICT_REPEAT, KXDICT_TODO, KXDICT_TOBYDO,

KXDICTIONARY_SIZE
} KxCoreDictionary;


typedef struct KxCore KxCore;

#ifdef KX_MULTI_STATE

	#define KXCORE ((self)->core)
	#define KXCORE_FROM(object) ((object)->core)

	#else // not KX_MULTI_STATE

	extern KxCore *kx_global_core;

	#define KXCORE (kx_global_core)
	#define KXCORE_FROM(object) (kx_global_core)

#endif // KX_MULTI_STATE

#define KXSTACK KXCORE->stack
#define KXSTACK_FROM(object) KXCORE_FROM(object)->stack


#define KXSYMBOL(cstring) kxcore_get_symbol(KXCORE,cstring)

#ifdef KX_THREADS_SUPPORT
#define KX_THREADS_BEGIN { KxStack *__stack = (KXCORE)->stack;\
						  if ((KXCORE)->gil_unlock) \
						        (KXCORE)->gil_unlock(KXCORE);
#define KX_THREADS_END if ((KXCORE)->gil_lock) (KXCORE)->gil_lock(KXCORE);\
						/*(core)->stack = __stack;*/\
						kxcore_switch_to_stack(KXCORE, __stack); }
typedef void(KxGILFcn)(KxCore*);

#else

#define KX_THREADS_BEGIN 
#define KX_THREADS_END


#endif // KX_THREADS_SUPPORT

#define KXCORE_IS_BLOCKED(core) (core)->blocked

#define KXCORE_OBJECT_CACHE_SIZE 50

#define KXCORE_SLOT_CACHE_CAPACITIES_COUNT 12
#define KXCORE_SLOT_CACHE_SIZE 40

#define KXCORE_ACTIVATION_CACHE_SIZE 20

#define KXCORE_CHAR_CACHE_SIZE 128

#define KXCORE_INTEGER_CACHE_START -10
#define KXCORE_INTEGER_CACHE_SIZE 100

typedef void(KxMarkFunction)(KxCore*);

struct KxCore  {
	KxStack *stack;

	KxObject *base_object;

	int objects_count;
	int objects_gc_treshold;

	KxObject *object_true;
	KxObject *object_false;
	KxObject *object_nil;

	KxObject *object_stdout;
	KxObject *object_stdin;
	KxObject *object_stderr;


	KxObject *lobby;

	List *symbol_table;
	

	List *stack_list;

	Dictionary *registered_exceptions;

/*	int gc_countdown;
	int gc_countdown_init;*/

	int doc_flag;

	int blocked; // 1 if core is destorying and no KX code should be executed

	#ifdef KX_THREADS_SUPPORT
		int yield_counter;
		int yield_interval;
		void *gil;
		KxGILFcn *gil_lock;
		KxGILFcn *gil_unlock;
	#endif

	Dictionary *prototypes;
	KxObject *dictionary[KXDICTIONARY_SIZE];
	KxObject *basic_prototypes[KXPROTOS_COUNT];

	Dictionary *global_data;

	List *mark_functions;

	List *local_import_paths;

	KxCallback *callback_gc_start;
	KxCallback *callback_gc_end;
	
	KxObject *first_codeblock_with_inline_cache;
	KxObject *last_codeblock_with_inline_cache;

	int activation_cache_count;
	void *activation_cache[KXCORE_ACTIVATION_CACHE_SIZE];

	int object_cache_pos;
	KxObject *object_cache[KXCORE_OBJECT_CACHE_SIZE];

	int slot_cache_count[KXCORE_SLOT_CACHE_CAPACITIES_COUNT];
	void *slot_cache[KXCORE_OBJECT_CACHE_SIZE][KXCORE_SLOT_CACHE_SIZE];
	
	KxObject *integer_cache[KXCORE_INTEGER_CACHE_SIZE];
	KxObject *char_cache[KXCORE_CHAR_CACHE_SIZE];
};

KxCore *kxcore_new();
void kxcore_free(KxCore *self);


KxObject *kxcore_clone_base_object(KxCore *self);

KxObject *kxcore_get_symbol(KxCore *self, char *symbol_name);

void kxcore_symbol_remove(KxCore *self, KxObject *symbol);
void kxcore_mark(KxCore *self);


KxObject *kxcore_get_basic_prototype(KxCore *core, KxBasicPrototypes proto);

void kxcore_register_stack(KxCore *self, KxStack *stack);

void kxcore_unregister_and_free_all_stacks(KxCore *core);

void kxcore_add_prototype(KxCore *self, KxObject *prototype);
KxObject * kxcore_get_prototype(KxCore *self, KxObjectExtension *prototype_extension);
void kxcore_remove_prototype(KxCore *self, KxObjectExtension *prototype_extension);


KxObject *kxcore_clone_prototype(KxCore *self, KxObjectExtension *prototype_extension);

void kxcore_gc_check(KxCore *core);
void kxcore_collect_now(KxCore *core);
KxObject * kxcore_clone_basic_prototype(KxCore *self, KxBasicPrototypes proto);
KxObject * kxcore_raw_clone_basic_prototype(KxCore *core, KxBasicPrototypes proto);


void kxcore_switch_to_stack(KxCore *core, KxStack *stack);
void kxcore_unregister_stack(KxCore *self, KxStack *stack);

void kxcore_register_exception(KxCore *core, char *group, char *name, KxObject *exception);
void
kxcore_register_simple_exception(KxCore *core, char *group, char *name, char *type);

KxObject *kxcore_get_registered_exception(KxCore *core, char *group, char *name);
KxObject *kxcore_clone_registered_exception(KxCore *core, char *group, char *name);
KxObject *kxcore_clone_registered_exception_text(KxCore *core, char *group, char *name, char *form, ...);

void kxcore_set_global_data(KxCore *core, char *key, void *data);
void* kxcore_get_global_data(KxCore *core, char *key);
void kxcore_remove_global_data(KxCore *core, char *key);

void kxcore_register_mark_function(KxCore *core, KxMarkFunction *function);

void kxcore_push_local_import_path(KxCore *core, char *path);
void kxcore_pop_local_import_path(KxCore *core);
char * kxcore_top_local_import_path(KxCore *core);


KxObject * kxcore_raw_object_get(KxCore *core);
void kxcore_raw_object_return(KxCore *core, KxObject *object);


void kxcore_slot_cache_put(KxCore *core, int size, void *mem);
void * kxcore_slot_cache_get(KxCore *core, int size);
void * kxcore_slot_cache_resize(KxCore *core, int old_size, int new_size, void *mem);

void kxcore_raw_activation_return(KxCore *core, void *activation);
void * kxcore_raw_activation_get(KxCore *core);
List * kxcore_list_with_all_objects(KxCore *core);

KxObject *kxcore_get_char(KxCore *self, unsigned char c);

KxObject *kxcore_get_integer(KxCore *core, int integer);

KxStack *kxcore_create_and_switch_to_stack(KxCore *core);
void kxcore_free_actual_and_switch_to_stack(KxCore *core, KxStack *stack);


#ifdef KX_THREADS_SUPPORT
	void kxcore_reset_yield_counter(KxCore *core);
#endif // KX_THREADS_SUPPORT

#endif // __KXCORE_H
