/*
   kxthread.h
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
	

#ifndef __KXTHREADS_THREAD_H
#define __KXTHREADS_THREAD_H


#include <pthread.h>
#include <sys/types.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXTHREAD(self) ((self)->extension == &kxthread_extension)

#define KXTHREAD_DATA(self) ((KxThreadData*) (self)->data.ptr)



typedef struct KxObject KxThread;
typedef struct KxThreadData KxThreadData;

KxObject *kxthread_new_prototype(KxCore *core);
void kxthread_extension_init();

typedef enum  { 
	KXTHREAD_STATE_INIT = 0,
	KXTHREAD_STATE_RUNNING_JOINABLE,
	KXTHREAD_STATE_RUNNING_DETACHED,
	KXTHREAD_STATE_WAITING_FOR_JOIN,
	KXTHREAD_STATE_STOPPED
} KxThreadState;

struct KxThreadData {
	KxThreadState state;
	pthread_t pthread;
	KxObject *return_value;
};

extern KxObjectExtension kxthread_extension;

#endif
