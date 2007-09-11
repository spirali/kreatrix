
#ifndef __DICTIONARY_H
#define __DICTIONARY_H

//tmp
#include "list.h"
#include "utils.h"

typedef struct Dictionary Dictionary;



struct Dictionary {
	List *keys;
	List *values;
};

Dictionary * dictionary_new();
void dictionary_free(Dictionary *self);


void dictionary_clean(Dictionary *self);
void dictionary_add(Dictionary *self, void *key, void *value);
void *dictionary_get(Dictionary *self, void *key);
void dictionary_remove(Dictionary *self, void *key);

void dictionary_add_with_compare(Dictionary *self, void *key, void *value, CompareFcn *fcn);
void *dictionary_get_with_compare(Dictionary *self, void *key, CompareFcn *fcn);
void dictionary_remove_with_compare(Dictionary *self, void *key, CompareFcn *fcn);


void dictionary_foreach_value(Dictionary *self, ForeachFcn *fcn);
void dictionary_foreach_key(Dictionary *self, ForeachFcn *fcn);
int dictionary_size(Dictionary *self);
Dictionary *dictionary_copy(Dictionary *self);


List *dictionary_keys_as_list(Dictionary *self);
List *dictionary_values_as_list(Dictionary *self);


#endif // __DICTIONARY_H
