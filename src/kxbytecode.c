/*
   kxbytecode.c
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
	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kxbytecode.h"
#include "compiler/kxcompiler.h"
#include "utils/utils.h"

static int
check_header(FILE *file)
{
	int header_size = strlen(KXC_FILE_MAGIC)+2*sizeof(char);

	char header[header_size];

	if (fread(header,header_size,1,file) != 1) {
		fprintf(stderr,"Invalid header (file is too small)\n");
		return 0;
	}

	if (memcmp(KXC_FILE_MAGIC, header, strlen(KXC_FILE_MAGIC))) {
		fprintf(stderr,"Invalid header\n");
		return 0;
	}

	return 1;
}

static int
bytecode_read_source_filename(FILE *file, char **source_filename)
{
	char filename_len; 
	if (fread(&filename_len,sizeof(filename_len),1,file) != 1) {
		perror("bytecode_load_from_file");
		fclose(file);
		return 0;
	}
	*source_filename = kxmalloc(filename_len+1);
	ALLOCTEST(*source_filename);

	if (fread(*source_filename,filename_len,1,file) != 1) {
		perror("bytecode_load_from_file");
		fclose(file);
		return 0;
	}
	(*source_filename)[(int)filename_len] = 0;
	return 1;
}

char *
bytecode_load_from_file(char *filename, int *bytecode_size, char **source_filename)
{
	FILE *file = fopen(filename,"r");
	if (!file) {
		perror("bytecode_load_from_file");
		return NULL;
	}

	if (!check_header(file)) {
		fclose(file);
		return NULL;
	}

	if (!bytecode_read_source_filename(file, source_filename)) {
		fclose(file);
		return NULL;
	}
	//printf("source_filename = %s\n",*source_filename);

	int size;
	
	if (fread(&size,sizeof(size),1,file) != 1) {
		perror("bytecode_load_from_file");
		fclose(file);
		return NULL;
	}

	char *bytecode = kxmalloc(size);
	ALLOCTEST(bytecode);

	if (fread(bytecode,size,1,file) != 1) {
		perror("bytecode_load_from_file");
		fclose(file);
		return NULL;
	}

	if (bytecode_size)
		*bytecode_size = size;
	
	fclose(file);

	return bytecode;
}
