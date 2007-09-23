#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtkwindow.h"

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

KxObjectExtension kxgtkwindow_extension;

static void kxgtkwindow_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtkwindow_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtkwindow_extension_init() {
	kxobjectext_init(&kxgtkwindow_extension);
	kxgtkwindow_extension.type_name = "GtkWindow";
	kxgtkwindow_extension.parent = &kxgtkbin_extension;
	kxgtkwindow_extension.free = kxgtkwindow_free;
	kxgtkwindow_extension.mark = kxgtkwindow_mark;
//%init_extension%
}


static void
kxgtkwindow_add_method_table(KxGtkWindow *self);

KxObject *
kxgtkwindow_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtkwindow_extension;

	
	kxgtkwindow_add_method_table(self);

	
	self->data.ptr = gtk_window_new(0);
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtkwindow_from(KxCore *core, GtkWindow* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkwindow_extension);
	self = kxobject_raw_clone(proto);
	g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;

/*
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtkwindow_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gtk_window_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_window_new(0);
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtkwindow_title_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_window_set_title(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_setWMName_WMClass_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	KXPARAM_TO_CSTRING(param1,1);
	gtk_window_set_wmclass(self->data.ptr,(gchar*)param0,(gchar*)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_setShrink_grow_autoShrink_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	KXPARAM_TO_INT(param2,2);
	gtk_window_set_policy(self->data.ptr,(gint)param0,(gint)param1,(gint)param2);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_resizable_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_resizable(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_isResizable (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_resizable(self->data.ptr));
}
static KxObject *
kxgtkwindow_addAccelGroup_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKACCELGROUP(param0,0);
	gtk_window_add_accel_group(self->data.ptr,(GtkAccelGroup*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_removeAccelGroup_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKACCELGROUP(param0,0);
	gtk_window_remove_accel_group(self->data.ptr,(GtkAccelGroup*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_activateFocus (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_activate_focus(self->data.ptr));
}
static KxObject *
kxgtkwindow_activateDefault (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_activate_default(self->data.ptr));
}
static KxObject *
kxgtkwindow_modal_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_modal(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_defaultWidth_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_window_set_default_size(self->data.ptr,(gint)param0,(gint)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_widget_geometry_hint_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	KXPARAM_TO_GDKGEOMETRY(param1,1);
	KXPARAM_TO_INT(param2,2);
	gtk_window_set_geometry_hints(self->data.ptr,(GtkWidget*)param0,(GdkGeometry*)param1,(GdkWindowHints)param2);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_gravity_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_window_set_gravity(self->data.ptr,(GdkGravity)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_getGravity (KxObject *self, KxMessage *message)
{

	return KXINTEGER((GdkGravity)gtk_window_get_gravity(self->data.ptr));
}
static KxObject *
kxgtkwindow_position_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_window_set_position(self->data.ptr,(GtkWindowPosition)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_transientFor_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWINDOW(param0,0);
	gtk_window_set_transient_for(self->data.ptr,(GtkWindow*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_destroyWithParent_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_destroy_with_parent(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_isActive (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_is_active(self->data.ptr));
}
static KxObject *
kxgtkwindow_hasToplevelFocus (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_has_toplevel_focus(self->data.ptr));
}
static KxObject *
kxgtkwindow_addMnemonic_target_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_GTKWIDGET(param1,1);
	gtk_window_add_mnemonic(self->data.ptr,(guint)param0,(GtkWidget*)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_removeMnemonic_target_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_GTKWIDGET(param1,1);
	gtk_window_remove_mnemonic(self->data.ptr,(guint)param0,(GtkWidget*)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_activeMnemonic_modifier_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	return KXGBOOLEAN((gboolean)gtk_window_mnemonic_activate(self->data.ptr,(guint)param0,(GdkModifierType)param1));
}
static KxObject *
kxgtkwindow_activateKey_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GDKEVENT(param0,0);
	return KXGBOOLEAN((gboolean)gtk_window_activate_key(self->data.ptr,(GdkEventKey*)param0));
}
static KxObject *
kxgtkwindow_propagateKeyEvent_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GDKEVENT(param0,0);
	return KXGBOOLEAN((gboolean)gtk_window_propagate_key_event(self->data.ptr,(GdkEventKey*)param0));
}
static KxObject *
kxgtkwindow_getFocus (KxObject *self, KxMessage *message)
{

	return KXGTKWIDGET((GtkWidget*)gtk_window_get_focus(self->data.ptr));
}
static KxObject *
kxgtkwindow_focus_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_window_set_focus(self->data.ptr,(GtkWidget*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_default_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_window_set_default(self->data.ptr,(GtkWidget*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_present (KxObject *self, KxMessage *message)
{

	gtk_window_present(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_presentWithTime_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_window_present_with_time(self->data.ptr,(guint32)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_iconify (KxObject *self, KxMessage *message)
{

	gtk_window_iconify(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_deiconify (KxObject *self, KxMessage *message)
{

	gtk_window_deiconify(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_stick (KxObject *self, KxMessage *message)
{

	gtk_window_stick(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_unstick (KxObject *self, KxMessage *message)
{

	gtk_window_unstick(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_maximize (KxObject *self, KxMessage *message)
{

	gtk_window_maximize(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_unmaximize (KxObject *self, KxMessage *message)
{

	gtk_window_unmaximize(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_fullscreen (KxObject *self, KxMessage *message)
{

	gtk_window_fullscreen(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_unfullscreen (KxObject *self, KxMessage *message)
{

	gtk_window_unfullscreen(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_keepAbove_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_keep_above(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_keepBelow_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_keep_below(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_decorated_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_decorated(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_deletable_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_deletable(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_setFrameLeft_top_right_bottom_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	KXPARAM_TO_INT(param2,2);
	KXPARAM_TO_INT(param3,3);
	gtk_window_set_frame_dimensions(self->data.ptr,(gint)param0,(gint)param1,(gint)param2,(gint)param3);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_hasFrame_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_has_frame(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_mnemonicModifier_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_window_set_mnemonic_modifier(self->data.ptr,(GdkModifierType)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_role_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_window_set_role(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_typeHint_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_window_set_type_hint(self->data.ptr,(GdkWindowTypeHint)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_skipTaskbarHint_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_skip_taskbar_hint(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_skipPagerHint_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_skip_pager_hint(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_urgencyHint_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_urgency_hint(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_acceptFocus_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_accept_focus(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_focusOnMap_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_window_set_focus_on_map(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_isDecorated (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_decorated(self->data.ptr));
}
static KxObject *
kxgtkwindow_isDeletable (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_deletable(self->data.ptr));
}
static KxObject *
kxgtkwindow_isDestroyWithParent (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_destroy_with_parent(self->data.ptr));
}
static KxObject *
kxgtkwindow_isHasFrame (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_has_frame(self->data.ptr));
}
static KxObject *
kxgtkwindow_getIconName (KxObject *self, KxMessage *message)
{

	return KXSTRING((gchar*)gtk_window_get_icon_name(self->data.ptr));
}
static KxObject *
kxgtkwindow_getMnemonicModifier (KxObject *self, KxMessage *message)
{

	return KXINTEGER((GdkModifierType)gtk_window_get_mnemonic_modifier(self->data.ptr));
}
static KxObject *
kxgtkwindow_isModal (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_modal(self->data.ptr));
}
static KxObject *
kxgtkwindow_getRole (KxObject *self, KxMessage *message)
{

	return KXSTRING((gchar*)gtk_window_get_role(self->data.ptr));
}
static KxObject *
kxgtkwindow_getTransientFor (KxObject *self, KxMessage *message)
{

	return KXGTKWINDOW((GtkWindow*)gtk_window_get_transient_for(self->data.ptr));
}
static KxObject *
kxgtkwindow_getTypeHint (KxObject *self, KxMessage *message)
{

	return KXINTEGER((GdkWindowTypeHint)gtk_window_get_type_hint(self->data.ptr));
}
static KxObject *
kxgtkwindow_isSkipTaskbarHint (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_skip_taskbar_hint(self->data.ptr));
}
static KxObject *
kxgtkwindow_isSkipPagerHint (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_skip_pager_hint(self->data.ptr));
}
static KxObject *
kxgtkwindow_isUrgencyHint (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_urgency_hint(self->data.ptr));
}
static KxObject *
kxgtkwindow_isAcceptFocus (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_accept_focus(self->data.ptr));
}
static KxObject *
kxgtkwindow_isFocusOnMap (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_window_get_focus_on_map(self->data.ptr));
}
static KxObject *
kxgtkwindow_moveX_y_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_window_move(self->data.ptr,(gint)param0,(gint)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_parseGeometry_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXGBOOLEAN((gboolean)gtk_window_parse_geometry(self->data.ptr,(gchar*)param0));
}
static KxObject *
kxgtkwindow_reshowWithInitialSize (KxObject *self, KxMessage *message)
{

	gtk_window_reshow_with_initial_size(self->data.ptr);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_resizeWidth_height_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_window_resize(self->data.ptr,(gint)param0,(gint)param1);
	KXRETURN(self);
}
static KxObject *
kxgtkwindow_iconName_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_window_set_icon_name(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}



static void
kxgtkwindow_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_window_clone},
		{"title:",1,kxgtkwindow_title_},
		{"setWMName:WMClass:",2,kxgtkwindow_setWMName_WMClass_},
		{"setShrink:grow:autoShrink:",3,kxgtkwindow_setShrink_grow_autoShrink_},
		{"resizable:",1,kxgtkwindow_resizable_},
		{"isResizable",0,kxgtkwindow_isResizable},
		{"addAccelGroup:",1,kxgtkwindow_addAccelGroup_},
		{"removeAccelGroup:",1,kxgtkwindow_removeAccelGroup_},
		{"activateFocus",0,kxgtkwindow_activateFocus},
		{"activateDefault",0,kxgtkwindow_activateDefault},
		{"modal:",1,kxgtkwindow_modal_},
		{"defaultWidth:height:",2,kxgtkwindow_defaultWidth_height_},
		{"widget:geometry:hint:",3,kxgtkwindow_widget_geometry_hint_},
		{"gravity:",1,kxgtkwindow_gravity_},
		{"getGravity",0,kxgtkwindow_getGravity},
		{"position:",1,kxgtkwindow_position_},
		{"transientFor:",1,kxgtkwindow_transientFor_},
		{"destroyWithParent:",1,kxgtkwindow_destroyWithParent_},
		{"isActive",0,kxgtkwindow_isActive},
		{"hasToplevelFocus",0,kxgtkwindow_hasToplevelFocus},
		{"addMnemonic:target:",2,kxgtkwindow_addMnemonic_target_},
		{"removeMnemonic:target:",2,kxgtkwindow_removeMnemonic_target_},
		{"activeMnemonic:modifier:",2,kxgtkwindow_activeMnemonic_modifier_},
		{"activateKey:",1,kxgtkwindow_activateKey_},
		{"propagateKeyEvent:",1,kxgtkwindow_propagateKeyEvent_},
		{"getFocus",0,kxgtkwindow_getFocus},
		{"focus:",1,kxgtkwindow_focus_},
		{"default:",1,kxgtkwindow_default_},
		{"present",0,kxgtkwindow_present},
		{"presentWithTime:",1,kxgtkwindow_presentWithTime_},
		{"iconify",0,kxgtkwindow_iconify},
		{"deiconify",0,kxgtkwindow_deiconify},
		{"stick",0,kxgtkwindow_stick},
		{"unstick",0,kxgtkwindow_unstick},
		{"maximize",0,kxgtkwindow_maximize},
		{"unmaximize",0,kxgtkwindow_unmaximize},
		{"fullscreen",0,kxgtkwindow_fullscreen},
		{"unfullscreen",0,kxgtkwindow_unfullscreen},
		{"keepAbove:",1,kxgtkwindow_keepAbove_},
		{"keepBelow:",1,kxgtkwindow_keepBelow_},
		{"decorated:",1,kxgtkwindow_decorated_},
		{"deletable:",1,kxgtkwindow_deletable_},
		{"setFrameLeft:top:right:bottom:",4,kxgtkwindow_setFrameLeft_top_right_bottom_},
		{"hasFrame:",1,kxgtkwindow_hasFrame_},
		{"mnemonicModifier:",1,kxgtkwindow_mnemonicModifier_},
		{"role:",1,kxgtkwindow_role_},
		{"typeHint:",1,kxgtkwindow_typeHint_},
		{"skipTaskbarHint:",1,kxgtkwindow_skipTaskbarHint_},
		{"skipPagerHint:",1,kxgtkwindow_skipPagerHint_},
		{"urgencyHint:",1,kxgtkwindow_urgencyHint_},
		{"acceptFocus:",1,kxgtkwindow_acceptFocus_},
		{"focusOnMap:",1,kxgtkwindow_focusOnMap_},
		{"isDecorated",0,kxgtkwindow_isDecorated},
		{"isDeletable",0,kxgtkwindow_isDeletable},
		{"isDestroyWithParent",0,kxgtkwindow_isDestroyWithParent},
		{"isHasFrame",0,kxgtkwindow_isHasFrame},
		{"getIconName",0,kxgtkwindow_getIconName},
		{"getMnemonicModifier",0,kxgtkwindow_getMnemonicModifier},
		{"isModal",0,kxgtkwindow_isModal},
		{"getRole",0,kxgtkwindow_getRole},
		{"getTransientFor",0,kxgtkwindow_getTransientFor},
		{"getTypeHint",0,kxgtkwindow_getTypeHint},
		{"isSkipTaskbarHint",0,kxgtkwindow_isSkipTaskbarHint},
		{"isSkipPagerHint",0,kxgtkwindow_isSkipPagerHint},
		{"isUrgencyHint",0,kxgtkwindow_isUrgencyHint},
		{"isAcceptFocus",0,kxgtkwindow_isAcceptFocus},
		{"isFocusOnMap",0,kxgtkwindow_isFocusOnMap},
		{"moveX:y:",2,kxgtkwindow_moveX_y_},
		{"parseGeometry:",1,kxgtkwindow_parseGeometry_},
		{"reshowWithInitialSize",0,kxgtkwindow_reshowWithInitialSize},
		{"resizeWidth:height:",2,kxgtkwindow_resizeWidth_height_},
		{"iconName:",1,kxgtkwindow_iconName_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
