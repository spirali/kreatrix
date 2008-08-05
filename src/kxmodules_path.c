/*
   kxmodules_path.c
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
#include <string.h>
#include <stdlib.h>
#include "kxmodules_path.h"
#include "utils/utils.h"


char **kxmodules_path = NULL;
/*
static char *
path_add(char *path1, char *path2)
{
	int len = strlen(path1) + strlen(path2) + 2;
	char *str = kxmalloc(sizeof(char) * len);
	ALLOCTEST(str);

	if (path1[strlen(path2)-1] == '/') {
		strcpy(str,path1);
		strcpy(str+strlen(path1),path2);
	} else {
		strcpy(str,path1);
		strcpy(str+1+strlen(path1),path2);
		str[strlen(path1)] = '/';
	}
	return str;
}*/

int 
kxmodules_path_init() 
{
	char *env = getenv("KREATRIX_MODULES_PATH");
	int size = 1;

	if (env != NULL) {
		size++;
		char *str = env;
		while( (*str) != 0) {
			if ( (*str) == ':')
				size++;
			str++;
		}
	}
	
	kxmodules_path = kxmalloc(sizeof(char*) * (size+1));

	int pos = 0;

	if (env != NULL) {
		char *str = env;
		char *end = env;
		while( (*end) != 0) {
			if (*end == ':') {
				*end = 0;
				
				kxmodules_path[pos++] = strdup(str);
				
				*end = ':';
				str = end + 1;
			} 
			end++;
			
		}

		if (str == env) {
			kxmodules_path[pos++] = env;
		} else {
			if (str != 0) {
				kxmodules_path[pos++] = strdup(str);
			}
		}
	}

	kxmodules_path[pos++] = PKGLIBDIR;

	kxmodules_path[pos] = NULL;
	return 1;
}
