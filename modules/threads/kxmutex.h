/*
   kxmutex.h
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
	

#ifndef __KXMUTEXS_MUTEX_H
#define __KXMUTEXS_MUTEX_H


#include <pthread.h>
#include <sys/types.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXMUTEX(self) ((self)->extension == &kxmutex_extension)

#define KXMUTEX_VALUE(self) ((pthread_mutex_t*) (self)->data.ptr)



typedef struct KxObject KxMutex;

KxObject *kxmutex_new_prototype(KxCore *core);
void kxmutex_extension_init();

extern KxObjectExtension kxmutex_extension;

#endif
