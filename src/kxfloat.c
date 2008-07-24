/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

/*KXobject Base Float
  [Basic prototypes] Prototype of floating point number.
  Protototype for objects that repreresents floating point values. All floating point litarals are clone of this object.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "kxfloat.h"
#include "kxobject.h"
#include "kxscopedblock.h"
#include "kxexception.h"
#include "kxinteger.h"

KxObjectExtension kxfloat_extension;

static void kxfloat_add_method_table(KxFloat *self);

void
kxfloat_init_extension() 
{
	kxobjectext_init(&kxfloat_extension);
	kxfloat_extension.type_name = "Float";
	kxfloat_extension.is_immutable = 1;
}



KxObject *
kxfloat_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxfloat_extension;

	kxfloat_add_method_table(object);
    
    object->data.doubleval = 0.0;

	return object;
}

KxFloat *
kxfloat_clone_with(KxFloat *self, double value)
{
	KxFloat *fl = kxobject_raw_clone(self);
	fl->data.doubleval = value;
	return fl;
}


KxFloat *kxfloat_new_with(KxCore *core, double value) 
{
	KxObject *prototype = kxcore_get_basic_prototype(core,KXPROTO_FLOAT);
	return kxfloat_clone_with(prototype, value);
}

char *
kxfloat_to_cstring(KxFloat *self)
{
	char tmp[24];
	snprintf(tmp,24,"%lg",KXFLOAT_VALUE(self));
	return strdup(tmp);
}

static KxObject *
kxfloat_plus(KxFloat *self, KxMessage *message)
{
	double selfvalue = KXFLOAT_VALUE(self);
	KXPARAM_TO_DOUBLE(param, 0);

	return KXFLOAT(selfvalue + param);
}


static KxObject *
kxfloat_minus(KxFloat *self, KxMessage *message)
{
	double selfvalue = KXFLOAT_VALUE(self);
	KXPARAM_TO_DOUBLE(param, 0);

	return KXFLOAT(selfvalue - param);
}

static KxObject *
kxfloat_mul(KxFloat *self, KxMessage *message)
{
	double selfvalue = KXFLOAT_VALUE(self);
	KXPARAM_TO_DOUBLE(param, 0);

	return KXFLOAT(selfvalue * param);
}


static KxObject *
kxfloat_div(KxFloat *self, KxMessage *message)
{
	double selfvalue = KXFLOAT_VALUE(self);
	KXPARAM_TO_DOUBLE(param, 0);

	return KXFLOAT(selfvalue / param);
}

static KxObject *
kxfloat_negated(KxInteger *self, KxMessage *message)
{
	double selfval = KXFLOAT_VALUE(self);
	return KXFLOAT(-selfval);
}

/*KXdoc asString
  Return receiver as string.
  Return string with decimal reprezentation of float.
*/
KxObject * 
kxfloat_as_string(KxFloat *self, KxMessage *message) 
{
	char tmp[50];
	snprintf(tmp,50,"%lg",KXFLOAT_VALUE(self));
	return KXSTRING(tmp);
}

KxObject *
kxfloat_as_integer(KxFloat *self, KxMessage *message) 
{
    return KXINTEGER((int) KXFLOAT_VALUE(self));
}

KxObject *
kxfloat_sqrt(KxFloat *self, KxMessage *message) 
{
	double f = sqrt(KXFLOAT_VALUE(self));
	return KXFLOAT(f);
}

KxObject *
kxfloat_sin(KxFloat *self, KxMessage *message) 
{
	double f = sin(KXFLOAT_VALUE(self));
	return KXFLOAT(f);
}

KxObject *
kxfloat_cos(KxFloat *self, KxMessage *message) 
{
	double f = cos(KXFLOAT_VALUE(self));
	return KXFLOAT(f);
}

static KxObject *
kxfloat_pow(KxInteger *self, KxMessage *message)
{
	double selfval = KXFLOAT_VALUE(self);
	KXPARAM_TO_LONG(param,0);
	return KXFLOAT(pow(selfval, param));
}

static KxObject *
kxfloat_eq(KxInteger *self, KxMessage *message)
{
	double selfval = KXFLOAT_VALUE(self);
	double paramval;
	KxObject *param = message->params[0];
	if (IS_KXFLOAT(param)) {
		paramval = KXFLOAT_VALUE(param);
	} else 	if (IS_KXINTEGER(param)) {
			paramval = (double) KXINTEGER_VALUE(param);
	} else {
			KXRETURN(KXCORE->object_false);
	}
	KXRETURN_BOOLEAN(selfval == paramval);
}

static KxObject *
kxfloat_neq(KxInteger *self, KxMessage *message)
{
	double selfval = KXFLOAT_VALUE(self);
	double paramval;
	KxObject *param = message->params[0];
	if (IS_KXFLOAT(param)) {
		paramval = KXFLOAT_VALUE(param);
	} else 	if (IS_KXINTEGER(param)) {
			paramval = (double) KXINTEGER_VALUE(param);
	} else {
			KXRETURN(KXCORE->object_false);
	}
	KXRETURN_BOOLEAN(selfval != paramval);
}

static int
kxfloat_compare_helper(KxInteger *self, KxObject *object,  KxInt *compare) 
{
	double objdouble;

	if (IS_KXFLOAT(object)) {
		objdouble = KXFLOAT_VALUE(object);
	} else if (IS_KXINTEGER(object)) {
		objdouble = (double) KXINTEGER_VALUE(object);
	} else {
		kxstack_throw_object(KXSTACK,kxobject_compare_exception(self, object));
		return 0;
    }

	
	if (KXFLOAT_VALUE(self) < objdouble) {
		*compare = -1;
	} else if (KXFLOAT_VALUE(self) > objdouble) {
		*compare = 1;
	} else {
		*compare = 0;
	}
    return 1;
}

static KxObject *
kxfloat_lt(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxfloat_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare < 0);
}


static KxObject *
kxfloat_gt(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxfloat_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare > 0);
}

static KxObject *
kxfloat_lteq(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxfloat_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare <= 0);
}

static KxObject *
kxfloat_gteq(KxInteger *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxfloat_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare >= 0);
}





static void 
kxfloat_add_method_table(KxFloat *self)
{
	KxMethodTable table[] = {
		{"==",1, kxfloat_eq },
		{"!=",1, kxfloat_neq },
		{"<",1, kxfloat_lt},
		{">",1, kxfloat_gt},
		{"<=",1, kxfloat_lteq},
		{">=",1, kxfloat_gteq},
		{"+",1, kxfloat_plus },
		{"-",1, kxfloat_minus },
		{"*",1, kxfloat_mul },
		{"/",1, kxfloat_div },
		{"**",1, kxfloat_pow },
		{"asString",0, kxfloat_as_string },
		{"asInteger",0, kxfloat_as_integer },
		{"asFloat",0, kxcfunction_returnself },
		{"copy",0, kxcfunction_returnself },
		{"sqrt",0, kxfloat_sqrt},
		{"sin",0, kxfloat_sin},
		{"cos",0, kxfloat_cos},
		{"negated",0, kxfloat_negated},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

