/**********************************************************************
 *
 *  kxarray2d.h - Array 2D 
 *
 *	$Id$
 *
 *
 **********************************************************************/

#ifndef __KX_OBJECT_ARRAY2D_H
#define __KX_OBJECT_ARRAY2D_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXARRAY2D_DATA(self) ((KxArray2dData*) (self)->data.ptr)

#define IS_KXARRAY2D(self) ((self)->extension == &kxarray2d_extension)

typedef struct KxObject KxArray2d;
typedef struct KxArray2dData KxArray2dData;

struct KxArray2dData {
	KxObject **array;
	int sizex;
	int sizey;
};


KxArray2d *kxarray2d_new_prototype(KxCore *core);


void kxarray2d_init_extension();
extern KxObjectExtension kxarray2d_extension;


#endif // __KX_OBJECT_ARRAY2D_H
