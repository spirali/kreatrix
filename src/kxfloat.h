/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXFLOAT_H
#define __KXFLOAT_H

#include "kxobject_struct.h"
#include "kxcore.h"
#include "kxstack.h"

typedef struct KxObject KxFloat;

#define IS_KXFLOAT(kxobject) ((kxobject)->extension == &kxfloat_extension)

#define KXFLOAT_VALUE(kxfloat) (double) (kxfloat)->data.doubleval


#define KXPARAM_TO_DOUBLE(double_var, param_id) \
	double double_var; { KxObject *tmp = message->params[param_id]; if (IS_KXFLOAT(tmp)) \
	{ double_var = KXFLOAT_VALUE(tmp); } else { kxobject_type_error(tmp, &kxfloat_extension, param_id); return NULL; }} 


#define KXFLOAT(fl) kxfloat_new_with(KXCORE,fl)

extern KxObjectExtension kxfloat_extension;

void kxfloat_init_extension();

KxObject *kxfloat_new_prototype(KxCore *core);

KxFloat *kxfloat_clone_with(KxFloat *self, double value);

KxFloat *kxfloat_new_with(KxCore *core, double value);

#endif
