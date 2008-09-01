#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "kxcompiler.h"
#include "block.h"
#include "kxcomp.h"
#include "../utils/list.h"
#include "../utils/utils.h"
#include "kxconfig.h"


/**
 *	Create bytecode from codeblock
 *	bytecode_mem is set to array of bytecode 
 *	(memory must by manualy freed)
 *  return size of bytecode
 */
static int 
kxc_get_bytecode(KxcBlock *block, char **bytecode_mem) 
{
	int bytecode_size = kxcblock_bytecode_size(block);

	char *bytecode = malloc(bytecode_size);
	char *start = bytecode;
	ALLOCTEST(bytecode);
	
	kxcblock_bytecode_write(block, &bytecode);
	int end_size = bytecode - start;
	if (end_size != bytecode_size) {
		fprintf(stderr,"Internal error, check of bytecode size FAILED (%i - %i)\n",bytecode_size, end_size);
		abort();
	}
	*bytecode_mem = start;
	return bytecode_size;
}

/**
 *   Write header into .kxc file
 *   file position is set after header
 *   return 0 if error occurs, otherwise 1
 */
static int 
kxc_write_header(FILE *file, char *source_filename) 
{
	int magic_size = strlen(KXC_FILE_MAGIC);
	int size = magic_size+2*sizeof(char);
	char *header = malloc(size);

	strcpy(header, KXC_FILE_MAGIC);
	header[magic_size] = KXC_FILE_VERSION_MAJOR;
	header[magic_size+1] = KXC_FILE_VERSION_MINOR;

	if (fwrite(header, size ,1,file) != 1) {
		free(header);
		return 0;
	}
	free(header);

	char len = strlen(source_filename);
	if (fwrite(&len, sizeof(char),1,file) != 1) {
		return 0;
	}


	if (fwrite(source_filename, len,1,file) != 1) {
		return 0;
	}
	return 1;
}

/**
 *   Write bytecode to file (header + bytecode)
 *
 *   Returns:
 *   0 - error
 *   1 - ok
 */ 
static int 
kxc_write_bytecode(FILE *file, char *bytecode, int bytecode_size, char *source_filename) 
{
	if (!kxc_write_header(file, source_filename))
		return 0;

	if (fwrite(&bytecode_size, sizeof(int), 1, file)!=1) {
		return 0;
	}

	if (fwrite(bytecode, bytecode_size, 1, file) != 1) {
		return 0;
	}

	return 1;
}

/**
 *   Compile file
 *   input_filename - filename of source .kx file
 *   output_filename - filename of .kxc file where bytecode will be written
 *   returns 0 if error occurs, 1 if all is ok
 *   errors is saved in kxc_error_list
 */ 
List *
kxc_compile_and_save_file(char *input_filename, char *output_filename,
	int compile_docs, int level_of_optimisation) 
{
	char *bytecode;
	int size;

	List *errors = kxc_compile_file(input_filename, compile_docs, 
		level_of_optimisation, &bytecode, &size);
	if (errors)
		return errors;
	
	FILE *fileout = fopen(output_filename,"w");
	if (!fileout) {
		free(bytecode);
		return kxcomp_one_error_list("Can't open file '%s': %s", output_filename,strerror(errno));
	}

	
	if (!kxc_write_bytecode(fileout, bytecode, size, input_filename)) {
		errors = kxcomp_one_error_list("write failed : %s", strerror(errno));
	}
	fclose(fileout);
	free(bytecode);

	return errors;
}

/**
 *   Compile file
 *   input_filename - filename of source .kx file
 *   bytecode - pointer at bytecode, pointer will be set at bytecode array
 *   returns NULL if compilation is successfull otherwise list with error string is returned
 */ 
List * 
kxc_compile_file(char *input_filename, int compile_docs, 
	int level_of_optimisation, char **bytecode, int *size) 
{
	KxcBlock *block;
	List *errors;
	errors = kxcomp_compile_file(input_filename, compile_docs, 
		level_of_optimisation, &block);
	if (errors) {
		if (block)
			kxcblock_free(block);
		return errors;
	}
	*size = kxc_get_bytecode(block,bytecode);
	kxcblock_free(block);
	return NULL;
	
}

/**
 *   Compile string
 *   code_string - Kreatrix code
 *   filename - Only for error messages
 *   bytecode - pointer at bytecode, pointer will be set at bytecode array
 *   returns NULL if compilation is successfull otherwise list with error string is returned
 */ 
List * 
kxc_compile_string(char *code_string, char *filename, int compile_docs, 
	int level_of_optimisation, char **bytecode, int *size) 
{
	KxcBlock *block;
	List *errors = kxcomp_compile_string(code_string, filename, 
		compile_docs, level_of_optimisation, &block);

	if (errors) {
		if (block)
			kxcblock_free(block);
		return errors;
	}
	*size = kxc_get_bytecode(block,bytecode);
	kxcblock_free(block);
	return NULL;

}


