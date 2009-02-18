/*
   kxstack.h
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
	
#ifndef __STACK_H
#define __STACK_H

#define KXSTACK_ACTIVATIONS_SIZE 4096
#define KXSTACK_MESSAGES_SIZE 6000

#include "kxobject_struct.h"
#include "../kxconfig.h"

struct KxActionation;

typedef struct KxStack KxStack;

//typedef struct KxCFunction KxCFunction;

struct KxStack {
	int messages_count;
	struct KxMessage *messages[KXSTACK_MESSAGES_SIZE];

	int activations_count;
	struct KxActivation *activations[KXSTACK_ACTIVATIONS_SIZE];

	int objects_count;
	struct KxObject *objects[KXSTACK_ACTIVATIONS_SIZE];

	
	KxReturn return_state;
	KxObject *return_object;
	struct KxActivation *long_return_to_activation;
	KxObject *running_cfunction;

	List *throw_trace;

	#ifdef KX_THREADS_SUPPORT
	KxObject *thread;
	#endif // KX_THREADS_SUPPORT


};

KxStack * kxstack_new();

void kxstack_free(KxStack *self);

void kxstack_mark(KxStack *self);


KxReturn kxstack_push_activation(KxStack *self, struct KxActivation *activation);
void kxstack_pop_activation(KxStack *self);

KxReturn kxstack_push_object(KxStack *self, KxObject *object);
void kxstack_pop_object(KxStack *self);


void kxstack_throw_object(KxStack *self, KxObject *object);

KxObject * kxstack_catch_thrown_object(KxStack *self);

// Message name must be symbol
void kxstack_throw_trace( KxStack *self, char *source_filename, int lineno, 
	char *object_type, KxObject *message_name );

void kxstack_print_trace(KxStack *self);

KxObject *kxstack_get_thrown_object(KxStack *self);

void kxstack_push_message (KxStack *self, struct KxMessage *message);
void kxstack_drop_message (KxStack *self);

KxReturn kxstack_get_return_state(KxStack *self);
void kxstack_set_return_state(KxStack *self, KxReturn state);
KxObject * kxstack_get_method_return_object(KxStack *self);

KxObject *kxstack_get_and_reset_return_object(KxStack *self);
void kxstack_set_return_object(KxStack *self, KxObject *object);
void kxstack_dump_messages(KxStack *self);

void kxstack_set_long_return_to_activation(KxStack *stack, struct KxActivation *activation);

struct KxActivation *kxstack_get_long_return_to_activation(KxStack *stack);

void kxstack_dump_throw(KxStack *self);
void kxstack_dump_messages(KxStack *self);


#ifdef KX_THREADS_SUPPORT
void kxstack_set_thread(KxStack *stack, KxObject *thread);
#endif // KX_THREADS_SUPPORT

#endif
