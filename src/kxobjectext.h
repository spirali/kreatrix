/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXOBJECTEXT_H
#define __KXOBJECTEXT_H

#include "kxtypes.h"

struct KxObject;
struct KxMessage;
struct KxIteratorData;
//enum KxReturn;

typedef struct KxObjectExtension KxObjectExtension;


typedef struct KxObject * (KxObjectFcnActivate)(struct KxObject *self, struct KxObject *target, struct KxMessage *message);
typedef struct KxObject*(KxObjectFcnClone)(struct KxObject *self);
typedef struct KxObject*(KxObjectFcnCopy)(struct KxObject *self);
typedef struct KxObject*(KxObjectFcnDepthCopy)(struct KxObject *self);
typedef void (KxObjectFcnMark)(struct KxObject *self);
typedef void(KxObjectFcnFree)(struct KxObject *self);
typedef void(KxObjectFcnClean)(struct KxObject *self);
typedef int(KxObjectFcnCompare)(struct KxObject *self, struct KxObject *param);
typedef struct KxObject * (KxObjectFcnIteratorCreate)(struct KxObject *self);
typedef struct KxObject * (KxObjectFcnIteratorNext)(struct KxObject *self, struct KxIteratorData *iterator);

struct KxObjectExtension {
	

	KxObjectFcnActivate *activate;
	KxObjectFcnClone *clone;
	KxObjectFcnFree *free;
	KxObjectFcnMark *mark;
	//KxObjectFcnCopy *copy;
	//KxObjectFcnDepthCopy *depth_copy;
	KxObjectFcnClean *clean;

	KxObjectFcnIteratorCreate *iterator_create;
	KxObjectFcnIteratorNext *iterator_next;

	char *type_name;
	KxObjectExtension *parent;

	int is_immutable;
};

void kxobjectext_init(KxObjectExtension *self);


#endif // __KXOBJECTEXT_H
