/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

/*KXobject Base Integer
 [Basic prototypes] Prototype of integer.
  Protototype for objects that repreresents integer values. All integer litarals are clone of this object.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kxinteger.h"
#include "kxobject.h"
#include "kxscopedblock.h"
#include "kxexception.h"
#include "kxfloat.h"
#include "kxcharacter.h"

KxObjectExtension kxinteger_extension;

static void kxinteger_add_method_table(KxInteger *self);
//static void kxinteger_free(KxInteger *self);
static KxInteger* kxinteger_clone(KxInteger *self);

void
kxinteger_init_extension() 
{
	kxobjectext_init(&kxinteger_extension);
	kxinteger_extension.type_name = "Integer";
	kxinteger_extension.clone = kxinteger_clone;
}


KxObject *
kxinteger_clone(KxInteger *self) 
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.intval = self->data.intval;
	return clone;
}


KxObject *
kxinteger_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->data.intval = 0;
	object->extension = &kxinteger_extension;

	kxinteger_add_method_table(object);


	return object;
}

KxInteger *
kxinteger_clone_with(KxInteger *self, KxInt value)
{
	KxInteger *integer = kxobject_raw_clone(self);
	integer->data.ptr = (void*)value;

	return integer;
}


KxInteger *kxinteger_new_with(KxCore *core, KxInt value) 
{
	KxObject *prototype = kxcore_get_basic_prototype(core,KXPROTO_INTEGER);
	return kxinteger_clone_with(prototype, value);
}

char *
kxinteger_to_cstring(KxInteger *self)
{
	char tmp[24];
	snprintf(tmp,24,"%i",(int)KXINTEGER_VALUE(self));
	return strdup(tmp);
}

static KxObject *
kxinteger_plus(KxInteger *self, KxMessage *message)
{
	if (IS_KXFLOAT(message->params[0])) {
		KXPARAM_TO_DOUBLE(param,0);
		return KXFLOAT(KXINTEGER_VALUE(self) + param);
	}

	KxInt selfvalue = KXINTEGER_VALUE(self);
	KXPARAM_TO_LONG(param, 0);

	return KXINTEGER(selfvalue + param);
}

static KxObject *
kxinteger_minus(KxInteger *self, KxMessage *message)
{
	if (IS_KXFLOAT(message->params[0])) {
		KXPARAM_TO_DOUBLE(param,0);
		return KXFLOAT(KXINTEGER_VALUE(self) - param);
	}

	KxInt selfvalue = KXINTEGER_VALUE(self);
	KXPARAM_TO_LONG(param, 0);

	return KXINTEGER(selfvalue - param);
}

static KxObject *
kxinteger_mul(KxInteger *self, KxMessage *message)
{
	if (IS_KXFLOAT(message->params[0])) {
		KXPARAM_TO_DOUBLE(param,0);
		return KXFLOAT(KXINTEGER_VALUE(self) * param);
	}
	KXPARAM_TO_LONG(param, 0);
	return KXINTEGER(KXINTEGER_VALUE(self) * param);
}


static KxObject *
kxinteger_div(KxInteger *self, KxMessage *message)
{
	if (IS_KXFLOAT(message->params[0])) {
		KXPARAM_TO_DOUBLE(param,0);
		return KXFLOAT(KXINTEGER_VALUE(self) / param);
	}

	KxInt selfvalue = KXINTEGER_VALUE(self);
	KXPARAM_TO_LONG(param, 0);

	return KXINTEGER(selfvalue / param);
}

/*KXdoc isEven
  Test if receiver is even number.
  Return true if receiver is even number, otherwise false.
*/
static KxObject *
kxinteger_is_even(KxInteger *self, KxMessage *message)
{
	KxInt selfvalue = KXINTEGER_VALUE(self);
	KXRETURN_BOOLEAN(!(selfvalue & 1));
}

/*KXdoc isOdd
  Test if receiver is odd number.
  Return true if receiver is odd number, otherwise false.
*/
static KxObject *
kxinteger_is_odd(KxInteger *self, KxMessage *message)
{
	KxInt selfvalue = KXINTEGER_VALUE(self);
	KXRETURN_BOOLEAN(selfvalue & 1);
}

/*KXdoc repeat: aBlock
 Evaluate blok receiver-times
 If receiver is positive number, aBlock is receiver-times executed and
 Value of last execution is returned.
 if number is negative number or zero. nil is returned and aBlock isn't executed.
 <br/>
 aBlock is block with no parameters or with one parameter. If block has parameter 
  then in parameter is stored sequnece number of execution - 1.

*/
static KxObject *
kxinteger_repeat(KxInteger *self, KxMessage *message)
{
	KxInt selfvalue = KXINTEGER_VALUE(self);
	
	KxObject *param = message->params[0];

	if (selfvalue <= 0) {
		KXRETURN(KXCORE->object_nil);
	}

	// Param is block with 1 param
	if (IS_KXSCOPEDBLOCK(param) && (KXCODEBLOCK_DATA(KXSCOPEDBLOCK_DATA(param)->codeblock)->params_count == 1)) {
		KxMessage msg;
		msg.message_name = NULL;
		msg.params_count = 1;
		msg.target = self;

		int t;
		for (t=0;t<selfvalue-1;t++) {

			// TODO: recylace pocitadla
			KxInteger *i = KXINTEGER(t);
			msg.params[0] = i;
			KxObject * ret =  kxobject_evaluate_block(message->params[0],&msg);
			REF_REMOVE(i);
			KXCHECK(ret);
			REF_REMOVE(ret);
		}
		KxInteger *i = KXINTEGER(t);
		msg.params[0] = i;
		KxObject * ret =  kxobject_evaluate_block(message->params[0],&msg);
		REF_REMOVE(i);
		return ret;
	} else {
	/*printf(">>>>>REPEAT:");
	kxobject_dump(message->params[0]);*/

		// Generic param
		int t;
		for (t=0;t<selfvalue-1;t++) {
			KxObject * ret = kxobject_evaluate_block_simple(message->params[0]);
			KXCHECK(ret);
			REF_REMOVE(ret);
		}
	    return kxobject_evaluate_block_simple(message->params[0]);

	}
	KXRETURN(self);
}

static KxObject *
kxinteger_to_by_do(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param0,0);
	KXPARAM_TO_LONG(step,1);
	KxObject *block = message->params[2];
	KxInt selfvalue = KXINTEGER_VALUE(self);

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = self;

	if (step == 0) {
		KXTHROW_EXCEPTION("Step can't be zero");
	}


	int t;
	if (step > 0) {
		if (selfvalue >= param0 ) {
			KXRETURN(KXCORE->object_nil);
		}
		int end = param0 - step;

		for (t=selfvalue;t<end;t+=step) {
			// TODO: recylace pocitadla

			KxInteger *i = KXINTEGER(t);
			msg.params[0] = i;
			KxObject * ret =  kxobject_evaluate_block(block,&msg);
			REF_REMOVE(i);
			KXCHECK(ret);
			REF_REMOVE(ret);
		}

	} else {
		if (selfvalue <= param0 ) {
			KXRETURN(KXCORE->object_nil);
		}
		int	end = param0 - step;

		for (t=selfvalue;t>end;t+=step) {
			// TODO: recylace pocitadla
			KxInteger *i = KXINTEGER(t);
			msg.params[0] = i;
			KxObject * ret =  kxobject_evaluate_block(block,&msg);
			REF_REMOVE(i);
			KXCHECK(ret);
			REF_REMOVE(ret);
		}

	}


	KxInteger *i = KXINTEGER(t);
	msg.params[0] = i;
	KxObject * ret =  kxobject_evaluate_block(block,&msg);
	REF_REMOVE(i);
	return ret;
}

/*KXdoc asString
  Return receiver as string.
  Return string with decimal reprezentation of receiver.
*/
static KxObject * 
kxinteger_as_string(KxInteger *self, KxMessage *message) 
{
	char tmp[50];
	snprintf(tmp,50,"%li",KXINTEGER_VALUE(self));
	return KXSTRING(tmp);
}

/*KXdoc asCharacter
  Return ASCII character whose value is receiver.
 */
static KxObject * 
kxinteger_as_character(KxInteger *self, KxMessage *message) 
{
	KxInt val = KXINTEGER_VALUE(self);
	return KXCHARACTER(val);
}

/*KXdoc asFloat
  Return value as float value.
*/
static KxObject * 
kxinteger_as_float(KxInteger *self, KxMessage *message) 
{
	 return KXFLOAT((double)KXINTEGER_VALUE(self));
}

static KxObject *
kxinteger_bit_and(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint & param);
}

static KxObject *
kxinteger_bit_or(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint | param);
}

static KxObject *
kxinteger_bit_xor(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint ^ param);
}

static KxObject *
kxinteger_mod(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint % param);
}


static KxObject *
kxinteger_shift_left(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint << param);
}

static KxObject *
kxinteger_shift_right(KxInteger *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(selfint >> param);
}



static KxObject *
kxinteger_is_zero(KxInteger *self, KxMessage *message)
{
	long selfint = KXINTEGER_VALUE(self);
	KXRETURN_BOOLEAN(selfint == 0);
}

static KxObject *
kxinteger_is_not_zero(KxInteger *self, KxMessage *message)
{
	long selfint = KXINTEGER_VALUE(self);
	KXRETURN_BOOLEAN(selfint != 0);
}

static KxObject *
kxinteger_negated(KxInteger *self, KxMessage *message)
{
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(-selfint);
}

static KxObject *
kxinteger_bit_negated(KxInteger *self, KxMessage *message)
{
	long selfint = KXINTEGER_VALUE(self);
	return KXINTEGER(~selfint);
}


static KxObject *
kxinteger_exp(KxInteger *self, KxMessage *message)
{
	// TODO: Need implementation 
	/*long selfint = KXINTEGER_VALUE(self);
	KXPARAM_TO_LONG(param,0);
	long t;
	long x = 1;
	for (t=0;t<param;t++) {
		x *= selfint;
	}
	return KXINTEGER(x);*/
	printf("kxinteger_exp: Need implementation\n");
	exit(-1);
}

static int
kxinteger_compare_helper(KxInteger *self, KxObject *object, KxInt *compare) 
{
	KxInt selfvalue = KXINTEGER_VALUE(self);
	if (IS_KXINTEGER(object)) {
		KxInt objval = KXINTEGER_VALUE(object);
		*compare = selfvalue - objval;
        return 1;
    } else if (IS_KXFLOAT(object)) {
		double selfdouble = (double)selfvalue;
		double objdouble = KXFLOAT_VALUE(object);
		*compare = selfdouble - objdouble;
        return 1;
	} else {
		kxstack_throw_object(KXSTACK,kxobject_compare_exception(self, object));
		return 0;
	}
}

static KxObject *
kxinteger_eq(KxInteger *self, KxMessage *message) 
{
	if (IS_KXINTEGER(message->params[0])) {
		KxInt selfvalue = KXINTEGER_VALUE(self);
		KxInt paramvalue = KXINTEGER_VALUE(message->params[0]);
		KXRETURN_BOOLEAN(selfvalue == paramvalue);
	} else 
		KXRETURN(KXCORE->object_false);
	
}

static KxObject *
kxinteger_neq(KxInteger *self, KxMessage *message) 
{
	if (IS_KXINTEGER(message->params[0])) {
		KxInt selfvalue = KXINTEGER_VALUE(self);
		KxInt paramvalue = KXINTEGER_VALUE(message->params[0]);
		KXRETURN_BOOLEAN(selfvalue != paramvalue);
	} else 
		KXRETURN(KXCORE->object_true);
	
}

static KxObject *
kxinteger_lt(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxinteger_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare < 0);
}


static KxObject *
kxinteger_gt(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxinteger_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare > 0);
}

static KxObject *
kxinteger_lteq(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxinteger_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare <= 0);
}

static KxObject *
kxinteger_gteq(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxinteger_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare >= 0);
}

static KxObject *
kxinteger_abs(KxInteger *self, KxMessage *message)
{
	KxInt i = KXINTEGER_VALUE(self);
	if (i < 0) {
		return KXINTEGER(-i);
	} 
	KXRETURN(self);
}

static void 
kxinteger_add_method_table(KxInteger *self)
{
	KxMethodTable table[] = {
		{"==",1, kxinteger_eq},
		{"!=",1, kxinteger_neq},
		{"<",1, kxinteger_lt},
		{">",1, kxinteger_gt},
		{"<=",1, kxinteger_lteq},
		{">=",1, kxinteger_gteq},
		{"+",1, kxinteger_plus },
		{"-",1, kxinteger_minus },
		{"*",1, kxinteger_mul },
		{"/",1, kxinteger_div },
		{"**",1, kxinteger_exp },
		{"hash",0, kxcfunction_returnself},
		{"repeat:",1, kxinteger_repeat },
		{"to:by:do:",3, kxinteger_to_by_do },
		{"asString",0, kxinteger_as_string },
		{"isEven",0, kxinteger_is_even },
		{"isOdd",0, kxinteger_is_odd },
		{"asInteger",0, kxcfunction_returnself },
		{"copy",0, kxcfunction_returnself },
		{"asFloat",0, kxinteger_as_float },
		{"bitAnd:",1, kxinteger_bit_and },
		{"bitOr:",1, kxinteger_bit_or },
		{"bitXor:",1, kxinteger_bit_xor },
		{"shiftLeft:",1, kxinteger_shift_left },
		{"shiftRight:",1, kxinteger_shift_right },
		{"mod:",1, kxinteger_mod },
		{"asCharacter",0, kxinteger_as_character },
		{"isNotZero",0, kxinteger_is_not_zero},
		{"isZero",0, kxinteger_is_zero},
		{"negated",0, kxinteger_negated},
		{"bitNegated",0, kxinteger_bit_negated},
		{"abs", 0, kxinteger_abs},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}


void 
kxinteger_type_error(KxCore *core)
{
	// Throw exception
	KxException *excp = kxexception_new_with_message(core,
		kxstring_new_with(core,"Integer expected as parameter"));
	kxstack_throw_object(core->stack, excp);
}
