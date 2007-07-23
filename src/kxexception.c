/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdarg.h>
#include <stdio.h>

#include "kxexception.h"
#include "kxobject.h"
#include "kxstring.h"

KxObject * kxexception_new_prototype(KxCore *core) 
{
	KxException *self = kxcore_clone_base_object(core);
	
	kxobject_set_slot_no_ref2(self, kxcore_get_symbol(KXCORE,"type"), KXSTRING("Exception"));
	kxobject_set_slot_no_ref2(self, kxcore_get_symbol(KXCORE,"message"), KXSTRING(""));

	return self;
}

KxObject * kxexception_new_with_message(KxCore *core, KxString *message) 
{
	KxObject *self = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_EXCEPTION));
	kxobject_set_slot(self,KXCORE->dictionary[KXDICT_MESSAGE], message);
	REF_REMOVE(message);
	return self;
}

KxObject * kxexception_new_with_text(KxCore *core, char *form, ...) 
{
	char buf[1024];

	va_list arg;
	va_start(arg,form);
	vsnprintf(buf,1024, form, arg);
	va_end(arg);

	KxObject *message = kxstring_new_with(core,buf);

	KxObject *self = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_EXCEPTION));
	kxobject_set_slot(self,KXCORE->dictionary[KXDICT_MESSAGE], message);
	REF_REMOVE(message);
	return self;
}


KxException *
kxexception_type_error(char *type_name, KxObject *obj)
{
	KxObject *type_obj2 = kxobject_type_name(obj);
	char *type_str2 = KXSTRING_VALUE(type_obj2);
	KxException *excp = kxexception_new_with_text(KXCORE_FROM(obj), "%s expected as parameter, not %s", type_name, type_str2);
	REF_REMOVE(type_obj2);
	return excp;
}
