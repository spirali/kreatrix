#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxexception.h"

//#include "gtkbin.h"

#include "gtkobject.h"
#include "gtkwidget.h"
#include "gtkcontainer.h"
#include "gtkbin.h"
#include "gtkwindow.h"
#include "gtkbutton.h"
#include "gtkcheckbutton.h"
#include "gtkbox.h"
#include "gtkvbox.h"
#include "gtkhbox.h"
#include "gtk_utils.h"

KxObjectExtension kxgtkbin_extension;

static void kxgtkbin_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
}

void kxgtkbin_extension_init() {
	kxobjectext_init(&kxgtkbin_extension);
	kxgtkbin_extension.type_name = "GtkBin";
	kxgtkbin_extension.parent = &kxgtkcontainer_extension;
	kxgtkbin_extension.free = kxgtkbin_free;
//%init_extension%
}


static void
kxgtkbin_add_method_table(KxGtkBin *self);

KxObject *
kxgtkbin_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkbin_extension;

	
	kxgtkbin_add_method_table(self);

	
	self->extension = &gtkabstractclass_extension;

	return self;
}


KxObject *
kxgtkbin_from(KxCore *core, GtkBin* data)
{
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkbin_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
}




static void
kxgtkbin_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
