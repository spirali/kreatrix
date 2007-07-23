/*************************
 *
 *  %%Name%%
 *
 */

#ifndef __KX_OBJECT_%%NAME%%_H
#define __KX_OBJECT_%%NAME%%_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KX%%NAME%%_DATA(self) ((%%data%%*) (self)->data.ptr)

#define IS_KX%%NAME%%(self) ((self)->extension == &kx%%name%%_extension)

typedef struct KxObject Kx%%Name%%;


Kx%%Name%% *kx%%name%%_new_prototype(KxCore *core);


void kx%%name%%_extension_init();
extern KxObjectExtension kx%%name%%_extension;


#endif // __KX_OBJECT_%%NAME%%_H
