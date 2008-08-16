/*
   kxobject.h
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
	
#ifndef __KXOBJECT_H
#define __KXOBJECT_H

#include "utils/hash.h"
#include "kxobject_struct.h"
#include "kxobject_slots.h"
#include "kxsymbol.h"
#include "kxcfunction.h"

#define REF_ADD(object) { (object)->ref_count++; }
#define REF_ADD2(object) { (object)->ref_count+=2; }

#define REF_REMOVE(object) {  if (--((object)->ref_count) == 0) kxobject_free(object); }

/*
#undef REF_ADD
#undef REF_REMOVE
#include <stdio.h>
#define REF_ADD(object) { (object)->ref_count++; printf("REF_ADD: %p_%s %i > %s %i (%p)\n",(object),kxobject_raw_type_name(object), (object)->ref_count ,__FILE__,__LINE__, (object)->data.ptr); }

#define REF_REMOVE(object) { if ((object)->ref_count>0) printf("REF_REMOVE: %p_%s %i > %s %i (%p)\n",(object),kxobject_raw_type_name(object), (object)->ref_count ,__FILE__,__LINE__, (object)->data.ptr); if (--((object)->ref_count) == 0) kxobject_free(object); }

*/

/**
	Following methods don't return referenced object
	(you need add own referece)
	
	kxcore_get_basic_prototype()
	kxobject_get_slot()
	kxobject_find_slot()
	kxexception_new_with_message()
*/

typedef struct KxMethodTable KxMethodTable;


struct KxMethodTable {
	char *message;
	int params_count;
	KxExternFunction *cfunction;
};

void kxobject_free(KxObject *self);

KxObject *kxobject_raw_clone(KxObject *self);

KxObject *kxobject_raw_copy(KxObject *self);

KxObject *kxobject_new_from(KxObject *self);


void kxobject_add_parent(KxObject *self, KxObject *parent);
void kxobject_remove_parent(KxObject *self, KxObject *parent);
void kxobject_insert_parent(KxObject *self, KxObject *parent);
void kxobject_set_slot(KxObject *self, KxSymbol *key, KxObject *value);
void kxobject_set_slot_no_ref(KxObject *self, KxSymbol *key, KxObject *value);
void kxobject_set_slot_no_ref2(KxObject *self, KxSymbol *key, KxObject *value);
void kxobject_set_slot_with_flags(KxObject *self, KxSymbol *key, KxObject *value, int flags);


void kxobject_free_slots(KxObject *self);
void kxobject_clean(KxObject *self);

KxObject *kxobject_get_slot(KxObject *self, KxSymbol *key);
KxObject *kxobject_get_slot_with_flags(KxObject *self, KxSymbol *key, int *flags);
KxObject *kxobject_find_slot(KxObject *self, KxSymbol *key); 
KxObject *kxobject_find_slot_and_holder(KxObject *self, KxSymbol *key, KxObject **slot_holder, int *flags);
KxObject *kxobject_find_slot_in_ancestors(KxObject *self, KxSymbol *key, KxObject **slot_holder, int *flags);


void kxobject_add_methods(KxObject *self, KxMethodTable *table);

char *kxobject_raw_type_name(KxObject *self);
KxObject *kxobject_type_name(KxObject *self);

void kxobject_set_parent(KxObject *self, KxObject *parent);
void kxobject_remove_all_parents(KxObject *self);

void kxobject_dump(KxObject *self);

void kxobject_mark(KxObject *self) ;

void kxobject_set_type(KxObject *self, char *type);


KxObject * kxobject_send_unary_message(KxObject *self, KxSymbol *message_name);


KxObject *kxobject_clone(KxObject *self);

KxObject * kxobject_activate(KxObject *self, KxObject *target, List *params_list);
KxObject * kxobject_activate_with_message(KxObject *self, KxMessage *message);

KxObject * kxobject_evaluate_block(KxObject *self, KxMessage *message);
KxObject * kxobject_evaluate_block_simple(KxObject *self);

int kxobject_is_kind_of(KxObject *self, KxObject *test);

int kxobject_update_slot(KxObject *self, KxSymbol *key, KxObject *value);

KxObject *kxobject_find_object_by_path(KxObject *self, char **path, int path_count);

KxObject *kxobject_secured_clone_object_by_path(KxObject *self, char **path, int path_count);

KxObject *kxobject_compare_exception(KxObject *self, KxObject *object);

int kxobject_get_hash(KxObject *self, unsigned long *hash);

int kxobject_update_slot_flags(KxObject *self, KxSymbol *key, int flags);

KxObject *kxobject_type_error(KxObject *self, KxObjectExtension *extension, int param_id);

KxObject *kxobject_need_boolean(KxObject *self);

int kxobject_check_type(KxObject *self, KxObjectExtension *extension);

KxObject *kxobject_send_message_init(KxObject *self);

void kxobject_set_as_prototype(KxObject *self);
void kxobject_set_as_singleton(KxObject *self);
void kxobject_set_as_noparent(KxObject *self);



/** Only for 'foreach' constructions */
void kxobject_ref_remove(void *self);
void kxobject_ref_add(void *self);

#endif // __KXOBJECT_H
