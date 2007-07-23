#ifndef __KX_MODULE_XML
#define __KX_MODULE_XML

void kxxml_add_error_to_list(List *list, const char *str, ...);
KxObject *kxxml_throw_exception(KxCore *core, List *list);

#endif
