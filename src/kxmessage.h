/*
   kxmessage.h
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
	
#ifndef __KXMESSAGE_H
#define __KXMESSAGE_H

typedef struct KxMessage KxMessage;

#include "kxobject_struct.h"
#include "kxsymbol.h"

#define KXMESSAGE_PARAMS_SIZE 8

struct KxMessage {
	KxObject *target;
	KxSymbol *message_name;
	int params_count;
	KxObject *slot_holder;
	KxObject *params[KXMESSAGE_PARAMS_SIZE];
};

KxObject * kxmessage_send(KxMessage *self);
KxObject * kxmessage_resend(KxMessage *self, KxObject *start_of_lookup);


KxObject * kxmessage_send_no_hook_and_exception(KxMessage *self, int *slot_not_found);

KxMessage *kxmessage_new(KxSymbol *message_name, KxObject *target, KxObject *slot_holder);

void kxmessage_init(KxMessage *self, KxObject *target, int params_count, KxSymbol * message_name);

void kxmessage_free(KxMessage *self);

void kxmessage_mark(KxMessage *self);

void kxmessage_dump(KxMessage *self);

#endif
