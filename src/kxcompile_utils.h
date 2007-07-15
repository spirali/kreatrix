/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#ifndef __KXCOMPILE_UTILS_H
#define __KXCOMPILE_UTILS_H

#include "kxobject_struct.h"
#include "kxsymbol.h"


KxObject * kxcompile_do_file(char *filename, KxObject *target, KxSymbol *message_name, int doc_flag );

KxObject * kxcompile_run_bytecode(char *bytecode,  KxObject *target, char *source_filename, KxSymbol *message_name);


#endif
