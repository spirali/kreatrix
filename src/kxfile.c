/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

/*KXobject Base File
  [I/O] Prototype of file.
  This object serves for accessing to files on filesystem or to standart 
  streams.
*/


#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "kxfile.h"
#include "utils/utils.h"
#include "utils/list.h"
#include "kxobject.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxlist.h"
#include "kxbytearray.h"
#include "kxinteger.h"
#include "kxcharacter.h"

#define THROW_STD_IO_EXCEPTION  { kxfile_throw_std_io_exception (self); return NULL; }

KxObjectExtension kxfile_extension;
KxFile* kxfile_clone(KxFile *self);
void kxfile_free(KxFile *self);

void 
kxfile_init_extension()
{
		kxobjectext_init(&kxfile_extension);
		kxfile_extension.type_name = "File";
		kxfile_extension.clone = kxfile_clone;
		kxfile_extension.free = kxfile_free;
}


static void kxfile_add_method_table(KxFile *self);


KxObject * 
kxfile_new_prototype(KxCore *core)
{
	KxObject *object = kxcore_clone_base_object(core);
	
	object->extension = &kxfile_extension;
	object->data.ptr = calloc(1,sizeof(KxFileData));
	ALLOCTEST(object->data.ptr);


	kxfile_add_method_table(object);
	return object;
}

KxFile*
kxfile_clone(KxFile *self) 
{
	KxObject *clone = kxobject_raw_clone(self);

	KxFileData *data = calloc(1,sizeof(KxFileData));
	ALLOCTEST(data);

	clone->data.ptr = data;

	return clone;
}

KxFile *
kxfile_stdout(KxCore *core) 
{
	KxObject *clone = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_FILE));
	KxFileData *data = calloc(1,sizeof(KxFileData));
	ALLOCTEST(data);
	data->file = stdout;
	data->filename = strdup("<stdout>");
	data->flag = KXFILE_FLAG_WRITE;
	clone->data.ptr = data;
	return clone;
}

KxFile *
kxfile_stdin(KxCore *core) 
{
	KxObject *clone = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_FILE));
	KxFileData *data = calloc(1,sizeof(KxFileData));
	ALLOCTEST(data);
	data->file = stdin;
	data->filename = strdup("<stdin>");
	data->flag = KXFILE_FLAG_READ;
	clone->data.ptr = data;
	return clone;
}

KxFile *
kxfile_stderr(KxCore *core) 
{
	KxObject *clone = kxobject_raw_clone(kxcore_get_basic_prototype(core,KXPROTO_FILE));
	KxFileData *data = calloc(1,sizeof(KxFileData));
	ALLOCTEST(data);
	data->file = stderr;
	data->filename = strdup("<stderr>");
	data->flag = KXFILE_FLAG_WRITE;
	clone->data.ptr = data;
	return clone;
}


/*static KxObject* 
kxfile_get_io_exception(KxFile *self)
{
	char *path[] = {"IOException"};
	return kxobject_secured_clone_object_by_path(self, path, 1);
};
*/

static void
kxfile_throw_io_exception(KxFile *self, char *msg) 
{
	KxException *exception = kxcore_clone_registered_exception_text(KXCORE,"vm","IOException",msg);
	kxstack_throw_object(KXSTACK,exception);

}

static int
kxfile_test_is_open(KxFile *self, int flag) 
{
	KxFileData *data = KXFILE_DATA(self);
	if (!data->file || (flag && data->flag != flag)) {
		if (flag == KXFILE_FLAG_READ) {
			kxfile_throw_io_exception(self, "File is not open for reading");
			return 0;
		}
		if (flag == KXFILE_FLAG_WRITE) {
			kxfile_throw_io_exception(self, "File is not open for writing");
			return 0;
		}
		kxfile_throw_io_exception(self, "File is not open");
		return 0;
	}
	return 1;
}

static void
kxfile_throw_std_io_exception(KxFile *self) 
{
	KxFileData *data = KXFILE_DATA(self);
	char tmp[250];
	snprintf(tmp,250, "%s: %s", data->filename, strerror(errno));

	kxfile_throw_io_exception(self, tmp);
}

void
kxfile_free(KxFile *self) 
{
	KxFileData *data = self->data.ptr;
	FILE *file = data->file;
	if (file && file != stdout && file != stdin && file != stderr)
		fclose(data->file);
	if (data->filename)
		free(data->filename);
	free(data);
}

/*KXdoc name
  Return name of file.
 */
static KxObject *
kxfile_filename(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	
	char *s;
	s = (data->filename)?data->filename:"";	
	return KXSTRING(s);
}


/*KXdoc name:
  Set name of file.
 */
static KxObject *
kxfile_set_filename(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	
	if (data->file) {
		KXTHROW_EXCEPTION("Filename can be changed only for closed file");
	}

	KxObject *param = message->params[0];
	if (!IS_KXSTRING(param)) {
		KXTHROW_EXCEPTION("Parametr must be string");
	}

	if (data->filename)
		free(data->filename);
	data->filename = strdup(param->data.ptr);

	KXRETURN(self);
}

static KxObject * 
kxfile_open(KxFile *self, KxMessage *message, char *mode) 
{
	KxFileData *data = self->data.ptr;
	if (data->filename == NULL) {
		KxException *excp = kxexception_new_with_message(KXCORE,
			KXSTRING("Filename is not setted."));
		KXTHROW(excp);
	}

	if (data->file) 
		fclose(data->file);
	
	data->file = fopen(data->filename, mode);
	if (data->file == NULL) {
		THROW_STD_IO_EXCEPTION;
	}
	KXRETURN(self);

}

static KxObject *
kxfile_open_for_reading(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	KxObject * ret = kxfile_open(self, message, "r");

	KXCHECK(ret);

	data->flag = KXFILE_FLAG_READ;

	return ret;
}

static KxObject *
kxfile_open_for_writing(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	KxObject * ret = kxfile_open(self, message, "w");

	KXCHECK(ret);

	data->flag = KXFILE_FLAG_WRITE;

	return ret;
}


static KxObject *
kxfile_open_for_appending(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	KxObject * ret = kxfile_open(self, message, "a");

	KXCHECK(ret);

	data->flag = KXFILE_FLAG_WRITE;
	//KXRETURN(self);
	return ret;
}



static KxObject *
kxfile_close(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	
	if (data->file) {
		fclose(data->file);
		data->file = NULL;
	}
	KXRETURN(self);
}

static KxObject *
kxfile_read_line(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self, KXFILE_FLAG_READ))
		return NULL;
	errno = 0;
	char buffer[1024*8];

	char *r;
	KX_THREADS_BEGIN
	r = fgets(buffer,1024*8,data->file);
	KX_THREADS_END
	if (r == NULL)  {
		if (errno) {
			THROW_STD_IO_EXCEPTION;
		} else {
			KXRETURN(KXCORE->object_nil);
		}
	}
	return KXSTRING(buffer);
}

static KxObject *
kxfile_read_lines(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;


	if (!kxfile_test_is_open(self, KXFILE_FLAG_READ))
		return NULL;


	List *list = list_new();
	errno = 0;
	while(1) {
		char buffer[1024*8];
		char *r;

		KX_THREADS_BEGIN
			r = fgets(buffer,1024*8,data->file);
		KX_THREADS_END

		if (r == NULL)  {
			if (errno) {
				THROW_STD_IO_EXCEPTION;
			} else 
				break;
		}
		KxString *str = KXSTRING(buffer);
		REF_ADD(str);
		list_append(list, str);
	}
	return kxlist_new_with(KXCORE,list);
}

static KxObject *
kxfile_read_bytearray_with_size(KxFile *self, KxMessage *message) 
{
	KXPARAM_TO_LONG(size,0);
	
	if (!kxfile_test_is_open(self, KXFILE_FLAG_READ))
		return NULL;

	KxFileData *data = self->data.ptr;
	char buffer[size];
	
	int readed;
	KX_THREADS_BEGIN
	readed = fread(buffer,  sizeof(char), size, data->file);
	KX_THREADS_END

	if (readed < 0) {
		THROW_STD_IO_EXCEPTION;
	}
	ByteArray *bytearray = bytearray_new_from_data(buffer, readed);
	return KXBYTEARRAY(bytearray);
}

static KxObject *
kxfile_foreach_line(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;


	if (!kxfile_test_is_open(self, KXFILE_FLAG_READ))
		return NULL;


	KxObject *param = message->params[0];

	KxMessage msg;
	msg.message_name = NULL;
	msg.target = param;
	msg.params_count = 1;


	errno = 0;

	KxObject *obj = KXCORE->object_nil;
	REF_ADD(obj);

	while(1) {
		
		char buffer[1024*8];

		char *r;
		KX_THREADS_BEGIN
			r = fgets(buffer,1024*8,data->file);
		KX_THREADS_END

		if (r == NULL)  {
			if (errno) {
				THROW_STD_IO_EXCEPTION;
			} else {
				return obj;
			}
		}
		REF_REMOVE(obj);
		msg.params[0] = KXSTRING(buffer);
		obj = kxobject_evaluate_block(param,&msg);
		REF_REMOVE(msg.params[0]);
	
		KXCHECK(obj);
	}
}

static KxObject *
kxfile_write_bytearray(KxFile *self, KxMessage *message) 
{
	KXPARAM_TO_BYTEARRAY(ba,0);

	if (!kxfile_test_is_open(self, KXFILE_FLAG_WRITE))
		return NULL;


	KxFileData *data = self->data.ptr;
	
	errno = 0;

	int r;
	KX_THREADS_BEGIN
		r = fwrite(ba->array, ba->size, 1, data->file);
	KX_THREADS_END
	if (ba->size && r != 1)  {
		THROW_STD_IO_EXCEPTION;
	}
	KXRETURN(self);
}

static KxObject *
seek(KxFile *self, KxMessage *message, int mode)
{
	KXPARAM_TO_LONG(param,0);
	
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self,0))
		return NULL;

	errno = 0;
	if (fseek(data->file, param, mode) == -1) {
		THROW_STD_IO_EXCEPTION;
	}
	KXRETURN(self);

}

static KxObject *
kxfile_seek_set(KxFile *self, KxMessage *message)
{
	return seek(self,message, SEEK_SET);
}


static KxObject *
kxfile_seek_cursor(KxFile *self, KxMessage *message)
{
	return seek(self,message, SEEK_CUR);
}

static KxObject *
kxfile_seek_end(KxFile *self, KxMessage *message)
{
	return seek(self,message, SEEK_END);
}

static KxObject *
kxfile_flush(KxFile *self, KxMessage *message)
{
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self, 0))
		return NULL;

	fflush(data->file);
	KXRETURN(self);
}

/*KXdoc exists
  Test if file exists.
  Return true if file exists. (filename must be setted).
 */
static KxObject *
kxfile_exists(KxFile *self, KxMessage *message)
{
	KxFileData *data = self->data.ptr;
	if (!data->filename) {
		KXTHROW_EXCEPTION("Filename is not set");
	}
	KXRETURN_BOOLEAN(utils_file_exist(data->filename));
}

static KxObject *
kxfile_is_at_end(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	
	if (!kxfile_test_is_open(self, 0))
		return NULL;

	KXRETURN_BOOLEAN(feof(data->file));
}

/*KXdoc contentAsString
  Return content of file as string.
  Return conent of file from actual position of cursor to end of file.
*/
static KxObject *
kxfile_content_as_string(KxFile *self, KxMessage *message)
{
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self,KXFILE_FLAG_READ))
		return NULL;

	FILE *file = data->file;

	if (fseek(file,0,SEEK_END) == -1) {
		THROW_STD_IO_EXCEPTION;
	}
	long size = ftell(file);
	
	if (fseek(file,0,SEEK_SET) == -1) {
		THROW_STD_IO_EXCEPTION;
	}

	char *buffer = malloc(size+1);

	int r;
	KX_THREADS_BEGIN
	r = fread(buffer,size, 1,file);
	KX_THREADS_END

	if (r != 1) {
		THROW_STD_IO_EXCEPTION;
	}
	buffer[size] = 0;

	KxString *str = KXSTRING(buffer);
	free(buffer);
	return str;
}


/*KXdoc size
  Return size of file (in bytes).
*/
static KxObject *
kxfile_size(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self, KXFILE_FLAG_READ))
		return NULL;

	FILE *file = data->file;

	long pos = ftell(file);

	if (fseek(file,0,SEEK_END) == -1) {
		THROW_STD_IO_EXCEPTION;
	}
	long size = ftell(file);
	
	if (fseek(file,pos,SEEK_SET) == -1) {
		THROW_STD_IO_EXCEPTION;
	}

	return KXINTEGER(size);
}


static KxObject *
kxfile_as_filedescriptor(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;

	if (!kxfile_test_is_open(self, 0))
		return NULL;

	return KXINTEGER(fileno(data->file));
}

static KxObject *
kxfile_read_character(KxFile *self, KxMessage *message) 
{
	KxFileData *data = self->data.ptr;
	int c = fgetc(data->file);
	if (c == EOF) {
		return kxobject_evaluate_block_simple(message->params[0]);
	}
	return KXCHARACTER(c);
}

static void 
kxfile_add_method_table(KxFile *self)
{
	KxMethodTable table[] = {
		{"name",0, kxfile_filename },
		{"name:",1, kxfile_set_filename },
		{"openForReading",0, kxfile_open_for_reading},
		{"openForWriting",0, kxfile_open_for_writing},
		{"openForAppending",0, kxfile_open_for_appending},
		{"close",0, kxfile_close},
		{"readLine", 0, kxfile_read_line},
		{"readLines", 0, kxfile_read_lines},
		{"readByteArraySize:", 1, kxfile_read_bytearray_with_size},
		{"readCharacterIfEof:", 1, kxfile_read_character},
		{"isAtEnd", 0, kxfile_is_at_end },
		{"size", 0, kxfile_size },
		{"foreachLine:", 1, kxfile_foreach_line},
		{"contentAsString", 0, kxfile_content_as_string},
		{"writeByteArray:", 1, kxfile_write_bytearray },
		{"seekSet:", 1, kxfile_seek_set },
		{"seekCursor:", 1, kxfile_seek_cursor },
		{"seekEnd:", 1, kxfile_seek_end },
		{"flush", 0, kxfile_flush },
		{"exists", 0, kxfile_exists },
		{"asFileDescriptor", 0, kxfile_as_filedescriptor },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
