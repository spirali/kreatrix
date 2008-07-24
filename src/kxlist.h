/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


#ifndef __KXLIST_H
#define __KXLIST_H

#include "kxobject_struct.h"
#include "kxcore.h"

typedef struct KxObject KxList;


#define IS_KXLIST(kxobject) ((kxobject)->extension == &kxlist_extension)

#define KXLIST_DATA(kxlist) (List *)((kxlist)->data.ptr)
#define KXLIST_VALUE(kxlist) (List *)((kxlist)->data.ptr)

#define KXLIST(list) (kxlist_new_with(KXCORE,list))

#define KXPARAM_TO_LIST(list, param_id) \
	List * list; { KxObject *tmp = message->params[param_id]; if (IS_KXLIST(tmp)) \
	{ list = KXLIST_VALUE(tmp); } else { return kxobject_type_error(tmp, &kxlist_extension, param_id); }} 


void kxlist_init_extension();

KxList *kxlist_new_prototype(KxCore *core);




/** list is added directly into new object, no copy */
KxList *kxlist_new_with(KxCore *core, List *list);

/** list is not touched, new copy is createn */
KxList *kxlist_new_from_list_of_cstrings(KxCore *core, List *list);


List *kxlist_to_list_of_cstrings(KxList *self);




extern KxObjectExtension kxlist_extension;

#endif
