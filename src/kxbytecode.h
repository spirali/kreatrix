/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


#ifndef __KXBYTECODE_H
#define __KXBYTECODE_H


char *
bytecode_load_from_file(char *filename, int *bytecode_size, char **source_filename);

void
kxc_reset_error_list();


#endif
