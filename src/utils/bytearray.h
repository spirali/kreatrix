/*
   bytearray.h
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
	

#ifndef __BYTEARRAY_H
#define __BYTEARRAY_H

typedef struct ByteArray ByteArray;

#define BYTEARRAY_DEFAULT_CAPACITY 4

struct ByteArray {
	char *array;
	int size;
	int capacity;
};

ByteArray *bytearray_new_with(int size);
ByteArray *bytearray_new_from_data(void *data, int size);
ByteArray *bytearray_new();
ByteArray *bytearray_copy(ByteArray *self);
ByteArray *bytearray_new_from_string(char *string);
ByteArray *bytearray_new_set(void *data, int size);
void bytearray_set_size(ByteArray *self, int newsize);


int bytearray_at(ByteArray *self, int pos);
void bytearray_at_put(ByteArray *self, int pos, int value);

int bytearray_as_int32(ByteArray *self, int pos);

//void bytearray_resize(ByteArray *self, int new_size);
ByteArray *bytearray_free(ByteArray *self);
void bytearray_append(ByteArray *self, char *data, int size);


void bytearray_append_int(ByteArray *self, int integer);
void bytearray_append(ByteArray *self, char *data, int size);
void bytearray_append_zero(ByteArray *self, int size);


#endif // __BYTEARRAY_H
