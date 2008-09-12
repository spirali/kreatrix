/*
   kxtree.c
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
	
#include "kxtree.h"

#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxlist.h"
#include "gtk_utils.h"
#include "kxgtk_wrapper.h"

static KxObject *
kxgtkliststore_clone_with_columns (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_LIST(list, 0);

	int t;
	int columns = list->size;

	GType *types = malloc(sizeof(GType) * columns);
	ALLOCTEST(types);
	
	for (t=0;t<list->size;t++) {
		types[t] = kxgtk_object_to_gtype(list->items[t]);
	}

	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_list_store_newv(columns, types);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	g_object_unref(clone->data.ptr);

	free(types);
	
	return clone;
}

static KxObject *
kxgtkliststore_at_set_value_column (KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_GTKTREEITER(iter, 0);
	KXPARAM_TO_INT(column, 1);
	KxObject *obj = message->params[2];
	
	GValue value = { 0 };
 	g_value_init(&value,
                 gtk_tree_model_get_column_type(GTK_TREE_MODEL(self->data.ptr),
                 	column));
	
	if (kxgtk_gvalue_from_object(obj, &value)) {
		KxObject *typestr = kxobject_type_name(obj);
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Cannot convert '%s' to '%s'", 
			KXSTRING_VALUE(typestr), 
			g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(&value))));
		REF_REMOVE(typestr);
		KXTHROW(excp);
	}

	gtk_list_store_set_value(GTK_LIST_STORE(self->data.ptr), iter, column, &value);

	g_value_unset(&value);

	KXRETURN(self);
}

static KxObject *
kxgtkliststore_at_set_values (KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_GTKTREEITER(iter, 0);
	KXPARAM_TO_LIST(list, 1);

	GValue value = { 0 };

	int t;
	for (t = 0; t < list->size; t++) {
		KxObject *obj = list->items[t];

		GValue value = { 0 };

	 	g_value_init(&value,
    	             gtk_tree_model_get_column_type(GTK_TREE_MODEL(self->data.ptr),
        	         t));
	
		if (kxgtk_gvalue_from_object(obj, &value)) {
			KxObject *typestr = kxobject_type_name(obj);
			KxException *excp = kxexception_new_with_text(KXCORE,
				"Cannot convert '%s' to '%s'", 
				KXSTRING_VALUE(typestr), 
				g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(&value))));
			REF_REMOVE(typestr);
			g_value_unset(&value);
			KXTHROW(excp);
		}
		gtk_list_store_set_value(GTK_LIST_STORE(self->data.ptr), iter, t, &value);

		g_value_unset(&value);
	}
	KXRETURN(self);
}


void kxgtkliststore_extra_init(KxObject *self) {
	KxMethodTable table[] = {
		{"cloneWithColumns:",1,kxgtkliststore_clone_with_columns},
		{"at:column:setValue:",3, kxgtkliststore_at_set_value_column},
		{"at:putValues:",2, kxgtkliststore_at_set_values },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

static KxObject *
kxgtktreestore_clone_with_columns (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_LIST(list, 0);

	int t;
	int columns = list->size;

	GType *types = malloc(sizeof(GType) * columns);
	ALLOCTEST(types);
	
	for (t=0;t<list->size;t++) {
		types[t] = kxgtk_object_to_gtype(list->items[t]);
	}

	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_tree_store_newv(columns, types);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	g_object_unref(clone->data.ptr);

	free(types);
	
	return clone;
}

static KxObject *
kxgtktreestore_at_set_value_column (KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_GTKTREEITER(iter, 0);
	KXPARAM_TO_INT(column, 1);
	KxObject *obj = message->params[2];
	
	GValue value = { 0 };
 	g_value_init(&value,
                 gtk_tree_model_get_column_type(GTK_TREE_MODEL(self->data.ptr),
                 	column));
	
	if (kxgtk_gvalue_from_object(obj, &value)) {
		KxObject *typestr = kxobject_type_name(obj);
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Cannot convert '%s' to '%s'", 
			KXSTRING_VALUE(typestr), 
			g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(&value))));
		REF_REMOVE(typestr);
		KXTHROW(excp);
	}

	gtk_tree_store_set_value(GTK_TREE_STORE(self->data.ptr), iter, column, &value);

	g_value_unset(&value);

	KXRETURN(self);
}

static KxObject *
kxgtktreestore_at_set_values (KxObject *self, KxMessage *message) 
{
	KXPARAM_TO_GTKTREEITER(iter, 0);
	KXPARAM_TO_LIST(list, 1);

	int t;

	for (t = 0; t < list->size; t++) {
		KxObject *obj = list->items[t];

		GValue value = { 0 };

	 	g_value_init(&value,
    	             gtk_tree_model_get_column_type(GTK_TREE_MODEL(self->data.ptr),
        	         t));
	
		if (kxgtk_gvalue_from_object(obj, &value)) {
			KxObject *typestr = kxobject_type_name(obj);
			KxException *excp = kxexception_new_with_text(KXCORE,
				"Cannot convert '%s' to '%s'", 
				KXSTRING_VALUE(typestr), 
				g_type_name(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(&value))));
			REF_REMOVE(typestr);
			g_value_unset(&value);
			KXTHROW(excp);
		}
		gtk_tree_store_set_value(GTK_TREE_STORE(self->data.ptr), iter, t, &value);

		g_value_unset(&value);
	}
	KXRETURN(self);
}


void kxgtktreestore_extra_init(KxObject *self) {
	KxMethodTable table[] = {
		{"cloneWithColumns:",1,kxgtktreestore_clone_with_columns},
		{"at:column:setValue:",3, kxgtktreestore_at_set_value_column},
		{"at:putValues:",2, kxgtktreestore_at_set_values },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

static void
kxgtk_mark_gobject_list_callback(GObject *self, gpointer data)
{
	kxgtk_mark_gobject(self);
}

void kxgtktreeview_extra_mark(KxObject *self) 
{
	GtkTreeView *data = self->data.ptr;
	GtkTreeModel *model = gtk_tree_view_get_model(data);
	if (model)
		kxgtk_mark_gobject(G_OBJECT(model));

	GtkTreeSelection *selection = gtk_tree_view_get_selection(data);
	if (selection)
		kxgtk_mark_gobject(G_OBJECT(selection));

	
	GList *list = gtk_tree_view_get_columns(data);
	g_list_foreach(list, (GFunc) kxgtk_mark_gobject_list_callback, NULL);
}



void kxgtktreeviewcolumn_extra_mark(KxObject *self) 
{
	GtkTreeViewColumn *data = self->data.ptr;
	GList *list = gtk_tree_view_column_get_cell_renderers(data);
	g_list_foreach(list, (GFunc) kxgtk_mark_gobject_list_callback, NULL);
	g_list_free(list);
}

static kxgtktreemodel_extra_mark_callback(GtkTreeModel *model,
                                          GtkTreePath *path,
                                          GtkTreeIter *iter,
                                          gpointer data)
{
	int *column = data;

	GValue value = { 0 };
	do {
		gtk_tree_model_get_value(model, iter, *column, &value);

		KxObject *self = g_value_get_boxed(&value);
		if (self) {
	//		printf("Mark! \n");
	//		kxobject_dump(self);
			kxobject_mark(self);
		}
		column++;
		g_value_unset(&value);
	} while( (*column) != -1);
	return FALSE;
}

void kxgtktreemodel_extra_mark(KxObject *self) 
{
	GtkTreeModel *data = self->data.ptr;
	int count = gtk_tree_model_get_n_columns(data);
	int t;
	
	int columns_with_kxobjects[count+1];
	int c = 0;

	for (t=0; t < count; ++t) {
		if (gtk_tree_model_get_column_type(data, t) == KX_TYPE_OBJECT) {
				columns_with_kxobjects[c++] = t;	
		}
	}

	if (c == 0)
		return;
	
	columns_with_kxobjects[c] = -1;

	gtk_tree_model_foreach(data, kxgtktreemodel_extra_mark_callback, columns_with_kxobjects);
}

static KxObject *
kxgtktreeselection_selected_iter (KxObject *self, KxMessage *message)
{
	GtkTreeIter *iter = malloc(sizeof(GtkTreeIter));
	gboolean result = gtk_tree_selection_get_selected(self->data.ptr, NULL, iter);
	if (result == TRUE) {
		return KXGTKTREEITER(iter);
	} else {
		KXTHROW_EXCEPTION("There is no selected node");
	}
}

static KxObject *
kxgtktreeselection_model (KxObject *self, KxMessage *message)
{
	GtkTreeModel *model;
	gtk_tree_selection_get_selected(self->data.ptr, &model, NULL);
	return KXGTKTREEMODEL(model);
}

static KxObject *
kxgtktreeselection_is_selected (KxObject *self, KxMessage *message)
{
	return KXGBOOLEAN(gtk_tree_selection_get_selected(self->data.ptr, NULL, NULL));
}


void kxgtktreeselection_extra_init(KxObject *self) {

	// Get selection from prototype of treeview 
	self->data.ptr = 
		gtk_tree_view_get_selection(
			kxcore_get_prototype(KXCORE, &kxgtktreeview_extension)->data.ptr);
	g_object_ref(self->data.ptr);
	KxMethodTable table[] = {
		{"selectedIter",0,kxgtktreeselection_selected_iter},
		{"model",0,kxgtktreeselection_model},
		{"isSelected",0,kxgtktreeselection_is_selected},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

static KxObject *
kxgtktreemodel_value_at_column (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKTREEITER(iter, 0);
	KXPARAM_TO_INT(column, 1);
	GValue value = { 0 };
	gtk_tree_model_get_value(self->data.ptr, iter, column, &value);
	KxObject *obj = kxgtk_kxobject_from_gvalue(KXCORE, &value);
	g_value_reset(&value);
	return obj;
}

void kxgtktreemodel_extra_init(KxObject *self) {
	
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"valueAt:column:",2,kxgtktreemodel_value_at_column},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);

	self->extension = ext;
}
