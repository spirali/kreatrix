/*
   utils.h
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
	


#ifndef __UTILS_H
#define __UTILS_H

#include "log.h"

#define ALLOCTEST(x) if((!x)) utils_out_of_memory(__FILE__,__LINE__)

#ifndef MAX
#define MAX(x,y) (x)>(y)?(x):(y)
#endif // #ifndef MAX

#ifndef MIN
#define MIN(x,y) (x)<(y)?(x):(y)
#endif // #ifndef MIN

//#define PDEBUG printf
#define PDEBUG  if(0) printf 



typedef void(ForeachFcn)(void *);
typedef void(ForeachPairFcn)(void *);
typedef int(CompareFcn)(void*, void *);

void utils_out_of_memory(char *filename, int lineno);

int utils_file_exist(char *filename);

char *utils_path_join(char *path, char *filename);

struct List;

int utils_file_exists_in_paths(struct List *list, char *filename);


#endif // __UTILS_H
