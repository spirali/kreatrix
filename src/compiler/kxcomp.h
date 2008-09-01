#ifndef __KREATRIX_KXCOMP_H
#define __KREATRIX_KXCOMP_H

#include "kxparser.h"
#include "../utils/list.h"
#include "block.h"
typedef struct KxCompiler KxCompiler;

struct KxCompiler {
	int token;
	KxParser *parser;
	
	KxcBlock *block;

	int resend;
	int compile_docs;
	int level_of_optimisation;

	char *filename;
	List *errors;
	
};

List* kxcomp_compile_file(char *filename, int compile_docs, 
	int level_of_optimisation, KxcBlock **block);
List *kxcomp_compile_string(char *string, char *filename, int compile_docs, 
	int level_of_optimisation, KxcBlock **block);

List *kxcomp_one_error_list(char *form, ...);



#endif // __KREATRIX_KXCOMP_H
