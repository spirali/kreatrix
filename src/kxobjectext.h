/*
   kxobjectext.h
   Copyright (C) 2007, 2008  Stanislav Bohm

   This file is part of Kreatrix.

   Kreatrix is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Kreatrix is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Kreatrix; see the file COPYING. If not, see 
   <http://www.gnu.org/licenses/>.
*/
	
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
typedef void(KxObjectFcnDump)(struct KxObject *self);
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
	KxObjectFcnDump *dump;

	char *type_name;
	KxObjectExtension *parent;

	int is_immutable;
};

void kxobjectext_init(KxObjectExtension *self);


#endif // __KXOBJECTEXT_H
