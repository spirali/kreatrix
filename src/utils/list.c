/*
   list.c
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
	

#include "list.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Create a new list
List* 
list_new_size(int size) 
{
    List *list = kxmalloc(sizeof(List));
    ALLOCTEST(list);
    
    list->capacity = MAX(size,LIST_DEFAULT_CAPACITY);
    list->size = size;
    list->items = kxcalloc(list->capacity, sizeof(void*));
    ALLOCTEST(list->items);
   
    return list;
}

List*
list_new() 
{
	return list_new_size(0);
}


/* Put item into list on position 
   without range check */
void 
list_put(List *list, int position, void *item) 
{
    list->items[position] = item;
}

/* Get item from list 
   without range check */
void *
list_get(List *list, int position) 
{
    return list->items[position];
}

void
list_set_size(List *list, int size)
{
	
	list->size = size;
	list->capacity = MAX(size, LIST_DEFAULT_CAPACITY);

	list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
	ALLOCTEST(list->items);


}

/*
	Add item at the end of list
	capacity is expanded if it is too small
*/
void
list_append(List *list, void *data) 
{
	if (list->size == list->capacity) 
	{
		list->capacity <<= 1;
		list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
		ALLOCTEST(list->items);
	} 
 	list->items[list->size] = data;
	list->size++;
}

/*
	Add item at the position in list 
	(allowed positions is 0 .. size, no control in this function)
	capacity is expanded if it is too small
*/
void
list_insert_at(List *list, void *data, int pos) 
{
	if (list->size == list->capacity) 
	{
		list->capacity <<= 1;
		list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
		ALLOCTEST(list->items);
	}
	void *p = list->items + pos;
	memmove(p+sizeof(void*),p, (list->size - pos)*sizeof(void*));
 	list->items[pos] = data;
	list->size++;

}

/*

*/
void
list_join(List *list, List *list2) 
{
	int old_size = list->size;

	list->size += list2->size;

	if (list->size >= list->capacity) 
	{
		list->capacity <<= 1;
		list->capacity = MAX(list->capacity, list->size);
		list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
		ALLOCTEST(list->items);
	}

	// TODO: use memcpy
	int t;
	for (t=0;t<list2->size;t++) {
	 	list->items[old_size+t] = list2->items[t];
	}
}


/*
	Remove item from list on position
	without range check
	without reducing capacity
*/
void 
list_fast_remove(List *list, int position) 
{
	list->size--;
	int t;
	for (t=position;t<list->size;t++) {
		list->items[t] = list->items[t+1];
	}
}



/*
	Remove item from list on position
	without range check
	if size is lesser than half of capacity, capacity is reduced
*/
void 
list_remove(List *list, int position) 
{
	list->size--;
	int t;
	for (t=position;t<list->size;t++) {
		list->items[t] = list->items[t+1];
	}

	if (list->size < list->capacity/2) {
		if (list->capacity <= LIST_DEFAULT_CAPACITY) {
			return;
		}
		list->capacity >>= 1;


		list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
		ALLOCTEST(list->items);
	}
}

void
list_remove_first(List *list, void *data) 
{
	int t;
	for (t=0;t<list->size;t++) {
		if (list->items[t] == data) {
			list_remove(list, t);
			return;
		}
	}
}
  
/**
    Remove all items
*/
void
list_clean(List* list) 
{
	list->capacity = LIST_DEFAULT_CAPACITY;
	list->size = 0;

	list->items = kxrealloc(list->items,sizeof(void*) * LIST_DEFAULT_CAPACITY);
}

// Free list
void
list_free(List *list)
{
    kxfree(list->items);
    kxfree(list);
}

// Free list and all pointers in list
void 
list_free_all(List *list) 
{
	int t;
	for (t=0;t<list->size;t++) {
		kxfree(list->items[t]);
	}
	list_free(list);
}

void 
list_foreach(List *list, ListForeachFcn *fcn) 
{
	int t;
	for (t=0;t<list->size;t++)
		(*fcn)(list->items[t]);

}

/*  Remove last item, if list is empty NULL is returned
 *  if size is lesser than half of capacity, capacity is reduced
 *  return last item
 */
void *
list_pop(List *list) 
{
	if (list->size == 0) 
		return NULL;
	list->size--;

	void *last_item = list->items[list->size];
	
	if (list->size < list->capacity/2) {
		if (list->capacity <= LIST_DEFAULT_CAPACITY) {
			return last_item;
		}
		list->capacity >>= 1;

		list->items = kxrealloc(list->items,sizeof(void*)*list->capacity);
		ALLOCTEST(list->items);
	}

	return last_item;
}

void *
list_fast_pop(List *list)
{
	if (list->size == 0)
		return NULL;
	list->size--;
	return list->items[list->size];
}

/*  Remove last item, if list is empty NULL is returned */
void *
list_top(List *list) 
{
	if (list->size == 0) 
		return NULL;

	return list->items[list->size-1];
}

List *
list_copy(List *list)
{
	List *newlist = list_new_size(list->size);
	int t;
	for (t=0;t<list->size;t++) {
		newlist->items[t] = list->items[t];
	}
	return newlist;
}


int 
list_size(List *list)
{
	return list->size;
}

typedef int(qsort_fcn)(const void *, const void *);

int
list_sort(List *list, CompareFcn *compare) 
{
	qsort(list->items, list->size, sizeof(void*), (qsort_fcn*)compare);
}

void
list_print_strings(List *list)
{
	int t;
	for (t=0;t<list->size;t++) {
		printf("%s\n", list->items[t]);
	}
}

void
list_collapse(List *list)
{
	// TODO: implement collapse
}
