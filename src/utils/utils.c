
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
		free(full_path);
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
		out = malloc(sizeof(char) * (path_size + filename_size+1));
		next = path_size;
	} else {
		out = malloc(sizeof(char) * (path_size + filename_size+2));
		out[path_size] = '/';
		next = path_size + 1;
	}
	memcpy(out, path, path_size);
	strcpy(out+next, filename);
	return out;
}


