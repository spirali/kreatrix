#ifndef __KX_CLOSURE_H
#define __KX_CLOSURE_H

#include <gtk/gtk.h>
#include "kxobject.h"

typedef struct KxClosure KxClosure;
struct KxClosure {
	GClosure closure;
	KxObject *callback;
};

KxClosure *kxclosure_new(KxObject *callback);

void kxclosure_mark(KxClosure *closure);

#endif
