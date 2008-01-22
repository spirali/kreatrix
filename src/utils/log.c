#include "log.h"

#ifdef KX_LOG

#include <stdio.h>
#include <stdlib.h>

static FILE *kx_log = NULL;

void kx_log_init() 
{
	kx_log = fopen("kreatrix.log","w");
	if (kx_log == NULL) {
		fprintf(stderr,"Error! \"kreatrix\".log cannot be created!");
		abort();
	}
}

void kx_log_write_raw(char *string)
{
	if (kx_log) {
		fprintf(kx_log,"%s\n", string);
	}
}


void kx_log_write(char *action, void *id)
{
	if (kx_log) {
		fprintf(kx_log,"%s:%p\n", action, id);
	}
}

void kx_log_write1(char *action, void *id, char *param)
{
	if (kx_log) {
		fprintf(kx_log,"%s:%p:%s\n", action, id, param);
	}
}

void * kx_log_malloc(int size, char *file, int linenum)
{
	if (kx_log) {
		fprintf(kx_log,"MA:%i:%s#%i\n", size, file, linenum);
	}
	return malloc(size);
}

void * kx_log_calloc(int count, int size, char *file, int linenum)
{
	if (kx_log) {
		fprintf(kx_log,"CA:%i:%s#%i\n", size * count, file, linenum);
	}
	return calloc(size, count);
}


#endif // KX_LOG

