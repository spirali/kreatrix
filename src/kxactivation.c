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

//static void kxactivation_add_method_table(KxObject *self);


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
	REF_REMOVE(self->locals);
	REF_REMOVE(self->codeblock);
	REF_REMOVE(self->slot_holder_of_codeblock);

	if (self->message_name)
		REF_REMOVE(self->message_name);

	kxcore_raw_activation_return(self->core,self);

}

void
kxactivation_mark(KxActivation *self)
{
	if (self->message_name) {
		kxobject_mark(self->message_name);
	}
	kxobject_mark(self->codeblock);
	kxobject_mark(self->receiver);
	kxobject_mark(self->target);
	//if (data->slot_holder_of_codeblock)
	kxobject_mark(self->slot_holder_of_codeblock);

	int t;
	for (t=0;t<self->inner_stack_pos;t++) {
		kxobject_mark(self->inner_stack[t]);
	}


	// TODO: params
}

KxReturn 
kxactivation_process_throw(KxActivation *self)
{

	KxCodeBlockData *cdata = self->codeblock->data.ptr;
	if (self->message.target) {
		KxObject *type = kxobject_type_name(self->message.target);
		char *type_name;
		if (IS_KXSTRING(type))
			type_name = KXSTRING_VALUE(type);
		else
			type_name = "<not string>";
		
		int lineno = cdata->message_linenumbers[self->message_num-1];

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
		fprintf(stderr,"Inner stack pop: Stack is empty\n");
		abort();
	}
	
	return self->inner_stack[ --self->inner_stack_pos ];
}

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
	self->core = core;
	self->ref_count = 1;
	return self;
}


KxObject * 
kxactivation_run(KxActivation *self) 
{
	
	// Init inner stack
	self->inner_stack_pos = 0;

	self->message_num=0;

	KxStack *stack = KXSTACK;
	//self->message.stack = stack;
	
	self->message.target = NULL;

	kxstack_push_activation(stack,self);
	
//	kxcore_gc_countdown(KXCORE);
	kxcore_gc_check(KXCORE);	

	char *codep = KXCODEBLOCK_DATA(self->codeblock)->code;

	for(;;) {
		char instruction = *(codep++);
		if (instruction >= KXCI_INSTRUCTIONS_COUNT) {
			printf("Instruction out of range\n");
			exit(-1);
		}

	/*	printf("INSTR = %i\n",instruction);
		kxstack_dump_messages(stack);*/
		KxActivationInstrFcn *fcn = (instr_fcn[(int)instruction]);
		KxReturn ret = fcn(self,&codep);
		
		if (ret != RET_OK) {
			self->is_over = 1;
			switch(ret) {
				case RET_METHODRETURN: {
					kxstack_pop_activation(stack);
			
					kxactivation_inner_stack_free(self);

					if (self->message.target)
						REF_REMOVE(self->message.target);

					if (KXCODEBLOCK_DATA(self->codeblock)->type == KXCODEBLOCK_METHOD) {
						return kxstack_get_and_reset_return_object(stack);
					}
					else {
						return NULL;
					}
				} break;
				case RET_BLOCKRETURN: {
						if (self->message.target)
							REF_REMOVE(self->message.target);

						kxstack_pop_activation(stack);
						KxObject *obj = kxactivation_inner_stack_pop(self);
						kxactivation_inner_stack_free(self);
						return obj;
					} break;
				case RET_LONGRETURN: {
					kxstack_pop_activation(stack);
			
					kxactivation_inner_stack_free(self);

					if (self->message.target)
						REF_REMOVE(self->message.target);

					KxActivation *longret = kxstack_get_long_return_to_activation(stack);
					if (KXCODEBLOCK_DATA(self->codeblock)->type == KXCODEBLOCK_METHOD && longret == self) {
						return kxstack_get_and_reset_return_object(stack);
					}
					else {
						return NULL;
					}
				} break;


				case RET_THROW: {
					kxactivation_process_throw(self);

					kxactivation_inner_stack_free(self);
					return NULL;
				} break;
				case RET_EXIT: {
					kxactivation_inner_stack_free(self);
					if (self->message.target)
						REF_REMOVE(self->message.target);
					kxstack_pop_activation(stack);
					return NULL;
				}
				default:
					fprintf(stderr,"Null returned but invalid return state\n");
					exit(-1);
			}
		} 
		
	}
}

static KxReturn 
kxactivation_instr_push_integer(KxActivation *self, char **codep)
{
	int integer = *((int*)*codep);
	*codep += sizeof(int);
	kxactivation_inner_stack_push(self, KXINTEGER(integer));
	return RET_OK;
}

static KxReturn 
kxactivation_instr_push_character(KxActivation *self, char **codep)
{
	int integer = *((int*)*codep);
	*codep += sizeof(int);
	kxactivation_inner_stack_push(self, KXCHARACTER(integer));
	return RET_OK;
}


static KxReturn 
kxactivation_instr_push_float(KxActivation *self, char **codep)
{
	double floatval = *((double*)*codep);
	*codep += sizeof(double);
	kxactivation_inner_stack_push(self, KXFLOAT(floatval));
	return RET_OK;
}

static KxReturn 
kxactivation_instr_push_string(KxActivation *self, char **codep)
{
	char *str = *codep;
	*codep += strlen(str)+1;
	kxactivation_inner_stack_push(self, KXSTRING(str));
	return RET_OK;
}

static KxReturn
kxactivation_instr_push_symbol(KxActivation *self, char **codep)
{
	KxCodeBlock *codeblock = self->codeblock;
	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;
	KxSymbol *symbol = symbol_frame[(int)(*((*codep)++))];
	REF_ADD(symbol);
	kxactivation_inner_stack_push(self, symbol);
	return RET_OK;
}

static KxReturn 
kxactivation_instr_pop(KxActivation *self, char **codep)
{
	KxObject *object = kxactivation_inner_stack_pop(self);

	REF_REMOVE(object);
	return RET_OK;
}

static KxReturn 
kxactivation_instr_returnself(KxActivation *self, char **codep)
{
	KxObject *self_obj = self->receiver;
	REF_ADD(self_obj);
	KxStack *stack = KXSTACK;
	kxstack_set_return_object(stack,self_obj);
	kxstack_set_return_state(stack, RET_METHODRETURN);

	return RET_METHODRETURN;
}

static KxReturn
kxactivation_instr_return(KxActivation *self, char **codep)
{
	KxObject *obj = kxactivation_inner_stack_pop(self);
	KxStack *stack = KXSTACK;
	kxstack_set_return_object(stack,obj);
	kxstack_set_return_state(stack, RET_METHODRETURN);
	return RET_METHODRETURN;
}

static KxReturn
kxactivation_instr_long_return(KxActivation *self, char **codep)
{
	KxActivation *long_return = self->long_return;

	// TODO: prevest is_over na novy rezim
	if (long_return->is_over) {
		KxException *exception = kxexception_new_with_text(KXCORE,"Blok with '^' evaluated when scope isn't running");
		kxstack_throw_object(KXSTACK,exception);
		return RET_THROW;
	}
	KxObject *obj = kxactivation_inner_stack_pop(self);
	KxStack *stack = KXSTACK;
	kxstack_set_long_return_to_activation(stack, long_return);
	kxstack_set_return_object(stack,obj);
	kxstack_set_return_state(stack, RET_LONGRETURN);
	return RET_LONGRETURN;
}

static KxReturn
kxactivation_instr_end_of_block(KxActivation *self, char **codep)
{
	return RET_BLOCKRETURN;
}


// You must specify message.start_search if target == NULL
static void 
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
	} else {
		self->message.target = kxactivation_inner_stack_pop(self);
		self->message.start_search  = self->message.target;

	}
	REF_ADD2(self->message.target);
}

static KxReturn
kxactivation_instr_unary_message(KxActivation *self, char **codep) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;
	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;

	kxactivation_message_prepare_from_inner_stack(self, NULL, 0, symbol_frame[(int)(*((*codep)++))]);
	

	KxObject *obj = kxmessage_send(&self->message);
	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);
}

static KxReturn
kxactivation_instr_binary_message(KxActivation *self, char **codep) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;
	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;

	kxactivation_message_prepare_from_inner_stack(self,NULL, 1,symbol_frame[(int)(*((*codep)++))]);

	KxObject *obj = kxmessage_send(&self->message);

	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);

}

static KxReturn
kxactivation_instr_local_unary_message(KxActivation *self, char **codep)
{
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;

	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;

	self->message.message_name = symbol_frame[(int)(*((*codep)++))];
	REF_ADD(self->message.message_name);

	self->message.params_count = 0;
	self->message.target = self->receiver;

	REF_ADD2(self->receiver);
	
	self->message.start_search = self->target;
	REF_ADD(self->message.start_search);

	KxObject *obj =  kxmessage_send(&self->message);
	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);

}

/*static void
kxactivation_resend_error() 
{
	printf("Fatal Error: resend instruction without slot_holder_of_scopeblock");
	abort();
}*/

static KxReturn
kxactivation_instr_resend_unary_message(KxActivation *self, char **codep)
{
	if (self->message.target)
		REF_REMOVE(self->message.target);

	/*if (!self->slot_holder_of_codeblock) {
		kxactivation_resend_error();
	}*/
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;

	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;

	self->message.message_name = symbol_frame[(int)(*((*codep)++))];
	REF_ADD(self->message.message_name);

	self->message.params_count = 0;

	self->message.target = self->receiver;
	REF_ADD2(self->message.target);
	KxObject *obj = kxmessage_resend(&self->message, self->slot_holder_of_codeblock);

	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);

}

static KxReturn
kxactivation_instr_keyword_message(KxActivation *self, char **codep) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;

	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;
	KxSymbol *symbol = symbol_frame[(int)(*((*codep)++))];

	int params = (int)(*((*codep)++));
	kxactivation_message_prepare_from_inner_stack(self,NULL, params,symbol);

	KxObject *obj = kxmessage_send(&self->message);

	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);

}

static KxReturn
kxactivation_instr_resend_keyword_message(KxActivation *self, char **codep) 
{
    /** TODO: remove this condition */
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;

	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;
	KxSymbol *symbol = symbol_frame[(int)(*((*codep)++))];

	int params = (int)(*((*codep)++));
	kxactivation_message_prepare_from_inner_stack(self,self->receiver, params,symbol);

	KxObject *obj = kxmessage_resend(&self->message, self->slot_holder_of_codeblock);

	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);

}

static KxReturn
kxactivation_instr_local_keyword_message(KxActivation *self, char **codep) 
{
	if (self->message.target)
		REF_REMOVE(self->message.target);
	self->message_num++;
	KxCodeBlock *codeblock = self->codeblock;

	KxSymbol **symbol_frame = KXCODEBLOCK_DATA(codeblock)->symbol_frame;
	KxSymbol *symbol = symbol_frame[(int)(*((*codep)++))];
	int params = (int)(*((*codep)++));

	kxactivation_message_prepare_from_inner_stack(self,self->receiver,params,symbol);

	self->message.start_search = self->target;
	REF_ADD(self->target);

	KxObject *obj = kxmessage_send(&self->message);

	if (obj) {
		kxactivation_inner_stack_push(self, obj);
		return RET_OK;
	} else 
		return kxstack_get_return_state(KXSTACK);
}

static KxReturn
kxactivation_instr_push_method(KxActivation *self, char **codep) 
{
	KxCodeBlock *codeblock = self->codeblock;
	KxCodeBlock **subblocks = KXCODEBLOCK_DATA(codeblock)->subcodeblocks;
	KxCodeBlock *method = subblocks[(int)(*((*codep)++))];
	REF_ADD(method);
	kxactivation_inner_stack_push(self, method);

	return RET_OK;
}

static KxReturn
kxactivation_instr_push_block(KxActivation *self, char **codep) 
{
	
	KxCodeBlock *codeblock = self->codeblock;
	KxCodeBlock **subblocks = KXCODEBLOCK_DATA(codeblock)->subcodeblocks;
	KxCodeBlock *block = subblocks[(int)(*((*codep)++))];

	KxScopedBlock *scopedblock = kxscopedblock_new(KXCORE, block, self);
	kxactivation_inner_stack_push(self, scopedblock);
	return RET_OK;
}

static KxReturn
kxactivation_instr_list(KxActivation *self, char **codep)
{
	int size = *((int*)*codep);
	*codep += sizeof(int);

	List *list = list_new_size(size);
	int t;

	for (t=size-1;t>=0;--t) {
		list->items[t] = kxactivation_inner_stack_pop(self);
	}

	KxList *kxlist = kxobject_raw_clone(kxcore_get_basic_prototype(KXCORE, KXPROTO_LIST));
	kxlist->data.ptr = list;

	kxactivation_inner_stack_push(self, kxlist);
	return RET_OK;
}


KxActivationInstrFcn *(instr_fcn)[KXCI_INSTRUCTIONS_COUNT] = 
{ 
	kxactivation_instr_unary_message,         // 0
	kxactivation_instr_binary_message,        // 1
	kxactivation_instr_keyword_message,       // 2
	kxactivation_instr_local_unary_message,   // 3
	kxactivation_instr_local_keyword_message, // 4
	kxactivation_instr_resend_unary_message,  // 5
	kxactivation_instr_resend_keyword_message, // 6
	kxactivation_instr_push_integer,          // 7 
	kxactivation_instr_push_string,           // 8 
	kxactivation_instr_push_float,            // 9
	kxactivation_instr_push_symbol,           // 10
	kxactivation_instr_push_character,           // 11
	kxactivation_instr_pop,                   // 12
	kxactivation_instr_push_method,           // 13
	kxactivation_instr_push_block,            // 14
	kxactivation_instr_list, 
	kxactivation_instr_end_of_block,          // 16
	kxactivation_instr_return,                // 17
	kxactivation_instr_returnself,            // 18
	kxactivation_instr_long_return,            // 19
};
