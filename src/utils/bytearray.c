/*
   bytearray.c
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
#include <stdio.h>

#include "bytearray.h"
#include "utils.h"
#include "string.h"

static void bytearray_resize(ByteArray *self, int new_size);


ByteArray * 
bytearray_new_with(int size) {
	ByteArray *self = kxcalloc(1,sizeof(ByteArray));
	ALLOCTEST(self);

	bytearray_resize(self, size);
	self->size = 0;
	return self;
}

ByteArray * 
bytearray_new_from_string(char *string) {
	return bytearray_new_from_data(string, strlen(string));
}


ByteArray *
bytearray_new_set(void *data, int size)
{
	ByteArray *self = kxcalloc(1,sizeof(ByteArray));
	ALLOCTEST(self);
	self->size = size;
	self->array = data;
	return self;
}

void
bytearray_set_size(ByteArray *self, int newsize)
{
	int old_size = self->size;
	self->size = newsize;
	bytearray_resize(self, newsize);
	if (old_size < newsize)
	   memset(self->array+old_size, 0, newsize - old_size);
}

int 
bytearray_at(ByteArray *self, int pos)
{
	return self->array[pos];
}

void
bytearray_at_put(ByteArray *self, int pos, int value)
{
	self->array[pos] = value;
}

ByteArray *
bytearray_new_from_data(void *data, int size)
{
	ByteArray *self = bytearray_new_with(size);
	self->size = size;
	memcpy(self->array, data, size);
	return self;

}

ByteArray * 
bytearray_new() {
	return bytearray_new_with(BYTEARRAY_DEFAULT_CAPACITY);
}

ByteArray *
bytearray_free(ByteArray *self) 
{
	if (self->array) 
		kxfree(self->array);
	kxfree(self);
}


ByteArray *bytearray_copy(ByteArray *self) 
{
	ByteArray *copy = bytearray_new(self->size);
	copy->size = self->size;
	bytearray_append(copy, self->array, self->size);
	return copy;

}

static void 
bytearray_resize(ByteArray *self, int new_size) 
{
	self->array = (char*)kxrealloc(self->array, new_size);
	ALLOCTEST(self->array);
	self->capacity = new_size;
}

void
bytearray_append(ByteArray *self, char *data, int size) 
{
	int old_size = self->size;
	self->size += size;
	if (self->size > self->capacity) {
		bytearray_resize(self, MAX(self->size,self->capacity*2));
	}
	
	memcpy(self->array + old_size, data, size);
}

void 
bytearray_append_zero(ByteArray *self, int size) 
{
	int old_size = self->size;
	self->size += size;
	if (self->size > self->capacity) {
		bytearray_resize(self, MAX(self->size,self->capacity*2));
	}
	bzero(self->array+old_size, size);
}

void
bytearray_append_string(ByteArray *self, char *string) 
{
	int length = strlen(string);
	bytearray_append(self, string, length);
}

void
bytearray_append_int(ByteArray *self, int integer) 
{
	bytearray_append(self, (char*) &integer, sizeof(int));
}


int bytearray_as_int32(ByteArray *self, int pos)
{
	int p = self->size - pos;
	char *array = self->array + pos;
	if (p < 4) {
	/*	int res = 0;
		switch(p) {
			case 3: res = (*(array+2))*256*256;
			case 2: res = res+(*(array+1))*256;
			case 1: res = res+(*(array)); return res;
			default: return 0;
		}*/
		return 0;
	} else {
		int *p = (int*) (self->array);
		return *p;
	}
}
