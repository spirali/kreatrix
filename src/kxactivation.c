/**********************************************************************
 *
 *  kxactivation.c - Activation of method block
 *
 *	$Id$
 *
 *  Implementation of KxActvation
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "kxactivation.h"
#include "kxobject.h"
#include "compiler/kxinstr.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxstack.h"
#include "kxtypes.h"
#include "kxgc.h"
#include "kxscopedblock.h"
#include "kxlist.h"
#include "kxfloat.h"
#include "kxexception.h"
#include "kxcharacter.h"
#include "kxactivation_object.h"
#include "kxiterator.h"

#define FETCH_BYTE(codep) *(codep++)

KxObjectExtension kxactivation_extension;

typedef KxReturn (KxActivationInstrFcn)(KxActivation *self, char **codep);

void kxactivation_free(KxActivation *self);
void kxactivation_mark(KxActivation *self);

static void kxactivation_inner_stack_free(KxActivation *self);


KxActivationInstrFcn *(instr_fcn)[KXCI_INSTRUCTIONS_COUNT];


void
kxactivation_free(KxActivation *self)
{
	kxactivation_inner_stack_free(self);

	REF_REMOVE(self->receiver);

	KxCodeBlockData *cdata = self->codeblock->data.ptr;
	int t;
	for (t=0; t < cdata->locals_count; t++) 
	{
		REF_REMOVE(self->locals[t]);
	}

	if (cdata->prealocated_locals) {
		kxfree(self->locals);
	} else {
		cdata->prealocated_locals = self->locals;
	}
	
	REF_REMOVE(self->codeblock);
	REF_REMOVE(self->slot_holder_of_codeblock);

	kxcore_raw_activation_return(KXCORE,self);

}

void
kxactivation_mark(KxActivation *self)
{

	kxobject_mark(self->codeblock);
	kxobject_mark(self->receiver);
	kxobject_mark(self->slot_holder_of_codeblock);

	int t;
	for (t=0;t<self->inner_stack_pos;t++) {
		kxobject_mark(self->inner_stack[t]);
	}

	KxCodeBlockData *cdata = KXCODEBLOCK_DATA(self->codeblock);
	for (t=0;t<cdata->locals_count;t++) {
		kxobject_mark(self->locals[t]);
	}
}

static int
kxactivation_get_line_index(KxActivation *self) 
{
	KxCodeBlockData *cdata = KXCODEBLOCK_DATA(self->codeblock);

	char *instr = cdata->code;

	int line_index = 0;

	while(instr < self->codepointer) {
		KxInstructionType i = *instr;

		if (kxcinstruction_has_linenumber(i)) {
			line_index++;
		}

		instr += 1 + kxinstructions_info[i].params_count;
	}
	return line_index - 1;
}

KxReturn 
kxactivation_process_throw(KxActivation *self)
{

	KxCodeBlockData *cdata = self->codeblock->data.ptr;
	if (self->message.target) {
		KxStack *original_stack = kxcore_create_and_switch_to_stack(KXCORE);

		KxObject *type = kxobject_type_name(self->message.target);
		char *type_name;
		if (IS_KXSTRING(type))
			type_name = KXSTRING_VALUE(type);
		else
			type_name = "<not string>";
		
		int lineno = cdata->message_linenumbers[kxactivation_get_line_index(self)];

		kxcore_free_actual_and_switch_to_stack(KXCORE, original_stack);

		kxstack_throw_trace(KXSTACK,cdata->source_filename, lineno,type_name, self->message.message_name);
		REF_REMOVE(type);
		REF_REMOVE(self->message.target);
	}
	kxstack_pop_activation(KXSTACK);

	return RET_THROW;

}

/** ! INNER STACK DON'T MODIFY REFERENCE COUNT ! */
 
static void
kxactivation_inner_stack_push(KxActivation *self, KxObject *object) 
{
	if (self->inner_stack_pos == KXACTIVATION_INNER_STACK_SIZE) {
		fprintf(stderr,"Inner stack push: Stack is full\n");
		abort();
	}
	self->inner_stack[ self->inner_stack_pos++ ] = object;
}

static KxObject *
kxactivation_inner_stack_pop(KxActivation *self) 
{
	if (self->inner_stack_pos == 0) {
		KxCodeBlockData *cdata = self->codeblock->data.ptr;
		fprintf(stderr,"Inner stack pop: Stack is empty (%s:%i)\n", 
			cdata->source_filename, 
			cdata->message_linenumbers[kxactivation_get_line_index(self)]);
		abort();
	}
	
	return self->inner_stack[ --self->inner_stack_pos ];
}

/** Drop second item on stack */
static inline void
kxactivation_inner_stack_drop2(KxActivation *self) 
{
	--self->inner_stack_pos;
	self->inner_stack[ self->inner_stack_pos - 1 ] = self->inner_stack [ self->inner_stack_pos ];
}


/*static void
kxactivation_inner_stack_dump(KxActivation *self) 
{
	printf("Inner stack dump:\n");
	int t;
	for (t = 0; t < self->inner_stack_pos; t++) {
		kxobject_dump(self->inner_stack[t]);
	}
}*/

static void
kxactivation_inner_stack_free(KxActivation *self) 
{
/*	// DEBUG 
	if (self->inner_stack_pos)
		printf("Error STACK = %i\n",  self->inner_stack_pos);*/
	int t;
	for (t=0;t<self->inner_stack_pos;t++) {
		REF_REMOVE(self->inner_stack[t]);
	}
	self->inner_stack_pos = 0;
}


KxActivation *kxactivation_new(KxCore *core) 
{
	KxActivation *self = kxcore_raw_activation_get(core);
	ALLOCTEST(self);
	self->is_over = 0;

#ifdef KX_MULTI_STATE
	self->core = core;
#endif

	self->ref_count = 1;
	return self;
}

// You must specify message.start_search if target == NULL
static inline void 
kxactivation_message_prepare_from_inner_stack(KxActivation *self, KxObject *target, int params_count, KxSymbol *message_name) 
{
	REF_ADD(message_name);
	self->message.message_name = message_name;
	self->message.params_count = params_count;

	int t;
	for (t=params_count-1; t>=0;t--) {
		self->message.params[t] = kxactivation_inner_stack_pop(self);
	}

	if (target) {
		self->message.target = target;
		REF_ADD2(self->message.target);
	} else {
		self->message.target = kxactivation_inner_stack_pop(self);
		REF_ADD(self->message.target);
	}
}

static inline KxObject *
kxactivation_return_object(KxActivation *self, KxObject *obj)
{
	self->is_over = 1;
	kxstack_pop_activation(KXSTACK);
	kxactivation_inner_stack_free(self);

	if (self->message.target) 
		REF_REMOVE(self->message.target);
	
	return obj;
}

static inline KxObject *
kxactivation_ret_longreturn(KxActivation *self)
{
	self->is_over = 1;
	kxstack_pop_activation(KXSTACK);

	kxactivation_inner_stack_free(self);

	if (self->message.target)
		REF_REMOVE(self->message.target);

	KxActivation *longret = kxstack_get_long_return_to_activation(KXSTACK);
	if (longret == self) {
		return kxstack_get_and_reset_return_object(KXSTACK);
	} else {
		return NULL;
	}
}

static inline KxObject * 
kxactivation_return(KxActivation *self, KxReturn ret)
{
	self->is_over = 1;
	switch(ret) {
		case RET_LONGRETURN: 
			return kxactivation_ret_longreturn(self);
		case RET_THROW: {
			kxactivation_process_throw(self);
			kxactivation_inner_stack_free(self);
			return NULL;
		} break;

		case RET_EXIT: {
			return kxactivation_return_object(self, NULL);
		}

		default:
			fprintf(stderr,"Invalid return code %i\n", ret);
			exit(-1);
			return NULL;
	}
}

static KxObject * 
kxactivation_send_standby_message_with_codeblock(KxActivation *self, KxObject *receiver, int dictionary_item, int codeblock) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);

	KxCodeBlock **subblocks = KXCODEBLOCK_DATA(self->codeblock)->subcodeblocks;
	KxScopedBlock *scopedblock = kxscopedblock_new(KXCORE, subblocks[codeblock], self);

	KxMessage *msg = &self->message;
	REF_ADD(receiver);
	kxmessage_init(msg, receiver, 1, KXCORE->dictionary[dictionary_item]);
	msg->params[0] = scopedblock;

	KxObject *result = kxmessage_send(msg);
	
	REF_REMOVE(receiver);

	return result;
}

static KxObject * 
kxactivation_send_standby_message_with_params_and_codeblock
	(KxActivation *self, KxObject *receiver, int dictionary_item, KxObject **params, int codeblock) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);


	KxMessage *msg = &self->message;
	REF_ADD(receiver);
	int params_count = 0;

	KxObject **p = params;
	do {
		msg->params[params_count++] = *p;
		++p;
	} while(*p);

	KxCodeBlock **subblocks = KXCODEBLOCK_DATA(self->codeblock)->subcodeblocks;
	KxScopedBlock *scopedblock = kxscopedblock_new(KXCORE, subblocks[codeblock], self);
	msg->params[params_count] = scopedblock;

	kxmessage_init(msg, receiver, params_count + 1, KXCORE->dictionary[dictionary_item]);

	KxObject *result = kxmessage_send(msg);
	
	/*do {
		REF_REMOVE(*params);
		++params;
	} while(*params);*/

	REF_REMOVE(receiver);

	return result;
}

static KxObject * 
kxactivation_send_standby_message_with_two_codeblocks(KxActivation *self, KxObject *receiver, int dictionary_item, int codeblock1, int codeblock2) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);

	KxCodeBlock **subblocks = KXCODEBLOCK_DATA(self->codeblock)->subcodeblocks;
	KxScopedBlock *scopedblock1 = kxscopedblock_new(KXCORE, subblocks[codeblock1], self);
	KxScopedBlock *scopedblock2 = kxscopedblock_new(KXCORE, subblocks[codeblock2], self);

	KxMessage *msg = &self->message;
	REF_ADD(receiver);
	kxmessage_init(msg, receiver, 2, KXCORE->dictionary[dictionary_item]);
	msg->params[0] = scopedblock1;
	msg->params[1] = scopedblock2;

	KxObject *result = kxmessage_send(msg);
	
	REF_REMOVE(receiver);

	return result;
}


KxObject * 
kxactivation_run(KxActivation *self) 
{
	// Init inner stack
	self->inner_stack_pos = 0;

	KxStack *stack = KXSTACK;
	
	self->message.target = NULL;

	kxstack_push_activation(stack,self);
	
	kxcore_gc_check(KXCORE);	


	KxCodeBlockData *cdata = KXCODEBLOCK_DATA(self->codeblock);
	char *codep = cdata->code;
	KxObject **symbol_frame = cdata->symbol_frame;
	
	//printf("-=BEGIN=-\n");
	for(;;) {
		char instruction = *(codep++);
	//	printf("INSTR = %i\n",instruction);
	/*	kxstack_dump_messages(stack);*/
		//KxActivationInstrFcn *fcn = (instr_fcn[(int)instruction]);

		switch(instruction) {
			case KXCI_PUSH_LITERAL: {
				KxObject *obj = cdata->literals[(int)FETCH_BYTE(codep)];
				REF_ADD(obj);
				kxactivation_inner_stack_push(self, obj);
				continue;
			}

			case KXCI_POP:
			{
				KxObject *object = kxactivation_inner_stack_pop(self);
				REF_REMOVE(object);
				continue;
			}

			case KXCI_RETURN_SELF:
			{
				KxObject *self_obj = self->receiver;
				REF_ADD(self_obj);
				return kxactivation_return_object(self, self_obj);
			}

			case KXCI_RETURN_STACK_TOP:
			{
				return kxactivation_return_object(self, kxactivation_inner_stack_pop(self));
			}
			
			case KXCI_LONGRETURN:
			{
				KxActivation *long_return = self->parent;

				while(long_return->parent) {
					long_return = long_return->parent;
				}

				if (long_return->is_over) {
					KxException *exception = kxexception_new_with_text(KXCORE,"Blok with '^' evaluated when scope isn't running");
					kxstack_throw_object(KXSTACK,exception);
					return kxactivation_return(self, RET_THROW);
				}

				KxObject *obj = kxactivation_inner_stack_pop(self);
				KxStack *stack = KXSTACK;
				kxstack_set_long_return_to_activation(stack, long_return);
				kxstack_set_return_object(stack,obj);
				kxstack_set_return_state(stack, RET_LONGRETURN);
				return kxactivation_return_object(self, NULL);
			}
			
			case KXCI_UNARY_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				kxactivation_message_prepare_from_inner_stack(self, NULL, 0, symbol_frame[(int)(*((codep)++))]);
				

				KxObject *obj = kxmessage_send(&self->message);
				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}

			case KXCI_BINARY_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				kxactivation_message_prepare_from_inner_stack(self,NULL, 1,symbol_frame[(int)(*((codep)++))]);

				KxObject *obj = kxmessage_send(&self->message);

				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else  {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}

			case KXCI_LOCAL_UNARY_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				self->message.message_name = symbol_frame[(int)(*((codep)++))];
				REF_ADD(self->message.message_name);

				self->message.params_count = 0;
				self->message.target = self->receiver;

				REF_ADD2(self->receiver);
				
				KxObject *obj =  kxmessage_send(&self->message);
				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else  {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}

			}

			case KXCI_RESEND_UNARY_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				self->message.message_name = symbol_frame[(int)(*((codep)++))];
				REF_ADD(self->message.message_name);

				self->message.params_count = 0;

				self->message.target = self->receiver;
				REF_ADD2(self->message.target);
				KxObject *obj = kxmessage_resend(&self->message, self->slot_holder_of_codeblock);

				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}

			case KXCI_KEYWORD_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				KxSymbol *symbol = symbol_frame[(int)(*((codep)++))];

				int params = (int)(*((codep)++));
				kxactivation_message_prepare_from_inner_stack(self,NULL, params,symbol);

				KxObject *obj = kxmessage_send(&self->message);

				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}

			case KXCI_RESEND_KEYWORD_MSG:
			{
				/** TODO: remove this condition */
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				KxSymbol *symbol = symbol_frame[(int)(*((codep)++))];

				int params = (int)(*((codep)++));
				kxactivation_message_prepare_from_inner_stack(self,self->receiver, params,symbol);

				KxObject *obj = kxmessage_resend(&self->message, self->slot_holder_of_codeblock);

				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}
		
			case KXCI_LOCAL_KEYWORD_MSG:
			{
				if (self->message.target)
					REF_REMOVE(self->message.target);

				self->codepointer = codep;

				KxSymbol *symbol = symbol_frame[(int)(*((codep)++))];
				int params = (int)(*((codep)++));

				kxactivation_message_prepare_from_inner_stack(self,self->receiver,params,symbol);

				KxObject *obj = kxmessage_send(&self->message);

				if (obj) {
					kxactivation_inner_stack_push(self, obj);
					continue;
				} else {
					return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
				}
			}
			
			case KXCI_PUSH_METHOD:
			{
				KxCodeBlock **subblocks = KXCODEBLOCK_DATA(self->codeblock)->subcodeblocks;
				KxCodeBlock *method = subblocks[(int)(*((codep)++))];
				REF_ADD(method);
				kxactivation_inner_stack_push(self, method);
				continue;
			}

			case KXCI_PUSH_BLOCK:
			{
				KxCodeBlock **subblocks = KXCODEBLOCK_DATA(self->codeblock)->subcodeblocks;
				KxCodeBlock *block = subblocks[(int)(*((codep)++))];

				KxScopedBlock *scopedblock = kxscopedblock_new(KXCORE, block, self);
				kxactivation_inner_stack_push(self, scopedblock);
				continue;
			}
			
			case KXCI_LIST:
			{
				int size = *((char*)codep);
				codep += sizeof(char);

				List *list = list_new_size(size);
				int t;

				for (t=size-1;t>=0;--t) {
					list->items[t] = kxactivation_inner_stack_pop(self);
				}

				KxList *kxlist = kxobject_raw_clone(kxcore_get_basic_prototype(KXCORE, KXPROTO_LIST));
				kxlist->data.ptr = list;

				kxactivation_inner_stack_push(self, kxlist);
				continue;
			}

			case KXCI_PUSH_LOCAL:
			{
				KxObject *obj = self->locals[(int)(*((codep)++))];
				REF_ADD(obj);
				kxactivation_inner_stack_push(self, obj);
				continue;

			}

			case KXCI_PUSH_SELF:
			{
				KxObject *obj = self->receiver;
				REF_ADD(obj);
				kxactivation_inner_stack_push(self, obj);
				continue;

			}

			case KXCI_PUSH_ACTIVATION:
			{
				KxObject *obj = kxactivationobject_new(KXCORE, self);
				kxactivation_inner_stack_push(self, obj);
				continue;

			}

			case KXCI_UPDATE_LOCAL:
			{
				KxObject *obj = kxactivation_inner_stack_pop(self);
				int pos = (int)(*((codep)++));
				REF_REMOVE(self->locals[pos]);
				self->locals[pos] = obj;
				continue;
			}

			case KXCI_PUSH_OUTER_LOCAL: 
			{
				int scope = (int) FETCH_BYTE(codep);
				KxActivation *act = self->parent;
				int t;
				for (t=0;t<scope;t++) {
					act = act->parent;
				}
				KxObject *obj = act->locals[(int)FETCH_BYTE(codep)];
				REF_ADD(obj);
				kxactivation_inner_stack_push(self, obj);
				continue;
			}

			case KXCI_UPDATE_OUTER_LOCAL:
			{
				int scope = (int) FETCH_BYTE(codep);
				KxActivation *act = self->parent;
				int t;
				for (t=0;t<scope;t++) {
					act = act->parent;
				}
				KxObject *obj = kxactivation_inner_stack_pop(self);
				int pos = (int)FETCH_BYTE(codep);
				REF_REMOVE(act->locals[pos]);
				act->locals[pos] = obj;
				continue;
			}

			/** Optimizing instructions **/

			case KXCI_IFTRUE:
			{
				int codeblock = (int) FETCH_BYTE(codep);
				int jump = (int) FETCH_BYTE(codep);
				KxObject *obj = kxactivation_inner_stack_pop(self);
				if (obj != KXCORE->object_true) { 
					if (obj != KXCORE->object_false) {
						self->codepointer = codep - 2;
						obj = kxactivation_send_standby_message_with_codeblock(self, obj, KXDICT_IFTRUE, codeblock);
						if (obj == NULL) {
							return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
						}

					}
					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				} else {
					REF_REMOVE(obj);
					continue;
				}
			}

			case KXCI_IFFALSE:
			{
				int codeblock = (int) FETCH_BYTE(codep);
				int jump = (int) FETCH_BYTE(codep);
				KxObject *obj = kxactivation_inner_stack_pop(self);
				if (obj != KXCORE->object_false) { 
					if (obj != KXCORE->object_true) {
						self->codepointer = codep - 2;
						obj = kxactivation_send_standby_message_with_codeblock(self, obj, KXDICT_IFFALSE, codeblock);
						if (obj == NULL) {
							return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
						}

					}
					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				} else {
					REF_REMOVE(obj);
					continue;
				}
			}

			case KXCI_IFTRUE_IFFALSE:
			{
				int codeblock = (int) FETCH_BYTE(codep);
				int jump1 = (int) FETCH_BYTE(codep);
				int jump2 = (int) FETCH_BYTE(codep);
				KxObject *obj = kxactivation_inner_stack_pop(self);
				if (obj != KXCORE->object_true) { 
					if (obj != KXCORE->object_false) {
						self->codepointer = codep - 3;
						obj = kxactivation_send_standby_message_with_two_codeblocks
							(self, obj, KXDICT_IFTRUE_IFFALSE, codeblock, codeblock + 1);

						if (obj == NULL) {
							return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
						}

						kxactivation_inner_stack_push(self, obj);
						codep += jump2;
						continue;
					}
					REF_REMOVE(obj);
					codep += jump1;
					continue;
				} else {
					REF_REMOVE(obj);
					continue;
				}
			}

			case KXCI_IFFALSE_IFTRUE:
			{
				int codeblock = (int) FETCH_BYTE(codep);
				int jump1 = (int) FETCH_BYTE(codep);
				int jump2 = (int) FETCH_BYTE(codep);
				KxObject *obj = kxactivation_inner_stack_pop(self);
				if (obj != KXCORE->object_false) { 
					if (obj != KXCORE->object_true) {
						self->codepointer = codep - 3;
						obj = kxactivation_send_standby_message_with_two_codeblocks
							(self, obj, KXDICT_IFFALSE_IFTRUE, codeblock, codeblock + 1);

						if (obj == NULL) {
							return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
						}

						kxactivation_inner_stack_push(self, obj);
						codep += jump2;
						continue;
					}
					REF_REMOVE(obj);
					codep += jump1;
					continue;
				} else {
					REF_REMOVE(obj);
					continue;
				}
			}


			case KXCI_JUMP:
			{
				int jump = (int) FETCH_BYTE(codep);
				codep += jump;
				continue;
			}

			case KXCI_FOREACH:
			{
				int jump = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);
				int codeblock = (int) FETCH_BYTE(codep);

				KxObject *obj = kxactivation_inner_stack_pop(self);

				if (obj->extension && obj->extension->iterator_create) {
					KxObject *iterator = obj->extension->iterator_create(obj);
					KxObject *each = obj->extension->iterator_next(obj, iterator->data.ptr);
					REF_REMOVE(obj);
					if (each == NULL) {
						codep += jump;
						REF_REMOVE(iterator);
						REF_ADD(KXCORE->object_nil);
						kxactivation_inner_stack_push(self, KXCORE->object_nil);
						continue;
					}
					kxactivation_inner_stack_push(self, iterator);
					
					REF_REMOVE(self->locals[local]);
					self->locals[local] = each;
					REF_ADD(each);
					continue;
				} else {
					self->codepointer = codep - 3;
					obj = kxactivation_send_standby_message_with_codeblock(self, obj, KXDICT_FOREACH, codeblock);
					if (obj == NULL) {
						return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
					}
					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				}
			}

			case KXCI_NEXTITER:
			{
				int jump = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);

				KxObject *iterator = self->inner_stack[self->inner_stack_pos - 2];

				KxIteratorData *data = KXITERATOR_DATA(iterator);
				KxObject *obj = data->object;


				KxObject *each = obj->extension->iterator_next(obj, data);
				if (each) {
					REF_REMOVE(self->locals[local]);
					self->locals[local] = each;
					REF_ADD(each);
					codep += jump;
	
					obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);

					continue;
				} else {
					REF_REMOVE(iterator);
					kxactivation_inner_stack_drop2(self);
					continue;
				}
			}

			case KXCI_REPEAT:
			{
				int jump = (int) FETCH_BYTE(codep);
				int codeblock = (int) FETCH_BYTE(codep);
				KxObject *obj = kxactivation_inner_stack_pop(self);
				if (IS_KXINTEGER(obj)) {
					long val = KXINTEGER_VALUE(obj);
					REF_REMOVE(obj);

					if (val == 0) {
						codep += jump;
						REF_ADD(KXCORE->object_nil);
						kxactivation_inner_stack_push(self, KXCORE->object_nil);
						continue;
					}

					kxactivation_inner_stack_push(self, KXINTEGER(val - 1));
					continue;
				} else {
					self->codepointer = codep - 2;
					obj = kxactivation_send_standby_message_with_codeblock(self, obj, KXDICT_REPEAT, codeblock);
					if (obj == NULL) {
						return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
					}
					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				}
			}

			case KXCI_REPEAT_END: 
			{
				int jump = (int) FETCH_BYTE(codep);
				
				KxObject *num = self->inner_stack[self->inner_stack_pos - 2];
				long val = KXINTEGER_VALUE(num);
				if (val == 0) {
					REF_REMOVE(num);
					kxactivation_inner_stack_drop2(self);
					continue;
				} else {
					KxObject *obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);
					obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);
					kxactivation_inner_stack_push(self, KXINTEGER(val - 1));
					codep += jump;
					continue;
				}
			}

			case KXCI_TODO:
			{
				int jump = (int) FETCH_BYTE(codep);
				int codeblock = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);
				KxObject *to = kxactivation_inner_stack_pop(self);
				KxObject *from = kxactivation_inner_stack_pop(self);

				if (IS_KXINTEGER(from) && IS_KXINTEGER(to)) {
					long from_val = KXINTEGER_VALUE(from);
					long to_val = KXINTEGER_VALUE(to);
					if (from_val >= to_val) {
						codep += jump;
						REF_REMOVE(from);
						REF_REMOVE(to);
						REF_ADD(KXCORE->object_nil);
						kxactivation_inner_stack_push(self, KXCORE->object_nil);
						continue;
					}
					kxactivation_inner_stack_push(self, to);
					kxactivation_inner_stack_push(self, from);
					
					REF_REMOVE(self->locals[local]);
					self->locals[local] = from;
					REF_ADD(from);
					continue;
				} else {
					self->codepointer = codep - 3;
					KxObject *params[2];
					params[0] = to;
					params[1] = NULL;
					KxObject *obj;
					obj = kxactivation_send_standby_message_with_params_and_codeblock
						(self, from, KXDICT_TODO, params, codeblock);

					if (obj == NULL) {
						return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
					}

					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				}
			}

			case KXCI_TODO_END: 
			{
				int jump = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);
				
				KxObject *num = self->inner_stack[self->inner_stack_pos - 2];
				KxObject *to = self->inner_stack[self->inner_stack_pos - 3];
				long val = KXINTEGER_VALUE(num) + 1;
				long to_val = KXINTEGER_VALUE(to);
				if (val >= to_val) {
					REF_REMOVE(num);
					REF_REMOVE(to);
					kxactivation_inner_stack_drop2(self);
					kxactivation_inner_stack_drop2(self);
					continue;
				} else {
					codep += jump;

					KxObject *obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);
					obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);

					obj = KXINTEGER(val);
					kxactivation_inner_stack_push(self, obj);
					
					REF_REMOVE(self->locals[local]);
					self->locals[local] = obj;
					REF_ADD(obj);
					continue;
				}

			}

			case KXCI_TOBYDO:
			{
				int jump = (int) FETCH_BYTE(codep);
				int codeblock = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);
				KxObject *delta = kxactivation_inner_stack_pop(self);
				KxObject *to = kxactivation_inner_stack_pop(self);
				KxObject *from = kxactivation_inner_stack_pop(self);

				if (IS_KXINTEGER(from) && IS_KXINTEGER(to) && IS_KXINTEGER(delta)) {
					long from_val = KXINTEGER_VALUE(from);
					long to_val = KXINTEGER_VALUE(to);
					long delta_val = KXINTEGER_VALUE(delta);
					if ((delta_val > 0 && from_val >= to_val) || (delta_val < 0 && from_val <= to_val)) {
						codep += jump;
						REF_REMOVE(from);
						REF_REMOVE(to);
						REF_REMOVE(delta);
						REF_ADD(KXCORE->object_nil);
						kxactivation_inner_stack_push(self, KXCORE->object_nil);
						continue;
					}
					kxactivation_inner_stack_push(self, to);
					kxactivation_inner_stack_push(self, delta);
					kxactivation_inner_stack_push(self, from);

					REF_REMOVE(self->locals[local]);
					self->locals[local] = from;
					REF_ADD(from);
					continue;
				} else {
					self->codepointer = codep - 3;
					KxObject *params[3];
					params[0] = to;
					params[1] = delta;
					params[2] = NULL;
					KxObject *obj;
					obj = kxactivation_send_standby_message_with_params_and_codeblock
						(self, from, KXDICT_TOBYDO, params, codeblock);

					if (obj == NULL) {
						return kxactivation_return(self, kxstack_get_return_state(KXSTACK));
					}

					kxactivation_inner_stack_push(self, obj);
					codep += jump;
					continue;
				}
			}

			case KXCI_TOBYDO_END: 
			{
				int jump = (int) FETCH_BYTE(codep);
				int local = (int) FETCH_BYTE(codep);
				
				KxObject *num = self->inner_stack[self->inner_stack_pos - 2];
				KxObject *delta = self->inner_stack[self->inner_stack_pos - 3];
				KxObject *to = self->inner_stack[self->inner_stack_pos - 4];
				long delta_val = KXINTEGER_VALUE(delta);
				long val = KXINTEGER_VALUE(num) + delta_val;
				long to_val = KXINTEGER_VALUE(to);
				if ((delta_val > 0 && val >= to_val) || (delta_val < 0 && val <= to_val)) {
					REF_REMOVE(num);
					REF_REMOVE(to);
					REF_REMOVE(delta);
					kxactivation_inner_stack_drop2(self);
					kxactivation_inner_stack_drop2(self);
					kxactivation_inner_stack_drop2(self);
					continue;
				} else {
					codep += jump;

					KxObject *obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);
					obj = kxactivation_inner_stack_pop(self);
					REF_REMOVE(obj);

					obj = KXINTEGER(val);
					kxactivation_inner_stack_push(self, obj);
					
					REF_REMOVE(self->locals[local]);
					self->locals[local] = obj;
					REF_ADD(obj);

					continue;
				}

			}


			default: 
	/*		{
				KxInstructionInfo *ii = &kxinstructions_info[(int)instruction];
				fprintf(stderr,"Invalid instruction %s\n", ii->name);
			}*/
				fprintf(stderr,"Invalid instruction\n");
				abort();
		}
		
		
	}
}

KxObject *
kxactivation_get_local(KxActivation *self, KxSymbol *name) 
{
	KxCodeBlockData *cdata = KXCODEBLOCK_DATA(self->codeblock);
	int t;
	for (t=0;t<cdata->locals_count;t++) {
		if (name == cdata->locals_symbols[t]) {
			KxObject *obj = self->locals[t];
			REF_ADD(obj);
			return obj;
		}
	}
	return NULL;
}


int
kxactivation_put_local(KxActivation *self, KxSymbol *name, KxObject *newobject) 
{
	KxCodeBlockData *cdata = KXCODEBLOCK_DATA(self->codeblock);
	int t;
	for (t=0;t<cdata->locals_count;t++) {
		if (name == cdata->locals_symbols[t]) {
			REF_ADD(newobject);
			KxObject *obj = self->locals[t];
			self->locals[t] = newobject;
			REF_REMOVE(obj);
			return 1;
		}
	}
	return 0;
}
