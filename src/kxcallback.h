/*
   kxcallback.h
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

#ifndef __KXCALLBACK_H

typedef struct KxCallback KxCallback;

typedef void(KxCallbackFcn)(void *param, void *data);

struct KxCallback {
	KxCallbackFcn *function;
	void *data;
};

void kxcallback_init(KxCallback **callback);
void kxcallback_add(KxCallback **callback, KxCallbackFcn *function, void *data);
void kxcallback_remove(KxCallback **callback, KxCallbackFcn *function, void *data);
void kxcallback_call(KxCallback *callback, void *param);
void kxcallback_free(KxCallback **callback);

#endif // __KXCALLBACK_H
