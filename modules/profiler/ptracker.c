/*
   ptracker.c
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

#include "ptracker.h"
#include "kxobject.h"
#include "kxcodeblock.h"
#include "../time/kxtimevalue.h"
#include "kxlist.h"
#include "kxinteger.h"
#include <string.h>
#include <stdio.h>

KxObjectExtension kxptracker_extension;

static void kxptracker_add_method_table(KxObject *self);
static void kxptracker_free(KxObject *self);
static void kxptracker_clean(KxObject *self);
static void kxptracker_mark(KxObject *self);

void kxptracker_extension_init() {
	kxobjectext_init(&kxptracker_extension);
	kxptracker_extension.type_name = "PTracker";
	kxptracker_extension.free = kxptracker_free;
	kxptracker_extension.clean = kxptracker_clean;
	kxptracker_extension.mark = kxptracker_mark;
	/*kxpid_extension.clone = kxpid_clone;*/
}

static KxPTrackerData * 
kxptracker_new_data()
{
	KxPTrackerData *data = kxmalloc(sizeof(KxPTrackerData));
	KxTrackedMessage *msg = kxmalloc(sizeof(KxTrackedMessage));
	ALLOCTEST(data && msg);
	memset(data, 0, sizeof(KxPTrackerData));
	data->tracked_messages = msg;
	msg->message_name = NULL;
	return data;
}

KxObject *
kxptracker_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxptracker_extension;
	self->data.ptr = kxptracker_new_data();
	kxptracker_add_method_table(self);
	return self;
}

KxObject *
kxptracker_new(KxCore *core, KxObject *tracked_object)
{
	KxObject *proto = kxcore_get_prototype(core, &kxptracker_extension);
	KxObject *self = kxobject_raw_clone(proto);
	self->data.ptr = kxptracker_new_data();
	KxPTrackerData *data = self->data.ptr;
	REF_ADD(tracked_object);
	data->tracked_object = tracked_object;
	return self;
}

KxObject *
kxptracker_remove_from_tracked_object(KxObject *self)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	KxObject *obj = data->tracked_object;
	if (obj == NULL)
		return;

	if (obj->extension == &kxcfunction_extension) {
		kxcfunction_objects_remove_object(obj, self);
	} else {
		if (obj->extension == &kxcodeblock_extension) {
			kxcodeblock_remove_literal(obj, self);
		} else {
			printf("Profiler internal error: Unknown tracked object\n");
			abort();
		}
	}
}

void
kxptracked_add_tracked_message(KxObject *self, KxObject *message_name, KxObject *slot_holder)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	KxTrackedMessage *tm = data->tracked_messages;
	int size = 2;
	while(tm->message_name != NULL) {
		size++;
		tm++;
	}
	data->tracked_messages = kxrealloc(data->tracked_messages, sizeof(KxTrackedMessage) * size);
	tm = data->tracked_messages + size - 2;
	REF_ADD(message_name);
	REF_ADD(slot_holder);
	tm->message_name = message_name;
	tm->slot_holder = slot_holder;
	tm++;
	tm->message_name = NULL;
	self->data.ptr = data;
}

void 
kxptracker_free(KxObject *self)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);

	KxTrackedMessage *tm = data->tracked_messages;

	while (tm->message_name != NULL) {
		REF_REMOVE(tm->message_name);
		REF_REMOVE(tm->slot_holder);
		tm++;
	}

	if (data->tracked_object != NULL) {
		REF_REMOVE(data->tracked_object);
	}
	kxfree(data->tracked_messages);
	kxfree(data);
}

void 
kxptracker_clean(KxObject *self)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	if (data->tracked_object != NULL) {
		REF_REMOVE(data->tracked_object);
		data->tracked_object = NULL;
	}

	KxTrackedMessage *tm = data->tracked_messages;
	while (tm->message_name != NULL) {
		REF_REMOVE(tm->message_name);
		REF_REMOVE(tm->slot_holder);
		tm++;
	}
	data->tracked_messages->message_name = NULL;
}

void 
kxptracker_mark(KxObject *self)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	if (data->tracked_object != NULL) {
		kxobject_mark(data->tracked_object);
	}

	KxTrackedMessage *tm = data->tracked_messages;
	while (tm->message_name != NULL) {
		kxobject_mark(tm->message_name);
		kxobject_mark(tm->slot_holder);
		tm++;
	}
}

static KxObject *
kxptracker_cumulative_time(KxObject *self, KxMessage *message)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	return KXTIMEVALUE(&data->cumulative_time);
}

static KxObject *
kxptracker_time(KxObject *self, KxMessage *message)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	struct timeval tv;
	timersub(&data->cumulative_time, &data->children_time, &tv);
	//return KXTIMEVALUE(&data->children_time);
	return KXTIMEVALUE(&tv);
}

static KxObject *
kxptracker_counter(KxObject *self, KxMessage *message)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	return KXINTEGER(data->counter);
}

static KxObject *
kxptracker_tracked_object(KxObject *self, KxMessage *message)
{
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	KXRETURN(data->tracked_object);
}

static KxObject *
kxptracker_calling_messages(KxObject *self, KxMessage *message)
{
	List *list = list_new();
	KxPTrackerData *data = KXPTRACKER_DATA(self);
	KxTrackedMessage *tm = data->tracked_messages;
	while (tm->message_name != NULL) {
		REF_ADD(tm->message_name);
		list_append(list, tm->message_name);
		REF_ADD(tm->slot_holder);
		list_append(list, tm->slot_holder);
		tm++;
	}
	return KXLIST(list);
}

void static
kxptracker_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"counter",0, kxptracker_counter },
		{"time",0, kxptracker_time },
		{"cumulativeTime",0, kxptracker_cumulative_time },
		{"callingMessages",0, kxptracker_calling_messages },
		{"trackedObject",0, kxptracker_tracked_object },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
