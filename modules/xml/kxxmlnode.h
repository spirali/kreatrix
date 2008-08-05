/*
   kxxmlnode.h
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
	
#ifndef __KX_OBJECT_XMLNODE_H
#define __KX_OBJECT_XMLNODE_H


#include <libxml/parser.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXXMLNODE_DATA(self) ((xmlNode*) (self)->data.ptr)

#define IS_KXXMLNODE(self) ((self)->extension == &kxxmlnode_extension)

#define KXXMLNODE(node) (kxxml_node_new_with(KXCORE, (node)))

typedef struct KxObject KxXmlNode;


KxXmlNode *kxxmlnode_new_prototype(KxCore *core);

KxXmlNode *kxxmlnode_new_with(KxCore *core, xmlNode *node);
void kxxmlnode_add_node_types(KxXmlNode *self, KxObject *module);


void kxxmlnode_extension_init();
extern KxObjectExtension kxxmlnode_extension;


#endif // __KX_OBJECT_XMLNODE_H
