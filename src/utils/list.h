
#ifndef __LIST_H
#define __LIST_H

#define LIST_DEFAULT_CAPACITY 2
#include "utils.h"

typedef struct List List;

// Structure which holds List
struct List {
    // Array of items
    void **items;
    // Number of used items in array
    int size;
    // Capacity of array
    int capacity;
};

#define LIST_FOREACH(list) int _index; List *_list = list; for (_index = 0; _index < _list->size; _index++) 
#define LIST_EACH _list->items[_index]


typedef void(ListForeachFcn)(void*);

List* list_new_size(int size);
List* list_new();
void list_free(List *list);
void list_free_all(List *list);
List *list_copy(List *list);
void list_set_size(List *list, int size);
void list_insert_at(List *list, void *data, int pos);


void list_put(List *list, int position, void *item);
void *list_get(List *list, int position);
void list_append(List *list, void *data);
void list_fast_remove(List *list, int position);
void list_remove(List *list, int position);
void list_remove_first(List *list, void *data);

void list_clean(List* list);
void list_foreach(List *list, ListForeachFcn *fcn);
void * list_pop(List *list);
void * list_top(List *list);
void *list_fast_pop(List *list);


int list_size(List *list);

void list_join(List *list, List *list2);
int list_sort(List *list, CompareFcn *compare);

void list_print_strings(List *list);

void list_collapse(List *list);

#endif // __LIST_H
