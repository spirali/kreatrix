/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>

#include "kxdynlib.h"
#include "kxobject.h"
#include "utils/utils.h"

KxObjectExtension kxdynlib_extension;

static void kxdynlib_free(KxDynLib *self);
static void kxdynlib_add_method_table(KxDynLib *self);


void
kxdynlib_init_extension() {
	kxobjectext_init(&kxdynlib_extension);
	kxdynlib_extension.type_name = "DynLib";
	kxdynlib_extension.free = kxdynlib_free;
}


KxObject *
kxdynlib_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxdynlib_extension;
	object->data.ptr = kxmalloc(sizeof(KxDynLibData));
	ALLOCTEST(object->data.ptr);

	kxdynlib_add_method_table(object);

	return object;
}


static void
kxdynlib_free(KxDynLib *self) 
{
	kxfree(self->data.ptr);
}

static void 
kxdynlib_add_method_table(KxDynLib *self)
{
	/*KxMethodTable table[] = {
		{"+",1, kxinteger_plus },
		{"-",1, kxinteger_minus },
		{"*",1, kxinteger_mul },
		{"/",1, kxinteger_div },
		{"inc",0, kxinteger_inc },
		{"dec",0, kxinteger_dec },
		{"print",0, kxinteger_print },
		{"repeat:",1, kxinteger_repeat },
		{"asString",0, kxinteger_as_string },
		{"isEven",0, kxinteger_is_even },
		{"isOdd",0, kxinteger_is_odd },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);*/
}

