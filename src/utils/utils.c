/*
   utils.c
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
	

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "list.h"


void 
utils_out_of_memory(char *filename, int lineno)
{
    fprintf(stderr,"Oops! Out of memory (%s:%i)\n",filename, lineno);
    exit(-1);

}

int
utils_file_exist(char *filename)
{
//	printf("fileexist = %s\n", filename);
	struct stat buf;

	if (stat(filename, &buf) != 0) 
		return 0;
	
	return S_ISREG(buf.st_mode);
}

int 
utils_file_exists_in_paths(struct List *list, char *filename)
{
	int t;
	for (t=0;t<list->size;t++) {
		char *full_path = utils_path_join((char*)list->items[t],filename);
//		printf("path test: %s\n", full_path);
		int ret = utils_file_exist(full_path);
		kxfree(full_path);
		if (ret)
			return t;
	}
	return -1;
}


// Dont forget free returned string
char *
utils_path_join(char *path, char *filename) 
{
	int path_size = strlen(path);
	int filename_size = strlen(filename);

	int next;
	char *out;
	if (path[path_size-1] == '/') {
		out = kxmalloc(sizeof(char) * (path_size + filename_size+1));
		ALLOCTEST(out);
		next = path_size;
	} else {
		out = kxmalloc(sizeof(char) * (path_size + filename_size+2));
		ALLOCTEST(out);
		out[path_size] = '/';
		next = path_size + 1;
	}
	memcpy(out, path, path_size);
	strcpy(out+next, filename);
	return out;
}


