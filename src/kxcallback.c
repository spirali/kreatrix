/*
   kxcallback.c
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

#include "kxcallback.h"
#include "kxglobals.h"
#include "utils/utils.h"
#include <stdlib.h>

void kxcallback_init(KxCallback **callback)
{
	*callback = NULL;
}

void kxcallback_free(KxCallback **callback)
{
	if (*callback) {
		kxfree(*callback);
		*callback = NULL;
	}
}

void kxcallback_add(KxCallback **callback, KxCallbackFcn *function, void *data)
{
	size_t size = 2;
	if (*callback != NULL) {
		KxCallback *c = *callback;
		do {
			size++;
			c++;
		} while(c->function);
	}
	KxCallback *c = kxrealloc(*callback, size * sizeof(KxCallback));
	c[size - 1].function = NULL;
	c[size - 2].function = function;
	c[size - 2].data = data;
	*callback = c;
}

void kxcallback_remove(KxCallback **callback, KxCallbackFcn *function, void *data)
{
	int size = 0;
	int id = -1;
	if (*callback != NULL) {
		KxCallback *c = *callback;
		do {
			if (c->function == function && c->data == data) {
				id = size;
			}
			size++;
			c++;
		} while(c->function);
	}

	if (id == -1) {
		return;
	}

	if (size == 1) {
		kxcallback_free(callback);
		return;
	}

	KxCallback *c = *callback;
	c[id] = c[size - 1];
	c[size - 1].function = NULL;
}

void kxcallback_call(KxCallback *callback, void *param)
{
	if (callback == NULL)
		return;
	
	KxCallback *c = callback;
	do {
		c->function(param, c->data);
		c++;
	} while(c->function);
}

