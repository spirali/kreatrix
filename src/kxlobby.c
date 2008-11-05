/*
   kxlobby.c
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
	
/*KXobject Base Lobby
  [Singletons] Global space
 */

#include <stdlib.h>
#include <stdio.h>

#include "kxlobby.h"
#include "kxobject.h"
#include "kxcore.h"
#include "compiler/kxcompiler.h"
#include "kxvm.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxbytecode.h"
#include "kxcodeblock.h"
#include "kxcompiler.h"
#include "kxcompile_utils.h"
#include "kxglobals.h"

//static KxObjectExtension ext;

char * create_kxc_filename(char *kxfilename);


// TODO: use standard function for do_file
//

/*KXdoc doFile: filename
[Actions] compile and run file
*/
static KxObject *
kxlobby_do_file(KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_CSTRING(param,0);
	return kxcompile_do_file(param, KXCORE->lobby, message->message_name, kx_doc_flag);

}

KxObject *kxlobby_new(KxCore *core) 
{

	KxObject *self = kxcore_clone_base_object(core);

	/** TMP */

	/*kxobjectext_init(&ext);
	ext.type_name = "Lobby";

	self->extension = &ext;*/


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Lobby"), self);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Object"), core->base_object);

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Integer"), 
		kxcore_get_basic_prototype(core,KXPROTO_INTEGER));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Float"), 
		kxcore_get_basic_prototype(core,KXPROTO_FLOAT));


	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"String"), 
		kxcore_get_basic_prototype(core,KXPROTO_STRING));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Symbol"), 
		kxcore_get_basic_prototype(core,KXPROTO_SYMBOL));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Exception"), 
		kxcore_get_basic_prototype(core,KXPROTO_EXCEPTION));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"List"), 
		kxcore_get_basic_prototype(core,KXPROTO_LIST));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"File"), 
		kxcore_get_basic_prototype(core,KXPROTO_FILE));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Character"), 
		kxcore_get_basic_prototype(core,KXPROTO_CHARACTER));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Dictionary"), 
		kxcore_get_basic_prototype(core,KXPROTO_DICTIONARY));
	
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Set"), 
		kxcore_get_basic_prototype(core,KXPROTO_SET));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Module"), 
		kxcore_get_basic_prototype(core,KXPROTO_MODULE));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"CFunction"), 
		kxcore_get_basic_prototype(core,KXPROTO_CFUNCTION));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"CodeBlock"), 
		kxcore_get_basic_prototype(core,KXPROTO_CODEBLOCK));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"ByteArray"), 
		kxcore_get_basic_prototype(core,KXPROTO_BYTEARRAY));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Association"), 
		kxcore_get_basic_prototype(core,KXPROTO_ASSOCIATION));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"ScopedBlock"), 
		kxcore_get_basic_prototype(core,KXPROTO_SCOPEDBLOCK));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Array2d"), 
		kxcore_get_basic_prototype(core,KXPROTO_ARRAY2D));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"LocalContext"), 
		kxcore_get_basic_prototype(core,KXPROTO_LOCALCONTEXT));


	kxobject_set_slot_no_ref2(self,kxcore_get_symbol(core,"VM"), kxvm_new(core));

	kxobject_set_slot_no_ref2(self,kxcore_get_symbol(core,"Compiler"), kxcompiler_new_prototype(core));

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"Lobby"), self);

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"true"), core->object_true);

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"false"), core->object_false);
	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"nil"), core->object_nil);

/*KXdoc stdout
 [I/O] Standard output
*/

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stdout"), core->object_stdout);

/*KXdoc stdin
 [I/O] Standard input
*/

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stdin"), core->object_stdin);

/*KXdoc stderr
 [I/O] Standard error output
*/

	kxobject_set_slot_no_ref(self,kxcore_get_symbol(core,"stderr"), core->object_stderr);
	
	KxMethodTable table[] = {
		{"doFile:",1, kxlobby_do_file },
		{NULL,0,NULL}
	};
	kxobject_add_methods(self, table);





	return self;
}
