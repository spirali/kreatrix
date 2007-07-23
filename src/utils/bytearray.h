
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
