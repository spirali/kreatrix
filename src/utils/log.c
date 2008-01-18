#include "log.h"

#ifdef KX_LOG

#include <stdio.h>
#include <stdlib.h>

static FILE *kx_log = NULL;

void kx_log_init() 
{
	kx_log = fopen("kreatrix.log","w");
	if (kx_log == NULL) {
		fprintf(stderr,"Warning! \"kreatrix\".log cannot be created!");
	}
}

void kx_log_write_raw(char *string)
{
	fprintf(kx_log,"%s\n", string);
}


void kx_log_write(char *action, void *id)
{
	fprintf(kx_log,"%s:%p\n", action, id);
}

void kx_log_write1(char *action, void *id, char *param)
{
	fprintf(kx_log,"%s:%p:%s\n", action, id, param);
}

void * kx_log_malloc(int size, char *file, int linenum)
{
	fprintf(kx_log,"MA:%i:%s#%i\n", size, file, linenum);
	return malloc(size);
}

#endif // KX_LOG

