/*
   profiler.c
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
#include <stdlib.h>
#include <string.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "ptracker.h"
#include "kxcodeblock.h"
#include "kxcfunction.h"
#include "kxlist.h"
#include "kxinteger.h"
#include "../time/kxtimevalue.h"

void static kxprofiler_add_method_table(KxObject *self);

static int kxprofiler_enabled = 0;
static KxPTrackerData *kxprofiler_running_ptracker_data = NULL;
static KxPTrackerData kxprofiler_dummy_ptracker_data;
static KxObjectFcnActivate *kxprofiler_original_codeblock_activate;
static KxObjectFcnActivate *kxprofiler_original_cfunction_activate;
static size_t kxprofiler_gc_counter;
static struct timeval kxprofiler_gc_cumulative_time;
static struct timeval kxprofiler_gc_starttime;

KxObject *
kxmodule_main(KxModule *self, KxMessage *message) 
{
	kxptracker_extension_init();

	kxprofiler_add_method_table(self);

	KxPid *kxptracker_prototype = kxptracker_new_prototype(KXCORE);
	kxcore_add_prototype(KXCORE, kxptracker_prototype);
	
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PTracker"),kxptracker_prototype);
	
	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
	kxcore_remove_prototype(KXCORE, &kxptracker_extension);
}

KxObject * kxprofiler_cfunction_activate(KxObject *self, KxObject *target, KxMessage *message)
{
	KxObject *slot_holder = message->slot_holder;
	/* This can be removed after problem with changing slot_holder in scopedblock_run */

	KxObject *tracker = kxcfunction_objects_find_by_extension(self, &kxptracker_extension);
	if (tracker == NULL) {
		tracker = kxptracker_new(KXCORE, self);
		kxcfunction_objects_add_object(self, tracker);
		REF_REMOVE(tracker);
	}
	KxPTrackerData *prev_data = kxprofiler_running_ptracker_data;
	KxPTrackerData *data = KXPTRACKER_DATA(tracker);
	int is_running = data->is_running;
	data->is_running = 1;
	kxprofiler_running_ptracker_data = data;

	struct timeval tv_start;
	struct timeval tv_end;
	gettimeofday(&tv_start, NULL);
	KxObject *result = kxprofiler_original_cfunction_activate(self, target, message);


	gettimeofday(&tv_end, NULL);

	struct timeval diff_tv;
	timersub(&tv_end, &tv_start, &diff_tv);

	data->counter++;

	if (is_running) {
		/* Add own cumulative time */
		struct timeval time = data->children_time;
		timersub(&time, &diff_tv, &data->children_time);
	} else {
		/* Add own cumulative time */
		struct timeval time = data->cumulative_time;
		timeradd(&time, &diff_tv, &data->cumulative_time);
		data->is_running = 0;
	}
	
	/* Add time to children time */
	struct timeval time = prev_data->children_time;
	timeradd(&time, &diff_tv, &prev_data->children_time);
	kxprofiler_running_ptracker_data = prev_data;

	KxTrackedMessage *tm = data->tracked_messages;
	while(tm->message_name != message->message_name || tm->slot_holder != slot_holder) {
		if (tm->message_name == NULL) {
			kxptracked_add_tracked_message(tracker, message->message_name, slot_holder);
			return result;
		}
		tm++;
	}
	return result;
}

KxObject * 
kxprofiler_codeblock_activate(KxObject *self, KxObject *target, KxMessage *message)
{

	KxObject *tracker = kxcodeblock_find_literal_by_extension(self, &kxptracker_extension);
	if (tracker == NULL) {
		tracker = kxptracker_new(KXCORE, self);
		kxcodeblock_add_literal(self, tracker);
		REF_REMOVE(tracker);
	}
	KxPTrackerData *prev_data = kxprofiler_running_ptracker_data;
	KxPTrackerData *data = KXPTRACKER_DATA(tracker);
	kxprofiler_running_ptracker_data = data;
	int is_running = data->is_running;
	data->is_running = 1;

	struct timeval tv_start;
	struct timeval tv_end;
	gettimeofday(&tv_start, NULL);

	KxObject *result = kxprofiler_original_codeblock_activate(self, target, message);

	gettimeofday(&tv_end, NULL);

	struct timeval diff_tv;
	timersub(&tv_end, &tv_start, &diff_tv);

	data->counter++;

	if (is_running) {
		/* Add own cumulative time */
		struct timeval time = data->children_time;
		timersub(&time, &diff_tv, &data->children_time);
	} else {
		/* Add own cumulative time */
		struct timeval time = data->cumulative_time;
		timeradd(&time, &diff_tv, &data->cumulative_time);
		data->is_running = 0;
	}
	
	/* Add time to children time */
	struct timeval time = prev_data->children_time;

	/* Add time to children time */
	time = prev_data->children_time;
	timeradd(&time, &diff_tv, &prev_data->children_time);
	kxprofiler_running_ptracker_data = prev_data;

	KxTrackedMessage *tm = data->tracked_messages;
	KxObject *slot_holder = message->slot_holder;
	while(tm->message_name != message->message_name || tm->slot_holder != slot_holder) {
		if (tm->message_name == NULL) {
			kxptracked_add_tracked_message(tracker, message->message_name, slot_holder);
			return result;
		}
		tm++;
	}
	return result;

	//printf("Profiler: Codeblock %s\n", KXSYMBOL_AS_CSTRING(message->message_name));
}

static void kxprofiler_gc_start_callback(void *param, void *data)
{
	++kxprofiler_gc_counter;
	gettimeofday(&kxprofiler_gc_starttime, NULL);
}

static void kxprofiler_gc_end_callback(void *param, void *data)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	struct timeval diff;
	timersub(&tv, &kxprofiler_gc_starttime, &diff);
	struct timeval ct = kxprofiler_gc_cumulative_time;
	timeradd(&ct, &diff, &kxprofiler_gc_cumulative_time);

	/* Add time to children time */
	KxPTrackerData *pdata = kxprofiler_running_ptracker_data;
	struct timeval time = pdata->children_time;
	timeradd(&time, &diff, &pdata->children_time);
}

static void kxprofiler_enable_hooks(KxCore *core)
{
	kxprofiler_original_codeblock_activate = kxcodeblock_extension.activate;
	kxcodeblock_extension.activate = kxprofiler_codeblock_activate;
	kxprofiler_original_cfunction_activate = kxcfunction_extension.activate;
	kxcfunction_extension.activate = kxprofiler_cfunction_activate;

	kxcallback_add(&core->callback_gc_start, kxprofiler_gc_start_callback, NULL);
	kxcallback_add(&core->callback_gc_end, kxprofiler_gc_end_callback, NULL);

}

static void kxprofiler_disable_hooks(KxCore *core)
{
	kxcodeblock_extension.activate = kxprofiler_original_codeblock_activate;
	kxcfunction_extension.activate = kxprofiler_original_cfunction_activate;

	kxcallback_remove(&core->callback_gc_start, kxprofiler_gc_start_callback, NULL);
	kxcallback_remove(&core->callback_gc_end, kxprofiler_gc_end_callback, NULL);

}


static KxObject *
kxprofiler_start(KxObject *self, KxMessage *message)
{
	if (kxprofiler_enabled) {
		KXTHROW_EXCEPTION("Profiler is already running");
	}
	kxprofiler_enabled = 1;
	memset(&kxprofiler_dummy_ptracker_data, 0, sizeof(KxPTrackerData));
	kxprofiler_running_ptracker_data = &kxprofiler_dummy_ptracker_data;
	kxprofiler_enable_hooks(KXCORE);
	kxprofiler_gc_counter = 0;
	timerclear(&kxprofiler_gc_cumulative_time);
	KXRETURN(self);
}

static KxObject *
kxprofiler_stop(KxObject *self, KxMessage *message)
{
	if (!kxprofiler_enabled) {
		KXTHROW_EXCEPTION("Profiler is not started");
	}
	kxprofiler_enabled = 0;
	kxprofiler_disable_hooks(KXCORE);

	List *trackers = list_new();
	List *list;
	int t;
	
	list = kxcore_list_with_all_objects_with_extension(KXCORE, &kxcfunction_extension);
	for (t = 0; t < list->size; t++) {
		KxObject *item = list->items[t];
		KxObject *obj = kxcfunction_objects_find_by_extension(item, &kxptracker_extension);
		if (obj != NULL) {
			REF_ADD(obj);
			kxptracker_remove_from_tracked_object(obj);
			list_append(trackers, obj);
		}
		REF_REMOVE(item);
	}
	list_free(list);

	list = kxcore_list_with_all_objects_with_extension(KXCORE, &kxcodeblock_extension);
	for (t = 0; t < list->size; t++) {
		KxObject *item = list->items[t];
		KxObject *obj = kxcodeblock_find_literal_by_extension(item, &kxptracker_extension);
		if (obj != NULL) {
			REF_ADD(obj);
			kxptracker_remove_from_tracked_object(obj);
			list_append(trackers, obj);
		}
		REF_REMOVE(item);
	}
	list_free(list);


	return KXLIST(trackers);

	//KXRETURN(self);
}

static KxObject *
kxprofiler_gc_counter_method(KxObject *self, KxMessage *message)
{
	return KXINTEGER(kxprofiler_gc_counter);
}

static KxObject *
kxprofiler_gc_cumulative_time_method(KxObject *self, KxMessage *message)
{
	return KXTIMEVALUE(&kxprofiler_gc_cumulative_time);
}


void static
kxprofiler_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"_start",0, kxprofiler_start },
		{"_stop",0, kxprofiler_stop },
		{"gcCounter",0, kxprofiler_gc_counter_method },
		{"gcCumulativeTime",0, kxprofiler_gc_cumulative_time_method },
		{NULL, 0, NULL },
	};
	kxobject_add_methods(self, table);
}
