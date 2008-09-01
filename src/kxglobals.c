#include <kxconfig.h>
#include <stdio.h>
#include <stdlib.h>

#include "kxglobals.h"


int kx_verbose = 0;
int kx_doc_flag = 0;
int kx_level_of_optimisation = 2;

#ifdef KX_THREADS_SUPPORT
	int kx_threads_support = 1;
#else
	int kx_threads_support = 0;
#endif //KX_THREADS_SUPPORT
