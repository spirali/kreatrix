/**********************************************************************
 *
 *  kxarray2d.h - Array 2D 
 *
 *	$Id$
 *
 *
 **********************************************************************/

/*KXobject Base Array2d
 [Data structures] 
*/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxexception.h"

#include "kxarray2d.h"

KxObjectExtension kxarray2d_extension;

static void
kxarray2d_add_method_table(KxArray2d *self);

static KxArray2dData *
kxarray2d_new_data(int x, int y) 
{
	KxArray2dData *data = malloc(sizeof(KxArray2dData));
	ALLOCTEST(data);
	data->array = malloc(sizeof(KxObject*)*x*y);
	ALLOCTEST(data->array);
	data->sizex = x;
	data->sizey = y;
	return data;
}

static void
kxarray2d_data_copy(KxArray2dData *data, KxArray2dData *source) 
{
	int size = source->sizex * source->sizey;
	int t;
	for (t=0;t<size;t++) {
		REF_ADD(source->array[t]);
		data->array[t] = source->array[t];
	}
}


static KxObject *
kxarray2d_clone(KxArray2d *self)
{
	KxArray2d *clone = kxobject_raw_clone(self);
	
	KxArray2dData *data = self->data.ptr;
	clone->data.ptr = kxarray2d_new_data(data->sizex, data->sizey);
	kxarray2d_data_copy(clone->data.ptr, data);
	
	return clone;
}

static void
kxarray2d_free(KxArray2d *self)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	int size = data->sizex * data->sizey;
	int t;
	for (t=0; t<size; ++t) {
		REF_REMOVE(data->array[t]);
	}
	free(data->array);
	free(data);
}

static void
kxarray2d_mark(KxArray2d *self)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	int size = data->sizex * data->sizey;
	int t;
	for (t=0; t<size; ++t) {
		kxobject_mark(data->array[t]);
	}
}

static void
kxarray2d_clean(KxArray2d *self)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	int size = data->sizex * data->sizey;
	int t;
	for (t=0; t<size; ++t) {
		REF_REMOVE(data->array[t]);
	}
	free(data->array);
	data->array = malloc(0);
	data->sizex = 0;
	data->sizey = 0;
}




void kxarray2d_init_extension() {
	kxobjectext_init(&kxarray2d_extension);
	kxarray2d_extension.type_name = "Array2d";

	kxarray2d_extension.clone = kxarray2d_clone;
	kxarray2d_extension.free = kxarray2d_free;
	kxarray2d_extension.clean = kxarray2d_clean;
	kxarray2d_extension.mark = kxarray2d_mark;
}

KxObject *
kxarray2d_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxarray2d_extension;
	
	KxArray2dData *data = kxarray2d_new_data(0,0);
	//kxarray2d_data_fill(data, KXCORE->object_nil);
	self->data.ptr = data;
	
	kxarray2d_add_method_table(self);
	return self;
}


static KxObject *
kxarray2d_set_width_height(KxArray2d *self, KxMessage *message)
{
	KXPARAM_TO_LONG(width, 0);
	KXPARAM_TO_LONG(height, 1);

	KxArray2dData *data = KXARRAY2D_DATA(self);

	KxObject **array_s = data->array;
	KxObject **array_d = malloc(width*height*sizeof(KxObject*));
	ALLOCTEST(array_d);
	int old_x = data->sizex;
	int old_y = data->sizey;
	data->array = array_d;
	data->sizex = width;
	data->sizey = height;

	int w = (old_x < width)?old_x:width;
	int h = (old_y < height)?old_y:height;

	int s,t;
	for (t=0;t<h;t++) {
		int si = old_x * t;
		int di = old_y * t;
		for (s=0;s<w;s++) {
			array_d[di+s] = array_s[si+s];
		}
	}
	KxObject *nil = KXCORE->object_nil;
	
	if (w < width) {
		for (t = 0; t < h; t++) {
			int i = width * t + w;
			for(s=w; s < width; s++) {
				REF_ADD(nil);
				array_d[i++] = nil;
			}
		}
	};

	if (h < height) {
		for (t = h; t < height; t++) {
			int i = t*width;
			for(s=0; s < width; s++) {
				REF_ADD(nil);
				array_d[i++] = nil;
			}
		}
	};

	if (width < old_x) {
		for (t = 0; t < height; t++) {
			int i = old_x * t + width;
			for(s=width; s < old_x; s++) {
				KxObject *obj = array_s[i++];
				REF_REMOVE(obj);
			}
		}
	}

	if (height < old_y) {
		for (t = height; t < old_y; t++) {
			int i = t*old_x;
			for(s=0; s < old_x; s++) {
				KxObject *obj = array_s[i++];
				REF_REMOVE(obj);
			}
		}
	};


	free(array_s);
	KXRETURN(self);
}

static KxObject *
kxarray2d_for_row_do(KxArray2d *self, KxMessage *message)
{
	KXPARAM_TO_LONG(row, 0);
	KxArray2dData *data = KXARRAY2D_DATA(self);
	if (row < 0 || row >= data->sizey) {
		KXTHROW_EXCEPTION("Row is out of bounds");
	}

	KxObject *block = message->params[1];

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;

	int t;
	int limit = data->sizex * (row+1);
	for (t=(data->sizex * row);t<limit;t++) {
		msg.params[0] = data->array[t];
		KxObject * obj = kxobject_evaluate_block(block,&msg);
		KXCHECK(obj);
		REF_REMOVE(obj);
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxarray2d_foreach(KxArray2d *self, KxMessage *message)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	KxObject *block = message->params[0];

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;

	int size = data->sizex * data->sizey;
	int t;
	for (t=0;t<size;t++) {
		msg.params[0] = data->array[t];
		KxObject * obj = kxobject_evaluate_block(block,&msg);
		KXCHECK(obj);
		REF_REMOVE(obj);
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxarray2d_for_column_do(KxArray2d *self, KxMessage *message)
{
	KXPARAM_TO_LONG(column, 0);
	KxArray2dData *data = KXARRAY2D_DATA(self);
	if (column < 0 || column >= data->sizex) {
		KXTHROW_EXCEPTION("Column is out of bounds");
	}

	KxObject *block = message->params[1];

	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = block;

	int t;
	for (t=0;t<data->sizey;t++) {
		msg.params[0] = data->array[t*data->sizex+column];
		KxObject * obj = kxobject_evaluate_block(block,&msg);
		KXCHECK(obj);
		REF_REMOVE(obj);
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxarray2d_at_put(KxArray2d *self, KxMessage *message)
{
	KXPARAM_TO_LONG(x, 0);
	KXPARAM_TO_LONG(y, 1);

	KxArray2dData *data = KXARRAY2D_DATA(self);
	if (x < 0 || x >= data->sizex) {
		KXTHROW_EXCEPTION("x is out of bounds");
	}

	if (y < 0 || y >= data->sizey) {
		KXTHROW_EXCEPTION("y is out of bounds");
	}

	int index = x + y * data->sizex;
	REF_REMOVE(data->array[index]);
	REF_ADD(message->params[2]);
	data->array[index] = message->params[2];
	KXRETURN(self);
}


static KxObject *
kxarray2d_at(KxArray2d *self, KxMessage *message)
{
	KXPARAM_TO_LONG(x, 0);
	KXPARAM_TO_LONG(y, 1);

	KxArray2dData *data = KXARRAY2D_DATA(self);
	if (x < 0 || x >= data->sizex) {
		KXTHROW_EXCEPTION("x is out of bounds");
	}

	if (y < 0 || y >= data->sizey) {
		KXTHROW_EXCEPTION("y is out of bounds");
	}

	int index = x + y * data->sizex;
	KXRETURN(data->array[index]);
}

static KxObject *
kxarray2d_fill(KxArray2d *self, KxMessage *message)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	KxObject *value = message->params[0];
	//kxarray2d_data_fill(data, message->params[0]);
	int size = data->sizex * data->sizey;
	int t;
	for (t=0;t<size;t++) {
		REF_REMOVE(data->array[t]);
		REF_ADD(value);
		data->array[t] = value;
	}

	KXRETURN(self);
}


static KxObject *
kxarray2d_width(KxArray2d *self, KxMessage *message)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	return KXINTEGER(data->sizex);
}

static KxObject *
kxarray2d_height(KxArray2d *self, KxMessage *message)
{
	KxArray2dData *data = KXARRAY2D_DATA(self);
	return KXINTEGER(data->sizey);
}



static void
kxarray2d_add_method_table(KxArray2d *self)
{
	KxMethodTable table[] = {
		{"width",0, kxarray2d_width},
		{"height",0, kxarray2d_height},
		{"foreach:",1, kxarray2d_foreach },
		{"width:height:",2, kxarray2d_set_width_height },
		{"forRow:do:",2, kxarray2d_for_row_do },
		{"forColumn:do:",2, kxarray2d_for_column_do },
		{"atX:y:put:",3, kxarray2d_at_put},
		{"atX:y:",2, kxarray2d_at},
		{"fillWith:",1, kxarray2d_fill},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

