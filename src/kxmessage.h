/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


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

KxMessage *kxmessage_new(KxSymbol *message_name, KxObject *slot_holder);

void kxmessage_init(KxMessage *self, KxObject *target, int params_count, KxSymbol * message_name);

void kxmessage_free(KxMessage *self);

void kxmessage_mark(KxMessage *self);

void kxmessage_dump(KxMessage *self);

#endif
