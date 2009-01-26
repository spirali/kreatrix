/*
   ptracker.h
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

#ifndef __KXPROFILER_PTRACKER_H
#define __KXPROFILER_PTRACKER_H

#include "kxobject_struct.h"
#include "kxcore.h"

#include <sys/time.h>

typedef struct KxPTrackerData KxPTrackerData;
typedef struct KxTrackedMessage KxTrackedMessage;

struct KxTrackedMessage {
	KxObject *message_name;
	KxObject *slot_holder;
};

struct KxPTrackerData {
	int is_running;
	size_t counter;
	struct timeval children_time; // Running time of childrens + GC
	struct timeval cumulative_time;
	KxObject *tracked_object;
	KxTrackedMessage *tracked_messages;
};

#define KXPTRACKER_DATA(self) ((KxPTrackerData*) ((self)->data.intval))

#define IS_KXPTRACKER(self) ((self)->extension == &kxptracker_extension)

typedef struct KxObject KxPid;

KxObject *kxptracker_new_prototype(KxCore *core);
void kxptacker_extension_init();
KxObject *kxptracker_new(KxCore *core, KxObject *tracked_object);

KxObject *kxptracker_remove_from_tracked_object(KxObject *self);



extern KxObjectExtension kxptracker_extension;

#endif // __KXPROFILER_PTRACKER_H
