/*
   kxfile.h
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
	
#ifndef __KXFILE_H
#define __KXFILE_H

#include <stdio.h>
#include "kxobject_struct.h"
#include "kxcore.h"

typedef struct KxObject KxFile;

typedef struct KxFileData KxFileData;

#define KXFILE_FLAG_READ 1
#define KXFILE_FLAG_WRITE 2

#define KXFILE_DATA(kxfile) (KxFileData*) kxfile->data.ptr

struct KxFileData {
	FILE *file;
	char *filename;
	int flag;
};

void kxfile_init_extension();

KxObject * kxfile_new_prototype(KxCore *core);

KxFile *kxfile_stdout(KxCore *core);
KxFile *kxfile_stdin(KxCore *core);
KxFile *kxfile_stderr(KxCore *core);

#endif
