/*
   kxxmldocument.c
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
	
#include <stdlib.h>

#include <kxmessage.h>
#include <kxobject.h>
#include <kxstring.h>
#include <kxcfunction.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "xml.h"
#include "kxxmldocument.h"
#include "kxxmlnode.h"

KxObjectExtension kxxmldocument_extension;

static void
kxxmldocument_add_method_table(KxXmlDocument *self);

static KxObject *
kxxmldocument_clone(KxXmlDocument *self)
{
	KxXmlDocument *clone = kxobject_raw_clone(self);
	
	clone->data.ptr = xmlCopyDoc(self->data.ptr, 1);
	// TODO

	return clone;
}

static void
xml_document_erase_private_from_nodes(xmlDoc *doc)
{
	xmlNode *root = xmlDocGetRootElement(doc);
	
	if (root == NULL)
		return;

	List *list = list_new();
	list_append(list, root);

	do {
		xmlNode *node = list_fast_pop(list);
		KxXmlNode *kxnode = node->_private;
		if (kxnode) {
			kxnode->data.ptr = NULL;
			node->_private = NULL;
		}
		
		if (node->next)
			list_append(list, node->next);
		if (node->children)
			list_append(list, node->children);

		node->_private = NULL;
	} while (list->size);
	list_free(list);
}

static void
xml_document_free(xmlDoc *doc)
{
	xml_document_erase_private_from_nodes(doc);
	xmlFreeDoc(doc);

}

static void
kxxmldocument_free(KxXmlDocument *self)
{
	xml_document_free(self->data.ptr);
}

void kxxmldocument_extension_init() {
	kxobjectext_init(&kxxmldocument_extension);
	kxxmldocument_extension.type_name = "XmlDocument";

	kxxmldocument_extension.clone = kxxmldocument_clone;
	kxxmldocument_extension.free = kxxmldocument_free;
}

KxObject *
kxxmldocument_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxxmldocument_extension;
	
	self->data.ptr = xmlNewDoc("1.0");
	
	kxxmldocument_add_method_table(self);
	return self;
}


static KxObject *
kxxmldocument_read_file(KxXmlDocument *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(filename, 0);
	List *list = list_new();
	
	xmlSetGenericErrorFunc(list, 
		(xmlGenericErrorFunc) kxxml_add_error_to_list);

	xmlDoc *doc;
	KX_THREADS_BEGIN
	doc = xmlReadFile(filename,NULL, 0);
	KX_THREADS_END

	if (!doc) {
		return kxxml_throw_exception(KXCORE, list);
	}
	list_free_all(list);
	xml_document_free(self->data.ptr);
	self->data.ptr = doc;

	KXRETURN(self);
}

static KxObject *
kxxmldocument_root_element(KxXmlDocument *self, KxMessage *message)
{
	xmlNode *node = xmlDocGetRootElement(KXXMLDOCUMENT_DATA(self));
	return kxxmlnode_new_with(KXCORE, node);
}

static KxObject *
kxxmldocument_name(KxXmlDocument *self, KxMessage *message)
{
	xmlDoc *doc = KXXMLDOCUMENT_DATA(self);
	return (doc->name)?KXSTRING(doc->name):KXSTRING("");
}

static void
kxxmldocument_add_method_table(KxXmlDocument *self)
{
	KxMethodTable table[] = {
		{"readFile:",1, kxxmldocument_read_file },
		{"rootElement",0, kxxmldocument_root_element },
		{"name",0, kxxmldocument_name },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

