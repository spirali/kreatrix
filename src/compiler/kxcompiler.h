
#ifndef __KXCOMPILER_H
#define __KXCOMPILER_H

#include "../utils/list.h"


#define KXC_FILE_MAGIC "\1KXcode"
#define KXC_FILE_VERSION_MAJOR 0
#define KXC_FILE_VERSION_MINOR 1


struct List;

List *kxc_compile_and_save_file(char *input_filename, char *output_filename, 
	int compile_docs, int level_of_optimisation);

List *kxc_compile_file(char *input_filename, int compile_docs, int level_of_optimisation,
	char **bytecode, int *size);

List *kxc_compile_string(char *code_string, char *filename, int compile_docs, 
	int level_of_optimisation, char **bytecode, int *size);

#endif // __KXCOMPILER_H
