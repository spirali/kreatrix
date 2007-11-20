#include <stdlib.h>
#include <string.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

#include "gtktextbuffer.h"
#include "gtktextiter.h"


static KxObject *
kxgtktextbuffer_insert (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(string, 0);
	KXPARAM_TO_GTKTEXTITER(iter, 1);

	gtk_text_buffer_insert(self->data.ptr, iter, string, strlen(string));

	KXRETURN(self);
}

static KxObject *
kxgtktextbuffer_set_text (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(string, 0);

	gtk_text_buffer_set_text(self->data.ptr, string, strlen(string));

	KXRETURN(self);
}


void
kxgtktextbuffer_extra_init(KxObject *self)
{
	KxMethodTable table[] = {
		{"insert:at:",2,kxgtktextbuffer_insert},
		{"text:",1,kxgtktextbuffer_set_text},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
