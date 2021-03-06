#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "kxmodule.h"
#include "kxmessage.h"
#include "kxcfunction.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxcore.h"
#include "gdkevent.h"
#include "kxgtk_wrapper.h"
#include "gtk_utils.h"

static void kxgtk_add_method_table(KxObject *self);


KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{
	List *list = list_new();
	kxcore_set_global_data(KXCORE,"kxgtk-window-list", list);

	gtk_init(0, NULL);
	kxgtk_utils_init(KXCORE);

	kxgdkevent_extension_init();
%init_extensions%

	KxObject *gdkevent = kxgdkevent_new_prototype(KXCORE);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Event"),gdkevent);
	kxcore_add_prototype(KXCORE, gdkevent);


%init_prototypes%

%constantsContainersInit%

%constants%
	
%constantsContainersEnd%

	kxgtk_add_method_table(self);
	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
%remove_prototypes%
  	kxcore_remove_prototype(KXCORE, &kxgdkevent_extension);

	kxgtk_utils_unload(KXCORE);
}

static KxObject *
kxgtk_main(KxObject *self, KxMessage *message)
{
	gtk_main();
	KXRETURN(self);
}

static KxObject *
kxgtk_main_quit(KxObject *self, KxMessage *message)
{
	gtk_main_quit();
	KXRETURN(self);
}

static void
kxgtk_add_method_table(KxObject *self)
{
	KxMethodTable table[] = {
		{"main",0, kxgtk_main},
		{"mainQuit",0, kxgtk_main_quit},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
