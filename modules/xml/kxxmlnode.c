/*
   kxxmlnode.c
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
#include <kxlist.h>

/*#include <libxml/parser.h>
#include <libxml/tree.h>*/

#include "kxxmlnode.h"
#include "xml.h"


KxObjectExtension kxxmlnode_extension;

static void
kxxmlnode_add_method_table(KxXmlNode *self);

/*static KxObject *
kxxmlnode_clone(KxXmlNode *self)
{
	KxXmlNode *clone = kxobject_raw_clone(self);
	
	// TODO

	return clone;
}*/

static KxXmlNode *
kxxmlnode_clone_with(KxXmlNode *self, xmlNode *node)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = node;

	node->_private = clone;
	return clone;
}

KxXmlNode *
kxxmlnode_new_with(KxCore *core, xmlNode *node)
{
	if (node->_private) {
		KxXmlNode *kxnode = (KxXmlNode*) node->_private;
		REF_ADD(kxnode);
		return kxnode;
	}

	KxXmlNode *proto = kxcore_get_prototype(core, &kxxmlnode_extension);

	/** TODO: Get rid of finding symbol for every new node */
	char *name;
	switch(node->type) {
		case XML_ELEMENT_NODE: name = "Element"; break;
		case XML_ATTRIBUTE_NODE: name = "Attribute"; break;
		case XML_TEXT_NODE: name = "Text"; break;
		default: name = NULL;
	}
	if (name) {
		KxSymbol *symbol = kxcore_get_symbol(core,name);
		KxObject *p = kxobject_get_slot(proto, symbol);
		REF_REMOVE(symbol);
		if (p == NULL)  {
			return kxxmlnode_clone_with(proto, node);
		}
		return kxxmlnode_clone_with(p, node);
	}
	return kxxmlnode_clone_with(proto, node);
}

static KxXmlNode *
kxxmlnode_clone_with_new_data(KxXmlNode *self)
{
	KxObject *clone = kxobject_raw_clone(self);

	xmlNode *node = xmlNewNode(NULL, "XmlNode");
	clone->data.ptr = node;
	node->_private = clone;

	return clone;
}

void
kxxmlnode_add_node_types(KxXmlNode *self, KxObject *module)
{
	KxXmlNode *element = kxxmlnode_clone_with_new_data(self);
	kxobject_set_type(element,"XmlElement");
    kxobject_set_slot_no_ref2(self, KXSYMBOL("Element"), element);

	KxXmlNode *attribute = kxxmlnode_clone_with_new_data(self);
	kxobject_set_type(attribute,"XmlAttribute");
    kxobject_set_slot_no_ref2(self, KXSYMBOL("Attribute"), attribute);

	KxXmlNode *text = kxxmlnode_clone_with_new_data(self);
	kxobject_set_type(text,"XmlText");
    kxobject_set_slot_no_ref2(self, KXSYMBOL("Text"), text);

}

static void
kxxmlnode_free(KxXmlNode *self)
{
	xmlNode *node = KXXMLNODE_DATA(self);

	//printf("Node free %p %p %p\n", self, node, node?node->doc:NULL);
	if (node == NULL)
		return;

	node->_private = NULL;
	if (node->doc == NULL)  {
		// xmlNode is freed when xmlDoc is freed
		xmlFreeNode(self->data.ptr);
	}
}

void kxxmlnode_extension_init() {
	kxobjectext_init(&kxxmlnode_extension);
	kxxmlnode_extension.type_name = "XmlNode";

//	kxxmlnode_extension.clone = kxxmlnode_clone;
	kxxmlnode_extension.free = kxxmlnode_free;
}

KxObject *
kxxmlnode_new_prototype(KxCore *core)
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxxmlnode_extension;

	xmlNode *node = xmlNewNode(NULL, "XmlNode");
	//printf("New node %p %p %p\n", self, node, node->doc);
	
	self->data.ptr = node;
	node->_private = self;

	kxxmlnode_add_method_table(self);
	return self;
}

static xmlNode *
kxxmlnode_test_data(KxXmlNode *self)
{
	if (self->data.ptr == NULL) {
		kxstack_throw_object(KXCORE->stack, kxcore_clone_registered_exception_text(
			KXCORE, "module-xml","XmlException", "Invalid node, document was freed"));
		return NULL;
	}
	return self->data.ptr;
}

static KxObject *
kxxmlnode_name(KxXmlNode *self, KxMessage *message)
{
	xmlNode *node = kxxmlnode_test_data(self);
	KXCHECK(node);
	return KXSTRING((char*)node->name);
}

static KxObject *
kxxmlnode_content(KxXmlNode *self, KxMessage *message)
{
	xmlNode *node = kxxmlnode_test_data(self);
	KXCHECK(node);
	if (node->content) {
		return KXSTRING((char*)node->content);
	} else {
		KXRETURN(KXCORE->object_nil);
	}
}

static KxObject *
kxxmlnode_childrens(KxXmlNode *self, KxMessage *message)
{
	xmlNode *data = kxxmlnode_test_data(self);
	KXCHECK(data);

	List *list = list_new();
	xmlNode *node;
	for (node=data->children; node; node=node->next) {
		list_append(list, kxxmlnode_new_with(KXCORE, node));
	}
	return KXLIST(list);
}

static KxObject *
kxxmlnode_properties(KxXmlNode *self, KxMessage *message)
{
	xmlNode *data = kxxmlnode_test_data(self);
	KXCHECK(data);

	List *list = list_new();
	xmlNode *node;
	for (node=(xmlNode*)data->properties; node; node=node->next) {
		list_append(list, kxxmlnode_new_with(KXCORE, node));
	}
	return KXLIST(list);

}

static KxObject *
kxxmlnode_children(KxXmlNode *self, KxMessage *message)
{
	xmlNode *data = kxxmlnode_test_data(self);
	KXCHECK(data);

	if (data->children) {
		return kxxmlnode_new_with(KXCORE, data->children);
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxxmlnode_next(KxXmlNode *self, KxMessage *message)
{
	xmlNode *data = kxxmlnode_test_data(self);
	KXCHECK(data);

	if (data->children) {
		return kxxmlnode_new_with(KXCORE, data->next);
	}
	KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxxmlnode_prev(KxXmlNode *self, KxMessage *message)
{
	xmlNode *data = kxxmlnode_test_data(self);
	KXCHECK(data);

	if (data->children) {
		return kxxmlnode_new_with(KXCORE, data->prev);
	}
	KXRETURN(KXCORE->object_nil);
}

/*static KxObject *
kxxmlnode_is_text(KxXmlNode *self, KxMessage *message)
{
	xmlNode *node = kxxmlnode_test_data(self);
	KXCHECK(node);

	KXRETURN_BOOLEAN(node->type == XML_TEXT_NODE);
}*/

static void
kxxmlnode_add_method_table(KxXmlNode *self)
{
	KxMethodTable table[] = {
		{"name",0, kxxmlnode_name },
		{"childrens",0, kxxmlnode_childrens },
		{"children",0, kxxmlnode_children },
		{"next",0, kxxmlnode_next },
		{"prev",0, kxxmlnode_prev },
		{"properties",0, kxxmlnode_properties },
		{"content",0, kxxmlnode_content },
	//	{"isText",0, kxxmlnode_is_text },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
