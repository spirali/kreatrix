/*
   log.c
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
	
#include "log.h"

#ifdef KX_LOG

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static FILE *kx_log = NULL;

unsigned int kx_log_time = 0; 
unsigned int kx_log_start_time = 0;

static unsigned int
_kx_log_get_time()
{
	struct timeval tv;
	errno = 0;
	if (gettimeofday(&tv, NULL)) {
		return kx_log_time;
	}
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static void write_time_mark()
{
	unsigned int tm = _kx_log_get_time();
	if (tm > kx_log_time) {
		kx_log_time = tm;
		fprintf(kx_log,"TM:%u\n", tm - kx_log_start_time);
	}
}

static void 
kx_log_at_exit() 
{
	unsigned int tm = _kx_log_get_time();
	if (tm <= kx_log_time) {
		tm++;
	}
	fprintf(kx_log,"TM:%u\n", tm - kx_log_start_time);
}


void kx_log_init() 
{
	kx_log_start_time = _kx_log_get_time();
	kx_log = fopen("kreatrix.log","w");
	if (kx_log == NULL) {
		fprintf(stderr,"Error! \"kreatrix\".log cannot be created!");
		abort();
	}
	write_time_mark();
	atexit(kx_log_at_exit);
}

void kx_log_write_raw(char *string)
{
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"%s\n", string);
	}
}


void kx_log_write(char *action, void *id)
{
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"%s:%p\n", action, id);
	}
}

void kx_log_write1(char *action, void *id, char *param)
{
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"%s:%p:%s\n", action, id, param);
	}
}

void kx_log_write_id_i(char *action, void *id, int param)
{
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"%s:%p:%i\n", action, id, param);
	}
}


void * kx_log_malloc(int size, char *file, int linenum)
{
	void *ptr = malloc(size);
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"MA:%i:%p:%s#%i\n", size, ptr, file, linenum);
	}
	return ptr;
}

void * kx_log_calloc(int count, int size, char *file, int linenum)
{
	void *ptr = calloc(count, size);
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"CA:%i:%p:%s#%i\n", size * count, ptr, file, linenum);
	}
	return ptr;
}

void * kx_log_realloc(void *ptr, int new_size)
{
	void *n = realloc(ptr, new_size);
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"RA:%p:%i:%p\n", ptr, new_size, n);
	}
	return n;
}

void kx_log_free(void *ptr)
{
	if (kx_log) {
		write_time_mark();
		fprintf(kx_log,"FR:%p\n", ptr);
	}
	free(ptr);
}





#endif // KX_LOG

