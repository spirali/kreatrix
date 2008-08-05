/*
   kxactivation.h
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
	
#ifndef __KXACTIVATION_H
#define __KXACTIVATION_H

#include "kxcodeblock.h"
#include "kxmessage.h"
#include "kxconfig.h"

/*#define IS_KXACTIVATION(kxobject) ((kxobject)->extension==&kxactivation_extension)

#define KXACTIVATION_DATA(kxactivation) ((KxActivationData*) kxactivation->data.ptr)
*/
typedef struct KxActivation KxActivation;


//typedef struct KxActivationData KxActivationData;

#define KXACTIVATION_INNER_STACK_SIZE 16

struct KxActivation {
	KxObject **locals;
	KxCodeBlock *codeblock;

	KxObject *receiver; /* Reference to receiver */

	KxActivation *parent;

	KxMessage message;

	char *codepointer;

	int inner_stack_pos;
	KxObject *inner_stack[KXACTIVATION_INNER_STACK_SIZE];

	int is_over; // activation is dead, running of method is over and activation isn't on stack

	KxObject *slot_holder_of_codeblock;

	#ifdef KX_MULTI_STATE
		KxCore *core;
	#endif

	int ref_count;
};

void kxactivation_mark(KxActivation *activation);
void kxactivation_free(KxActivation *activation);

int kxactivation_put_local(KxActivation *self, KxSymbol *name, KxObject *newobject);
KxObject *kxactivation_get_local(KxActivation *self, KxSymbol *name);




KxActivation * kxactivation_new(KxCore *core);
//void kxactivation_init_extension();

//KxObject *kxactivation_new_prototype(KxCore *core);

//KxActivation *kxactivation_clone_with_new_data(KxActivation *self);

KxObject * kxactivation_run(KxActivation *activation);

//void kxactivation_post_init(KxActivation *self, KxCore *core);



//extern KxObjectExtension kxactivation_extension;

#endif
