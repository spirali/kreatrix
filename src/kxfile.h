/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

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
