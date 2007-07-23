/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

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
	char *str = malloc(sizeof(char) * len);
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
	}
	
	kxmodules_path = malloc(sizeof(char*) * (size+1));

	int pos = 0;

	if (env != NULL) {
		kxmodules_path[pos++] = env;
	}

	kxmodules_path[pos++] = PKGLIBDIR;

	kxmodules_path[pos] = NULL;
	return 1;
}
