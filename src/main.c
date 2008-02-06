/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>
#include <ctype.h>
#include <kxconfig.h>

#include "utils/list.h"
#include "utils/utils.h"
#include "compiler/kxcompiler.h"
#include "kxcompile_utils.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxstack.h"
#include "kxcore.h"
#include "kxcodeblock.h"
#include "kxactivation.h"
#include "kxscopedblock.h"
#include "kxobject.h"
#include "kxbytecode.h"
#include "kxgc.h"
#include "kxlist.h"
#include "kxfile.h"
#include "kxbytearray.h"
#include "kxcharacter.h"
#include "kxdictionary.h"
#include "vminit/kxvminit.h"
#include "kxdynlib.h"
#include "kxmodule.h"
#include "kxfloat.h"
#include "kxset.h"
#include "kxglobals.h"
#include "kxmodules_path.h"
#include "kxarray2d.h"

int kx_doc_flag = 0;
int kx_interactive = 0;
int kx_compile_flag = 0;
static char *evaluate_commands = NULL;

char * create_kxc_filename(char *kxfilename) 
{
	int size = strlen(kxfilename);
	
	char *kxcfilename;

	if (size> 3 && !strcmp(kxfilename+size-3,".kx")) {
		kxcfilename = kxmalloc(size+2);
		ALLOCTEST(kxcfilename);
		strcpy(kxcfilename, kxfilename);
		kxcfilename[size] = 'c';
		kxcfilename[size+1] = '\0';
	} else {
		kxcfilename = kxmalloc(size+5);
		ALLOCTEST(kxcfilename);
		strcpy(kxcfilename, kxfilename);
		strcpy(kxcfilename+size, ".kxc");
	}

	return kxcfilename;
}

static void 
extensions_init()
{
	kxsymbol_init_extension();
	kxcfunction_init_extenstion();
	kxinteger_init_extension();
	kxstring_init_extension();
	kxcodeblock_init_extension();
	kxscopedblock_init_extension();
	kxlist_init_extension();
	kxfile_init_extension();
	kxbytearray_init_extension();
	kxcharacter_init_extension();
	kxdictionary_init_extension();
	kxdynlib_init_extension();
	kxfloat_init_extension();
	kxmodule_init_extension();
	kxset_init_extension();
	kxarray2d_init_extension();
}

static char *
compile_and_load(char *filename, char **source_filename) {

	char *bytecode;
	int size;
	List *errors;
	errors = kxc_compile_file(filename, kx_doc_flag, &bytecode, &size);

	if (errors) {
		list_print_strings(errors);
		list_free_all(errors);
		return NULL;
	}
	*source_filename = strdup(filename);
	return bytecode;


}



static int 
main_run_codeblock(KxCore *core, KxStack *stack, KxCodeBlock *codeblock, KxSymbol *message_name) {
	KxMessage *start_msg = kxmessage_new(message_name, core->lobby);

	KxObject *ret_obj = kxcodeblock_run(codeblock,core->lobby, start_msg, KXCB_RUN_NORMAL);

	int ret_value = 0;
	if (ret_obj == NULL) {
		if (kxstack_get_return_state(stack) == RET_THROW) {
			kxstack_dump_throw(stack);
			ret_value = -2;
		}	

		if (kxstack_get_return_state(stack) == RET_LONGRETURN) {
			printf("Fatal error: Invalid long return\n");
			ret_value = -1;
		}
		if (kxstack_get_return_state(stack) == RET_EXIT) {
			KxObject *obj = kxstack_get_and_reset_return_object(stack);
			if (IS_KXINTEGER(obj)) {
				 ret_value = KXINTEGER_VALUE(obj);
			} else {
				fprintf(stderr,"Warning: Exit parameter isn't integer\n");
				ret_value = -1;
			}
			REF_REMOVE(obj);
		}
	}
	if (ret_obj)
		REF_REMOVE(ret_obj);
	if (start_msg)
		kxmessage_free(start_msg);
	return ret_value;
}

int 
vm_init(KxCore *core, KxStack *stack) {
	KxCodeBlock *codeblock;
	char *bytecode = kxvm_init_bytecode;

	KxSymbol *init_message = kxcore_get_symbol(core,"start");
	//REF_ADD(init_message);

	codeblock = kxcodeblock_new_from_bytecode(core, &bytecode, "<vm_init>");
	//REF_ADD(codeblock);

	int ret  = main_run_codeblock(core,stack,codeblock,init_message);
	REF_REMOVE(init_message);
	REF_REMOVE(codeblock);
	return ret;
}

void print_help() 
{
		printf("%s\n",PACKAGE_STRING);
		printf("usage: kreatrix [options] filename [arguments]\n");
		printf("\nOptions:\n");
		printf("  -c cmd    : run command\n");
		printf("  -C        : compile source file in .kxc file (file is not interpreted)\n");
		printf("  -d        : activate 'documentation' mode\n");
		printf("  -i        : run interpreter in interactive mode\n");
		printf("  -h --help : show this help text\n");
		printf("  -v        : switch interpreter into verbose mode\n");
		printf("  --cflags  : CFLAGS for building modules\n");
}

int parse_arguments(int argc, char **argv)
{
  int c;
  struct option longopts[] = {
     { "help",   0,    NULL, 'h' },
     { "cflags", 0,    NULL,  1  },
     { NULL,       0,    NULL,  0  }
  };

  while ((c = getopt_long (argc, argv, "dvhiCc:", longopts, NULL)) != -1)
    switch (c)
      {
      case 'd':
	  	kx_doc_flag = 1;
        break;
      case 'v':
	  	kx_verbose = 1;
        break;
      case 'i':
	  	kx_interactive = 1;
        break;
      case 'C':
	  	kx_compile_flag = 1;
        break;

      case 'h':
	  	print_help();
        exit(-1);
	  case 'c':
	    evaluate_commands = optarg;
		break;
	  
	  case 1:
	  	#ifdef KX_THREADS_SUPPORT
	  	printf("-I%s -DKX_THREADS_SUPPORT\n", PKGINCLUDEDIR);
		#else
	  	printf("-I%s\n", PKGINCLUDEDIR);
		#endif
		exit(0);

      case '?':
 /*       if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);*/
      default:
	  	exit(1);
      }
	
	int count = argc - optind;
	if (count < 1 && (!kx_interactive || kx_compile_flag) && !evaluate_commands) {
		printf("%s\n",PACKAGE_STRING);
		printf("usage: kreatrix [options] [filename] [arguments]\n");
		exit(-1);
	}
	return optind;

}


static int run_interactive(KxCore *core, KxStack *stack) 
{
	char *bytecode;
	int size;
	List *errors;
	errors = kxc_compile_string("VM runShellReadFrom: stdin writeOn: stdout", "<built-in>", 0, &bytecode, &size);
	if (errors) {
		list_free_all(errors);
		printf("Internal error: interactive mode: init compilation FAILED\n");
		return -1;
	}

	char *bytecode_pos = bytecode;
	KxCodeBlock *codeblock;
	codeblock = kxcodeblock_new_from_bytecode(core, &bytecode_pos, "<built-in>");
	kxfree(bytecode);

	KxSymbol *msg_name = kxcore_get_symbol(core, "run_interactive");

	int ret = main_run_codeblock(core,stack,codeblock, msg_name);
	REF_REMOVE(codeblock);
	REF_REMOVE(msg_name);
	return ret;
}

static int
compile_and_save(char *source_filename) 
{
	char *output_filename = create_kxc_filename(source_filename);
	
	if (kx_verbose) {
		printf("Compiling file %s into file %s\n", source_filename, output_filename);
	}

	List *errors = kxc_compile_and_save_file(source_filename, output_filename, kx_doc_flag);
	kxfree(output_filename);

	if (errors) {
		list_print_strings(errors);
		list_free_all(errors);
		return 0;
	}
	return 1;
}

// TODO: Provedet kontrolu na datum
int 
main(int argc, char **argv)
{
	#ifdef KX_LOG
		kx_log_init();
	#endif

 /*   if (!setlocale(LC_CTYPE, "")) {
      fprintf(stderr, "Can't set the specified locale! "
              "Check LANG, LC_CTYPE, LC_ALL.\n");
      return 1;
    }*/

	if (!kxmodules_path_init())
		return -1;
	
	int args_pos = parse_arguments(argc, argv);

	char *source_filename = NULL;
	char *bytecode;
	char *script_filename = NULL;

	if (evaluate_commands) {
		source_filename = strdup("<string>");
		int size;
		List *errors;
		errors = kxc_compile_string(evaluate_commands,source_filename,kx_doc_flag, &bytecode, &size);
		if (errors) {
			kxfree(source_filename);
			list_print_strings(errors);
			list_free_all(errors);
			return -1;
		}
	} else {
		if (args_pos < argc) {
		    script_filename = argv[args_pos];
			if (kx_compile_flag) {
				if(!compile_and_save(script_filename)) {
					return -1;
				}
				return 0;
			}
			bytecode = compile_and_load(script_filename, &source_filename);

			if (bytecode == NULL)
				return -1;


		} else 
			bytecode = NULL;
	}
	extensions_init();

	KxCore *core = kxcore_new();

	KxStack *stack = kxstack_new();
	kxcore_register_stack(core,stack);
	kxcore_switch_to_stack(core,stack);

	int retvalue;
	if ((retvalue=vm_init(core,stack))) {
		fprintf(stderr,"VM init FAILED!\n");
		return retvalue;
	}
	
	// Save arguments into list
	List *list = list_new_size(argc-args_pos-1);
	int t;
	for (t=args_pos+1;t<argc;t++) {
		list->items[t-args_pos-1] = kxstring_new_with(core,argv[t]);
	}
	kxobject_set_slot_no_ref2(core->lobby,kxcore_get_symbol(core,"args"),kxlist_new_with(core,list));

	if (script_filename) {
	/*	kxobject_set_slot_no_ref2(
			core->lobby, 
			kxcore_get_symbol(core, "localImportPath"), 
			kxstring_new_with(core, dirname(script_filename))
		); */
		char *fname = strdup(script_filename);
		kxcore_push_local_import_path(core, strdup(dirname(fname)));
		kxfree(fname);
	} else {
		kxcore_push_local_import_path(core, strdup("."));
	}
	
	PDEBUG("-- vm inited --\n");
	if (kx_verbose) {
		printf("-- vm inited --\n");
	}

	retvalue = 0;
	if (bytecode) {
		char *bytecode_pos = bytecode;

		KxCodeBlock *codeblock;
		codeblock = kxcodeblock_new_from_bytecode(core, &bytecode_pos, source_filename);

		kxfree(source_filename);
		kxfree(bytecode);


		KxSymbol *init_message = kxcore_get_symbol(core,"main");

		retvalue = main_run_codeblock(core,stack,codeblock,init_message);
		REF_REMOVE(init_message);
		REF_REMOVE(codeblock);
	}
	
	if (kx_interactive) {
		retvalue = run_interactive(core, stack);
	}
	
	kxcore_unregister_and_free_all_stacks(core);
	//kxstack_free(stack);

	kxcore_free(core);

	return retvalue;
}



























