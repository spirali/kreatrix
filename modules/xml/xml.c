
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


#include "kxmodule.h"
#include "kxmessage.h"
#include "kxcfunction.h"
#include "kxobject.h"
#include "kxexception.h"

#include "kxxmldocument.h"
#include "kxxmlnode.h"

#define MAX_ERROR_STRING_LENGTH 10*1024

void kxxml_add_error_to_list(List *list, const char *str, ...)
{
	char buf[MAX_ERROR_STRING_LENGTH];
	va_list arg;

	va_start(arg,str);
	vsnprintf(buf,MAX_ERROR_STRING_LENGTH, str, arg);
	va_end(arg);

	list_append(list, strdup(buf));
}

KxObject *kxxml_throw_exception(KxCore *core, List *list)
{
	int t;
	char *string = strdup("");
	int len = 0;

	// Join string in list into one string
	for (t=0;t<list->size;t++)
	{
		int old_len = len;
		char *s = list->items[t];
		len += strlen(s);
		string = realloc(string, len+1);
		ALLOCTEST(string);
		strcpy(string + old_len, s);
	}
	KxException *excp = kxcore_clone_registered_exception_text(
			core, "module-xml","XmlException", string);
	free(string);
	list_free_all(list);
	//KXTHROW(excp);
	kxstack_throw_object(core->stack, excp);
	return NULL;
}

KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{
	LIBXML_TEST_VERSION

	kxxmldocument_extension_init();
	kxxmlnode_extension_init();

    KxObject *xmldocument = kxxmldocument_new_prototype(KXCORE);
    kxcore_add_prototype(KXCORE, xmldocument);
    kxobject_set_slot_no_ref2(self, KXSYMBOL("Document"), xmldocument);

    KxObject *xmlnode = kxxmlnode_new_prototype(KXCORE);
    kxcore_add_prototype(KXCORE, xmlnode);
    kxobject_set_slot_no_ref2(self, KXSYMBOL("Node"), xmlnode);
	kxxmlnode_add_node_types(xmlnode, self);

	kxcore_register_simple_exception(KXCORE, 
		"module-xml", "XmlException","XmlException");

	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxxmldocument_extension);

	/* Break cyclic references */
	KxObject *obj = kxcore_get_prototype(KXCORE, &kxxmlnode_extension);
    kxobject_clean(obj);

	kxcore_remove_prototype(KXCORE, &kxxmlnode_extension);

    xmlCleanupParser();
}
