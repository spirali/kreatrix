/*
   pid.h
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
	

#ifndef __KXSYSTEM_PID_H
#define __KXSYSTEM_PID_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXPID_VALUE(self) ((pid_t) ((self)->data.intval))

#define IS_KXPID(self) ((self)->extension == &kxpid_extension)

#include <sys/types.h>


typedef struct KxObject KxPid;

KxObject *kxpid_new_prototype(KxCore *core);
KxObject *kxpid_set_pid(KxPid *self, pid_t pid);
void kxpid_extension_init();

extern KxObjectExtension kxpid_extension;

#endif
