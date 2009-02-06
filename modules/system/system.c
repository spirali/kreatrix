/*
   system.c
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
	

#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "pid.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"

void static kxsystem_add_method_table(KxObject *self);



KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxpid_extension_init();

	kxsystem_add_method_table(self);

	KxPid *kxpid_prototype = kxpid_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxpid_prototype);
	
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Pid"),kxpid_prototype);
	
	KXRETURN(self);
}



void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxpid_extension);
}

static KxObject * 
kxsystem_fork(KxObject *self, KxMessage *message) 
{
	pid_t pid = fork();
	if (pid < -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,"Fork failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	KxPid *kxpid = kxcore_clone_prototype(KXCORE, &kxpid_extension);
	kxpid_set_pid(kxpid, pid);
	//RETURN_AND_REFREMOVE(kxpid);
	return kxpid;
}

static KxObject *
kxsystem_exit(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_LONG(param,0);
	exit(param);
}

static KxObject *
kxsystem_sleep(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	
	KX_THREADS_BEGIN
	sleep(param);
	KX_THREADS_END

	KXRETURN(self);
}

static KxObject *
kxsystem_usleep(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	
	KX_THREADS_BEGIN
	usleep(param);
	KX_THREADS_END

	KXRETURN(self);
}


static KxObject *
kxsystem_wait_for_status_of_any_child_process(KxObject *self, KxMessage *message)
{
	int status;
	
	errno = 0;
	
	int pid;
	KX_THREADS_BEGIN
	pid = waitpid(-1,&status,0);
	KX_THREADS_END

	if (pid == 0 || (pid < 0 && errno == ECHILD)) {
		KXRETURN(KXCORE->object_nil);
	}


	if (pid < 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"waitpid failed: %s", strerror(errno));
		KXTHROW(excp);
	}

	KxPid *kxpid = kxcore_clone_prototype(KXCORE, &kxpid_extension);
	kxpid_set_pid(kxpid, pid);

	// TODO: use symbol frame in KxCFunction
	KxSymbol *symbol = kxcore_get_symbol(KXCORE,"status");
	kxobject_set_slot_no_ref2(kxpid,symbol, KXINTEGER(status));

	return kxpid;
}

// same as kxsystem_wait_for_status_of_any_child_process
// no wait, returns nil if no terminated process
static KxObject *
kxsystem_get_status_of_any_child_process(KxObject *self, KxMessage *message)
{
	int status;
	
 	errno = 0;
	int pid = waitpid(-1,&status,WNOHANG);


	if (pid == 0 || (pid < 0 && errno == ECHILD)) {
		//RETURN_NIL;
		KXRETURN(KXCORE->object_nil);
	}

	if (pid < 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"waitpid failed: %s", strerror(errno));
 		KXTHROW(excp);
 	}


	KxPid *kxpid = kxcore_clone_prototype(KXCORE, &kxpid_extension);
	kxpid_set_pid(kxpid, pid);

	// TODO: use symbol frame in KxCFunction
	KxSymbol *symbol = kxcore_get_symbol(KXCORE,"status");
	kxobject_set_slot_no_ref2(kxpid,symbol, KXINTEGER(status));

	//KXRETURN(kxpid);
	//kxobject_dump(kxpid);
	return kxpid;
}

static KxObject *
kxsystem_run_command(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param, 0)

	errno = 0;
	int ret = system(param);

	if (ret < 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"system failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	KXRETURN(self);
}


static KxObject *
kxsystem_getenv(KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param,0);
	char *result = getenv(param);
	if (result == NULL) {
	//	RETURN_NIL;
		KXRETURN(KXCORE->object_nil);
	}
	KxString *str = KXSTRING(result);
	//RETURN(str);
	return str;
}

void static
kxsystem_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"fork",0, kxsystem_fork },
		{"exit:",1, kxsystem_exit },
		{"sleep:",1, kxsystem_sleep },
		{"usleep:",1, kxsystem_usleep },
		{"getenv:",1, kxsystem_getenv},
		{"runCommand:",1, kxsystem_run_command},
		{"waitForStatusOfAnyChildProcess",0, kxsystem_wait_for_status_of_any_child_process },
		{"getStatusOfAnyChildProcess",0, kxsystem_get_status_of_any_child_process },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
