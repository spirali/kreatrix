/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXEXCEPTION_H
#define __KXEXCEPTION_H

#include "kxcore.h"
#include "kxstring.h"
typedef KxObject KxException;

#define KXTHROW_EXCEPTION(text)  KXTHROW(kxexception_new_with_text(KXCORE,(text)));

#define KXTHROW_REGEXCEPTION(group, name) KXTHROW(kxcore_get_registered_exception(KXCORE, (group), (name)));

KxObject * kxexception_new_prototype(KxCore *core);

/* Don't add own reference on message, BACKWARD COMPATABILITY */
KxObject * kxexception_new_with_message(KxCore *core, KxString *message);
KxObject * kxexception_new_with_text(KxCore *core, char *form, ...);

KxException *kxexception_type_error(char *type_name, KxObject *obj);



#endif
