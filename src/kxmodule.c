/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

#include <config.h>

#include "kxmodule.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxmodules_path.h"
#include "kxcompile_utils.h"
#include "kxlist.h"


// TODO: kx_verbose do globals.h
extern int kx_verbose;

KxObjectExtension kxmodule_extension;

static void kxmodule_free(KxModule *self);
static void kxmodule_add_method_table(KxModule *self);
static KxModule *kxmodule_clone(KxModule *self);


void
kxmodule_init_extension() {
	kxobjectext_init(&kxmodule_extension);
	kxmodule_extension.type_name = "Module";
	kxmodule_extension.free = kxmodule_free;
	kxmodule_extension.clone = kxmodule_clone;
}


KxObject *
kxmodule_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);

	object->extension = &kxmodule_extension;
	object->data.ptr = calloc(1,sizeof(KxModuleData));
	ALLOCTEST(object->data.ptr);

	kxmodule_add_method_table(object);

	List *list = list_new();

	char **path = kxmodules_path;

	while(*path) {
		KxString *str = kxstring_new_with(core,*path);
		list_append(list,str);
		path++;
	}

	KxList *kxlist = kxlist_new_with(core,list);

	kxobject_set_slot_no_ref2(object, kxcore_get_symbol(core,"paths"),kxlist);

	return object;
}


static void
kxmodule_call_unload(KxModule *self, void *handle)
{

	void (*module_unload) (KxModule *) = dlsym(handle, "kxmodule_unload");

	if (module_unload) {
		module_unload(self);
	}
}


static void
kxmodule_free(KxModule *self) 
{
	KxModuleData *data = KXMODULE_DATA(self);

	if (data->handle_list) {
		int t;
		for (t=0;t<data->handle_list->size;t++) {
			kxmodule_call_unload(self, data->handle_list->items[t]);
	/*	FIXME, check if all object from lib isn't exist
	 *	if (dlclose(data->handle)) {
			fprintf(stderr,"Module: dlclose: %s",dlerror());
		}*/

		}
		list_free(data->handle_list);
	}
	if (data->path) {
		free(data->path);
	}
	
	free(data);
}

static KxModule * 
kxmodule_clone(KxModule *self) 
{
	KxObject *clone = kxobject_raw_clone(self);

	clone->data.ptr = calloc(1,sizeof(KxModuleData));
	ALLOCTEST(clone->data.ptr);

	return clone;
}

static KxObject * 
kxmodule_run_main(KxModule *self, void *handle, KxMessage *message)
{	
	KxObject * (*module_main) (KxModule *, KxMessage *) = dlsym(handle, "kxmodule_main");
	if (!module_main) {
		KXTHROW_EXCEPTION("Module init function not found");
	}
	return module_main(self,message);
}

static KxObject *
kxmodule_load(KxModule *self, KxMessage *message) 
{
	KxModuleData *data = KXMODULE_DATA(self);
	
	if (data->path) { 
		/** Use path from data withou looking up */
		free(data->path);
	}

	KXPARAM_TO_CSTRING(param,0);


	KxSymbol *symbol = kxcore_get_symbol(KXCORE,"paths");
	KxObject *paths_obj = kxobject_find_slot(self, symbol);
	REF_REMOVE(symbol);
	if (paths_obj == NULL) {
		KXTHROW_EXCEPTION("slot 'paths' not found");
	}
	if (!IS_KXLIST(paths_obj)) {
		KXTHROW_EXCEPTION("slot 'paths' must be List");
	}

	List *paths = kxlist_to_list_of_cstrings(paths_obj);
	char kx_filename[256];
	//snprintf(lib_filename,256,"%s/%s%s",param, param, SHARE_EXT);
	
	/* Looking for NAME/NAME.kx" */
	snprintf(kx_filename,256,"%s/%s.kx",param, param);
	int short_path = 0;
	int path_index = utils_file_exists_in_paths(paths,kx_filename);

	/* Looking for NAME.kx" */
	if (path_index == -1) {
		snprintf(kx_filename,256,"%s.kx", param);
		path_index = utils_file_exists_in_paths(paths,kx_filename);
		short_path = 1;
	}

	if (path_index != -1) {
		char *path = paths->items[path_index];
		data->path = malloc(sizeof(char) * (strlen(path) + strlen(param) + 3));

		if (short_path) {
			strcpy(data->path, path);
		} else {
			sprintf(data->path, "%s/%s/", path, param);
		}
		char *kx_path = utils_path_join(path, kx_filename);
		KxObject *ret_object = kxcompile_do_file(kx_path, self, message->message_name);
		free(kx_path);
		list_free(paths);
		KXCHECK(ret_object);
		REF_REMOVE(ret_object);
		if (kx_verbose) {
			printf("Module '%s' loaded\n",param);
		}
		KXRETURN(self);
	}
	list_free(paths);
	KxException *excp = kxexception_new_with_text(KXCORE,"Module '%s' not found", param);
	KXTHROW(excp);
}

static KxObject *
kxmodule_path(KxModule *self, KxMessage *message)
{
	KxModuleData *data = KXMODULE_DATA(self);
	if (data->path == NULL) {
		KXRETURN(KXCORE->object_nil);
	}
	return KXSTRING(data->path);
}

static KxObject *
kxmodule_load_c_lib(KxModule *self, KxMessage *message)
{
	KxModuleData *data = KXMODULE_DATA(self);
	KXPARAM_TO_CSTRING(param,0);
	char libname[256];
	snprintf(libname,256,"%s%s", param, SHARE_EXT);
	char *libpath = utils_path_join(data->path, libname);
	if (kx_verbose) {
		printf("dlopen: %s\n", libpath);
	}
	void *handle = dlopen(libpath, RTLD_NOW|RTLD_GLOBAL);
	free(libpath);
	if (handle == NULL) {
		KxException *excp = kxexception_new_with_text(KXCORE,"Module load error: %s", dlerror());
		KXTHROW(excp);
	}
	if (data->handle_list == NULL) {
		data->handle_list = list_new();
	}
	list_append(data->handle_list,handle);
	return kxmodule_run_main(self, handle, message);
}


static void 
kxmodule_add_method_table(KxModule *self)
{
	KxMethodTable table[] = {
		{"load:",1, kxmodule_load },
		{"loadCLibrary:",1, kxmodule_load_c_lib },
		{"path",0, kxmodule_path },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
