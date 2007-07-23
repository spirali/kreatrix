/*************************
 *
 *  XmlNode
 *
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
