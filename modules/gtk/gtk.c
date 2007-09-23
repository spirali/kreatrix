#include <gtk/gtk.h>

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

#include "gobject.h"
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
#include "gtkmisc.h"
#include "gtklabel.h"
#include "gtkaccelgroup.h"
#include "gtkrequisition.h"
#include "gdkgeometry.h"
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
	kxgobject_extension_init();
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
	kxgtkmisc_extension_init();
	kxgtklabel_extension_init();
	kxgtkaccelgroup_extension_init();
	kxgtkrequisition_extension_init();
	kxgdkgeometry_extension_init();


	KxObject *gdkevent = kxgdkevent_new_prototype(KXCORE);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Event"),gdkevent);
	kxcore_add_prototype(KXCORE, gdkevent);


	KxObject *gobject = kxgobject_new_prototype(KXCORE->base_object);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("ject"),gobject);

	KxObject *gtkobject = kxgtkobject_new_prototype(gobject);
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

	KxObject *gtkmisc = kxgtkmisc_new_prototype(gtkwidget);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Misc"),gtkmisc);

	KxObject *gtklabel = kxgtklabel_new_prototype(gtkmisc);
	kxcore_add_prototype(KXCORE, gtklabel);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Label"),gtklabel);

	KxObject *gtkaccelgroup = kxgtkaccelgroup_new_prototype(gobject);
	kxcore_add_prototype(KXCORE, gtkaccelgroup);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("AccelGroup"),gtkaccelgroup);

	KxObject *gtkrequisition = kxgtkrequisition_new_prototype(KXCORE->base_object);
	kxcore_add_prototype(KXCORE, gtkrequisition);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Requisition"),gtkrequisition);

	KxObject *gdkgeometry = kxgdkgeometry_new_prototype(KXCORE->base_object);
	kxcore_add_prototype(KXCORE, gdkgeometry);
	kxobject_set_slot_no_ref2(self, KXSYMBOL("Geometry"),gdkgeometry);



	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TOPLEVEL"),KXINTEGER(GTK_WINDOW_TOPLEVEL));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_POPUP"),KXINTEGER(GTK_WINDOW_POPUP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WIN_POS_NONE"),KXINTEGER(GTK_WIN_POS_NONE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WIN_POS_CENTER"),KXINTEGER(GTK_WIN_POS_CENTER));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WIN_POS_MOUSE"),KXINTEGER(GTK_WIN_POS_MOUSE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WIN_POS_CENTER_ALWAYS"),KXINTEGER(GTK_WIN_POS_CENTER_ALWAYS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WIN_POS_CENTER_ON_PARENT"),KXINTEGER(GTK_WIN_POS_CENTER_ON_PARENT));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SORT_ASCENDING"),KXINTEGER(GTK_SORT_ASCENDING));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SORT_DESCENDING"),KXINTEGER(GTK_SORT_DESCENDING));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_NORTH_WEST"),KXINTEGER(GDK_GRAVITY_NORTH_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_NORTH"),KXINTEGER(GDK_GRAVITY_NORTH));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_NORTH_EAST"),KXINTEGER(GDK_GRAVITY_NORTH_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_WEST"),KXINTEGER(GDK_GRAVITY_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_CENTER"),KXINTEGER(GDK_GRAVITY_CENTER));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_EAST"),KXINTEGER(GDK_GRAVITY_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_SOUTH_WEST"),KXINTEGER(GDK_GRAVITY_SOUTH_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_SOUTH"),KXINTEGER(GDK_GRAVITY_SOUTH));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_SOUTH_EAST"),KXINTEGER(GDK_GRAVITY_SOUTH_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAVITY_STATIC"),KXINTEGER(GDK_GRAVITY_STATIC));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SHIFT_MASK"),KXINTEGER(GDK_SHIFT_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("LOCK_MASK"),KXINTEGER(GDK_LOCK_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("CONTROL_MASK"),KXINTEGER(GDK_CONTROL_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOD1_MASK"),KXINTEGER(GDK_MOD1_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOD2_MASK"),KXINTEGER(GDK_MOD2_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOD3_MASK"),KXINTEGER(GDK_MOD3_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOD4_MASK"),KXINTEGER(GDK_MOD4_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOD5_MASK"),KXINTEGER(GDK_MOD5_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON1_MASK"),KXINTEGER(GDK_BUTTON1_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON2_MASK"),KXINTEGER(GDK_BUTTON2_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON3_MASK"),KXINTEGER(GDK_BUTTON3_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON4_MASK"),KXINTEGER(GDK_BUTTON4_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON5_MASK"),KXINTEGER(GDK_BUTTON5_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SUPER_MASK"),KXINTEGER(GDK_SUPER_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HYPER_MASK"),KXINTEGER(GDK_HYPER_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("META_MASK"),KXINTEGER(GDK_META_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("RELEASE_MASK"),KXINTEGER(GDK_RELEASE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MODIFIER_MASK"),KXINTEGER(GDK_MODIFIER_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_NORTH_WEST"),KXINTEGER(GDK_WINDOW_EDGE_NORTH_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_NORTH"),KXINTEGER(GDK_WINDOW_EDGE_NORTH));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_NORTH_EAST"),KXINTEGER(GDK_WINDOW_EDGE_NORTH_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_WEST"),KXINTEGER(GDK_WINDOW_EDGE_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_EAST"),KXINTEGER(GDK_WINDOW_EDGE_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_SOUTH_WEST"),KXINTEGER(GDK_WINDOW_EDGE_SOUTH_WEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_SOUTH"),KXINTEGER(GDK_WINDOW_EDGE_SOUTH));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_EDGE_SOUTH_EAST"),KXINTEGER(GDK_WINDOW_EDGE_SOUTH_EAST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_NORMAL"),KXINTEGER(GDK_WINDOW_TYPE_HINT_NORMAL));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_DIALOG"),KXINTEGER(GDK_WINDOW_TYPE_HINT_DIALOG));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_MENU"),KXINTEGER(GDK_WINDOW_TYPE_HINT_MENU));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_TOOLBAR"),KXINTEGER(GDK_WINDOW_TYPE_HINT_TOOLBAR));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_SPLASHSCREEN"),KXINTEGER(GDK_WINDOW_TYPE_HINT_SPLASHSCREEN));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_UTILITY"),KXINTEGER(GDK_WINDOW_TYPE_HINT_UTILITY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_DOCK"),KXINTEGER(GDK_WINDOW_TYPE_HINT_DOCK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_DESKTOP"),KXINTEGER(GDK_WINDOW_TYPE_HINT_DESKTOP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_DROPDOWN_MENU"),KXINTEGER(GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_POPUP_MENU"),KXINTEGER(GDK_WINDOW_TYPE_HINT_POPUP_MENU));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_TOOLTIP"),KXINTEGER(GDK_WINDOW_TYPE_HINT_TOOLTIP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_NOTIFICATION"),KXINTEGER(GDK_WINDOW_TYPE_HINT_NOTIFICATION));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_COMBO"),KXINTEGER(GDK_WINDOW_TYPE_HINT_COMBO));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_TYPE_HINT_DND"),KXINTEGER(GDK_WINDOW_TYPE_HINT_DND));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_POS"),KXINTEGER(GDK_HINT_POS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_MIN_SIZE"),KXINTEGER(GDK_HINT_MIN_SIZE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_MAX_SIZE"),KXINTEGER(GDK_HINT_MAX_SIZE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_BASE_SIZE"),KXINTEGER(GDK_HINT_BASE_SIZE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_ASPECT"),KXINTEGER(GDK_HINT_ASPECT));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_RESIZE_INC"),KXINTEGER(GDK_HINT_RESIZE_INC));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_WIN_GRAVITY"),KXINTEGER(GDK_HINT_WIN_GRAVITY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_USER_POS"),KXINTEGER(GDK_HINT_USER_POS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("HINT_USER_SIZE"),KXINTEGER(GDK_HINT_USER_SIZE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("NOTHING"),KXINTEGER(GDK_NOTHING));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DELETE"),KXINTEGER(GDK_DELETE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DESTROY"),KXINTEGER(GDK_DESTROY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("EXPOSE"),KXINTEGER(GDK_EXPOSE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MOTION_NOTIFY"),KXINTEGER(GDK_MOTION_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON_PRESS"),KXINTEGER(GDK_BUTTON_PRESS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("2BUTTON_PRESS"),KXINTEGER(GDK_2BUTTON_PRESS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("3BUTTON_PRESS"),KXINTEGER(GDK_3BUTTON_PRESS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON_RELEASE"),KXINTEGER(GDK_BUTTON_RELEASE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("KEY_PRESS"),KXINTEGER(GDK_KEY_PRESS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("KEY_RELEASE"),KXINTEGER(GDK_KEY_RELEASE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("ENTER_NOTIFY"),KXINTEGER(GDK_ENTER_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("LEAVE_NOTIFY"),KXINTEGER(GDK_LEAVE_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("FOCUS_CHANGE"),KXINTEGER(GDK_FOCUS_CHANGE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("CONFIGURE"),KXINTEGER(GDK_CONFIGURE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("MAP"),KXINTEGER(GDK_MAP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("UNMAP"),KXINTEGER(GDK_UNMAP));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROPERTY_NOTIFY"),KXINTEGER(GDK_PROPERTY_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SELECTION_CLEAR"),KXINTEGER(GDK_SELECTION_CLEAR));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SELECTION_REQUEST"),KXINTEGER(GDK_SELECTION_REQUEST));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SELECTION_NOTIFY"),KXINTEGER(GDK_SELECTION_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROXIMITY_IN"),KXINTEGER(GDK_PROXIMITY_IN));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROXIMITY_OUT"),KXINTEGER(GDK_PROXIMITY_OUT));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DRAG_ENTER"),KXINTEGER(GDK_DRAG_ENTER));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DRAG_LEAVE"),KXINTEGER(GDK_DRAG_LEAVE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DRAG_MOTION"),KXINTEGER(GDK_DRAG_MOTION));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DRAG_STATUS"),KXINTEGER(GDK_DRAG_STATUS));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DROP_START"),KXINTEGER(GDK_DROP_START));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("DROP_FINISHED"),KXINTEGER(GDK_DROP_FINISHED));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("CLIENT_EVENT"),KXINTEGER(GDK_CLIENT_EVENT));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("VISIBILITY_NOTIFY"),KXINTEGER(GDK_VISIBILITY_NOTIFY));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("NO_EXPOSE"),KXINTEGER(GDK_NO_EXPOSE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SCROLL"),KXINTEGER(GDK_SCROLL));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("WINDOW_STATE"),KXINTEGER(GDK_WINDOW_STATE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SETTING"),KXINTEGER(GDK_SETTING));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("OWNER_CHANGE"),KXINTEGER(GDK_OWNER_CHANGE));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("GRAB_BROKEN"),KXINTEGER(GDK_GRAB_BROKEN));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("EXPOSURE_MASK"),KXINTEGER(GDK_EXPOSURE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POINTER_MOTION_MASK"),KXINTEGER(GDK_POINTER_MOTION_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("POINTER_MOTION_HINT_MASK"),KXINTEGER(GDK_POINTER_MOTION_HINT_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON_MOTION_MASK"),KXINTEGER(GDK_BUTTON_MOTION_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON1_MOTION_MASK"),KXINTEGER(GDK_BUTTON1_MOTION_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON2_MOTION_MASK"),KXINTEGER(GDK_BUTTON2_MOTION_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON3_MOTION_MASK"),KXINTEGER(GDK_BUTTON3_MOTION_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON_PRESS_MASK"),KXINTEGER(GDK_BUTTON_PRESS_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("BUTTON_RELEASE_MASK"),KXINTEGER(GDK_BUTTON_RELEASE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("KEY_PRESS_MASK"),KXINTEGER(GDK_KEY_PRESS_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("KEY_RELEASE_MASK"),KXINTEGER(GDK_KEY_RELEASE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("ENTER_NOTIFY_MASK"),KXINTEGER(GDK_ENTER_NOTIFY_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("LEAVE_NOTIFY_MASK"),KXINTEGER(GDK_LEAVE_NOTIFY_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("FOCUS_CHANGE_MASK"),KXINTEGER(GDK_FOCUS_CHANGE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("STRUCTURE_MASK"),KXINTEGER(GDK_STRUCTURE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROPERTY_CHANGE_MASK"),KXINTEGER(GDK_PROPERTY_CHANGE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("VISIBILITY_NOTIFY_MASK"),KXINTEGER(GDK_VISIBILITY_NOTIFY_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROXIMITY_IN_MASK"),KXINTEGER(GDK_PROXIMITY_IN_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("PROXIMITY_OUT_MASK"),KXINTEGER(GDK_PROXIMITY_OUT_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SUBSTRUCTURE_MASK"),KXINTEGER(GDK_SUBSTRUCTURE_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("SCROLL_MASK"),KXINTEGER(GDK_SCROLL_MASK));
	kxobject_set_slot_no_ref2(self, KXSYMBOL("ALL_EVENTS_MASK"),KXINTEGER(GDK_ALL_EVENTS_MASK));


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
	
	kxcore_remove_prototype(KXCORE, &kxgtklabel_extension);
	kxcore_remove_prototype(KXCORE, &kxgtkaccelgroup_extension);
	kxcore_remove_prototype(KXCORE, &kxgtkrequisition_extension);
	kxcore_remove_prototype(KXCORE, &kxgdkgeometry_extension);

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
