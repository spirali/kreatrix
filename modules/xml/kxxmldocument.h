/*************************
 *
 *  XmlDocument
 *
 */

#ifndef __KX_OBJECT_XMLDOCUMENT_H
#define __KX_OBJECT_XMLDOCUMENT_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXXMLDOCUMENT_DATA(self) ((xmlDoc*) (self)->data.ptr)

#define IS_KXXMLDOCUMENT(self) ((self)->extension == &kxxmldocument_extension)

typedef struct KxObject KxXmlDocument;


KxXmlDocument *kxxmldocument_new_prototype(KxCore *core);


void kxxmldocument_extension_init();
extern KxObjectExtension kxxmldocument_extension;


#endif // __KX_OBJECT_XMLDOCUMENT_H
