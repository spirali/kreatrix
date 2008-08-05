/*
   kxpoll.c
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
	

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "kxpoll.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxstring.h"
#include "kxinteger.h"
#include "kxexception.h"
#include "kxcore.h"

KxObjectExtension kxpoll_extension;

static KxObject *kxpoll_clone(KxPoll *self);

static void kxpoll_add_method_table(KxObject *self);
static void kxpoll_free(KxObject *self);
static void kxpoll_clean(KxPoll *self);
static void kxpoll_mark(KxPoll *self);


void kxpoll_extension_init() {

	kxobjectext_init(&kxpoll_extension);
	kxpoll_extension.type_name = "Poll";
	kxpoll_extension.free = kxpoll_free;
	kxpoll_extension.clone = kxpoll_clone;
	kxpoll_extension.mark = kxpoll_mark;
	kxpoll_extension.clean = kxpoll_clean;

}

static void kxpoll_clean(KxPoll *self) {
	KxPollData *data = KXPOLL_DATA(self);
	list_foreach(data->obj_list, kxobject_ref_remove);
	list_clean(data->obj_list);
}


static void kxpoll_free(KxPoll *self) {
	KxPollData *data = KXPOLL_DATA(self);
	list_foreach(data->obj_list, kxobject_ref_remove);
	list_free(data->obj_list);
	if (data->pfd) {
		free(data->pfd);
	}
	free(data);
}

static KxObject *
kxpoll_clone(KxPoll *self)
{
	KxPollData *old_data = KXPOLL_DATA(self);

	KxPoll *clone = kxobject_raw_clone(self);
	
	KxPollData *data = calloc(sizeof(KxPollData),1);
	ALLOCTEST(data);
	data->obj_list = list_new();
	
	clone->data.ptr = data;
	return clone;
}

static void
kxpoll_mark(KxPoll *self) 
{
	KxPollData *data = KXPOLL_DATA(self);
	list_foreach(data->obj_list,(ListForeachFcn*)kxobject_mark);
}

KxObject * 
kxpoll_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxpoll_extension;

	KxPollData *data = calloc(sizeof(KxPollData),1);
	ALLOCTEST(data);
	data->obj_list = list_new();
	self->data.ptr = data;

	kxpoll_add_method_table(self);


	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLIN"),KXINTEGER(POLLIN));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLPRI"),KXINTEGER(POLLPRI));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLOUT"),KXINTEGER(POLLOUT));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLERR"),KXINTEGER(POLLERR));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLHUP"),KXINTEGER(POLLHUP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POLLNVAL"),KXINTEGER(POLLNVAL));


	return self;
}

static KxObject *
kxpoll_run(KxPoll *self, KxMessage *message)
{
	// TODO: check buffers
	KxPollData *data = KXPOLL_DATA(self);

	int t;
	for (t=0;t<data->pfd_count;t++) {
		data->pfd[t].revents = 0;
	}

	int ret;
	KX_THREADS_BEGIN
	ret = poll(data->pfd, data->pfd_count, data->timeout);
	KX_THREADS_END
	return KXINTEGER(ret);
}

static KxObject *
kxpoll_timeout(KxPoll *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	KxPollData *data = KXPOLL_DATA(self);
	data->timeout = param;
	KXRETURN(self);
}

static KxObject *
kxpoll_send_as_file_descriptor(KxPoll *self, KxMessage *message, KxObject *target, int *fd)
{
	KxSymbol *symbol = kxcore_get_symbol(KXCORE,"asFileDescriptor");
	
	KxObject *obj = kxobject_send_unary_message(target,symbol);
	/*if (ret != RET_OK)
		return ret;*/
	KXCHECK(obj);
	
	if (!IS_KXINTEGER(obj)) {
		REF_REMOVE(obj);
		KXTHROW_EXCEPTION("message 'asFileDescriptor' hasn't returned Integer object");
	}

	*fd = KXINTEGER_VALUE(obj);
	return obj;
}


static KxObject *
kxpoll_add_with_flags(KxPoll *self, KxMessage *message)
{
	KXPARAM_TO_LONG(flags,1);
	KxObject *obj = message->params[0];

	int fd;
	KxObject * fdobj = kxpoll_send_as_file_descriptor(self, message, obj, &fd);
	KXCHECK(fdobj);
	REF_REMOVE(fdobj);

	KxPollData *data = KXPOLL_DATA(self);
	int count = data->pfd_count;
	data->pfd = realloc(data->pfd,(count+1)*sizeof(struct pollfd));
	ALLOCTEST(data->pfd);

	REF_ADD(obj);
	list_append(data->obj_list,obj);

	data->pfd[count].fd = fd;
	data->pfd[count].events = flags;
	data->pfd[count].revents = 0;
	data->pfd_count++;
	KXRETURN(self);
}

KxObject *kxpoll_get_pollresult_proto(KxPoll *self) 
{
	return kxobject_find_slot(self, KXSYMBOL("PollResult"));
}

KxObject *
kxpollresult_clone_with(KxObject *self, int result)
{
	KxObject *clone = kxobject_clone(self);
	if (clone) {
		clone->data.intval = result;
		return clone;
	} else {
		KXRETURN(KXCORE->object_nil);
	}
}

static KxObject *
kxpoll_foreach(KxPoll *self, KxMessage *message) 
{
	KxObject *block = message->params[0];
	KxPollData *data = KXPOLL_DATA(self);
	List *list =  data->obj_list;

	if (list->size == 0) {
		KXRETURN(KXCORE->object_nil);	
	}

	KxMessage msg;
	//msg.stack = message->stack;
	msg.message_name = NULL;
	msg.params_count = 2;
	msg.target = block;

	KxObject *pollresult_proto = kxpoll_get_pollresult_proto(self);

	int t;
	for (t=0;t<list->size-1;t++) {
		msg.params[0] = list->items[t];
		msg.params[1] = kxpollresult_clone_with(pollresult_proto, data->pfd[t].revents);
		KxObject *o = kxobject_evaluate_block(block,&msg);
		
		REF_REMOVE(msg.params[1]);

		KXCHECK(o);
		REF_REMOVE(o);
	}
	msg.params[0] = list->items[t];
	msg.params[1] = kxpollresult_clone_with(pollresult_proto, data->pfd[t].revents);
	KxObject *obj = kxobject_evaluate_block(block,&msg);
	REF_REMOVE(msg.params[1]);
	return obj;
}

static KxObject *
kxpoll_remove(KxPoll *self, KxMessage *message)
{
	KxObject *obj = message->params[0];

	
	int fd;
	KxObject * o = kxpoll_send_as_file_descriptor(self, message, obj, &fd);
	KXCHECK(o);
	REF_REMOVE(o);

	KxPollData *data = KXPOLL_DATA(self);
	int t;
	for (t=0; t<data->pfd_count; t++) {
		if (data->pfd[t].fd == fd)
			break;
	}

	if (t == data->pfd_count) {
		KXTHROW_EXCEPTION("FileDescriptor not in poll list");
	}

	list_remove(data->obj_list,t);

	if (t != data->pfd_count -1) {
		memmove(&data->pfd[t],&data->pfd[t+1],data->pfd_count - t);
	}
	data->pfd_count--;

	data->pfd = realloc(data->pfd,(data->pfd_count)*sizeof(struct pollfd));
	ALLOCTEST(data->pfd);


	//RETURN(self);
	KXRETURN(self);
}



static KxObject *
kxpoll_method_clean(KxPoll *self, KxMessage *message) {
	KxPollData *data = KXPOLL_DATA(self);
	list_foreach(data->obj_list, kxobject_ref_remove);
	list_clean(data->obj_list);

	if (data->pfd) {
		free(data->pfd);
		data->pfd = NULL;
		data->pfd_count = 0;
	}

	KXRETURN(self);
	//data->pfd_count = 0;
}


void static
kxpoll_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		// TODO: change events sockets
		{"run",0,kxpoll_run},
		{"timeout:",1,kxpoll_timeout},
		{"add:withFlags:",2,kxpoll_add_with_flags},
		{"foreach:",1,kxpoll_foreach},
		{"clean",0, kxpoll_method_clean},
		{"remove:",1, kxpoll_remove},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
