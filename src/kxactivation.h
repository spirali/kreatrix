/**********************************************************************
 *
 *  kxactivation.h - Activation of method or block
 *
 *	$Id$
 *
 *  KxActivation represents activation of KxCodeBlock 
 *  and inteprets bytecode from KxCodeBlock.
 *
 **********************************************************************/

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
