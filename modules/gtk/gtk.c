#include <gtk/gtk.h>

#include "kxmodule.h"
#include "kxmessage.h"
#include "kxcfunction.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxstring.h"
#include "kxcore.h"
#include "gdkevent.h"

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

static void kxgtk_add_method_table(KxObject *self);


KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{
	List *list = list_new();
	kxcore_set_global_data(KXCORE,"kxgtk-window-list", list);

	gtk_init(0, NULL);
	gtk_utils_init();

	kxgdkevent_extension_init();
	kxgtkobject_extension_init();
	kxgtkwidget_extension_init();
	kxgtkcontainer_extension_init();
	kxgtkbin_extension_init();
	kxgtkwindow_extension_init();
	kxgtkbutton_extension_init();
	kxgtkcheckbutton_extension_init();
	kxgtkbox_extension_init();
	kxgtkvbox_extension_init();
	kxgtkhbox_extension_init();


	KxObject *gdkevent = kxgdkevent_new_prototype(KXCORE);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Event"),gdkevent);
	kxcore_add_prototype(KXCORE, gdkevent);


	KxObject *gtkobject = kxgtkobject_new_prototype(KXCORE->base_object);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Object"),gtkobject);

	KxObject *gtkwidget = kxgtkwidget_new_prototype(gtkobject);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Widget"),gtkwidget);

	KxObject *gtkcontainer = kxgtkcontainer_new_prototype(gtkwidget);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Container"),gtkcontainer);

	KxObject *gtkbin = kxgtkbin_new_prototype(gtkcontainer);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Bin"),gtkbin);

	KxObject *gtkwindow = kxgtkwindow_new_prototype(gtkbin);
	kxcore_add_prototype(KXCORE, gtkwindow);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Window"),gtkwindow);

	KxObject *gtkbutton = kxgtkbutton_new_prototype(gtkbin);
	kxcore_add_prototype(KXCORE, gtkbutton);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Button"),gtkbutton);

	KxObject *gtkcheckbutton = kxgtkcheckbutton_new_prototype(gtkbutton);
	kxcore_add_prototype(KXCORE, gtkcheckbutton);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("CheckButton"),gtkcheckbutton);

	KxObject *gtkbox = kxgtkbox_new_prototype(gtkcontainer);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Box"),gtkbox);

	KxObject *gtkvbox = kxgtkvbox_new_prototype(gtkbox);
	kxcore_add_prototype(KXCORE, gtkvbox);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("VBox"),gtkvbox);

	KxObject *gtkhbox = kxgtkhbox_new_prototype(gtkbox);
	kxcore_add_prototype(KXCORE, gtkhbox);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HBox"),gtkhbox);



	kxgtk_add_method_table(self);
	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
	
	
	
	
	kxcore_remove_prototype(KXCORE, &kxgtkwindow_extension);
	kxcore_remove_prototype(KXCORE, &kxgtkbutton_extension);
	kxcore_remove_prototype(KXCORE, &kxgtkcheckbutton_extension);
	
	kxcore_remove_prototype(KXCORE, &kxgtkvbox_extension);
	kxcore_remove_prototype(KXCORE, &kxgtkhbox_extension);

  	kxcore_remove_prototype(KXCORE, &kxgdkevent_extension);
	List *list = 
		(List*) kxcore_get_global_data(KXCORE, "kxgtk-window-list");
	kxcore_remove_global_data(KXCORE,"kxgtk-window-list");

	int t;
	for (t=0;t<list->size;t++) {
		gtk_widget_destroy(GTK_WIDGET(list->items[t]));
	}
	list_free(list);
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
