#include <stdlib.h>

#include "kxmessage.h"
#include "kxobject.h"
#include "kxcfunction.h"
#include "kxinteger.h"
#include "kxfloat.h"
#include "kxstring.h"
#include "kxexception.h"
#include "gdkevent.h"

//#include "gtklabel.h"

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

KxObjectExtension kxgtklabel_extension;

static void kxgtklabel_free(KxObject *self)
{
	//g_object_remove_toggle_ref(self->data.ptr);
	//g_object_unref(self->data.ptr);
	kxgtk_remove_wrapper(self);
	//kxgtk_remove_wrapper(self);
}

static void kxgtklabel_mark(KxObject *self)
{
	kxgtk_mark_container(self->data.ptr);
	kxgtk_mark_watched_closures(self->data.ptr);
}

void kxgtklabel_extension_init() {
	kxobjectext_init(&kxgtklabel_extension);
	kxgtklabel_extension.type_name = "GtkLabel";
	kxgtklabel_extension.parent = &kxgtkmisc_extension;
	kxgtklabel_extension.free = kxgtklabel_free;
	kxgtklabel_extension.mark = kxgtklabel_mark;
//%init_extension%
}


static void
kxgtklabel_add_method_table(KxGtkLabel *self);

KxObject *
kxgtklabel_new_prototype(KxObject *parent)
{
	KxObject *self = kxobject_raw_clone(parent);
	self->extension = &kxgtklabel_extension;

	
	kxgtklabel_add_method_table(self);

	
	self->data.ptr = gtk_label_new("Label");
	kxgtk_set_wrapper(self, self->data.ptr);


	return self;
}


KxObject *
kxgtklabel_from(KxCore *core, GtkLabel* data)
{
	
	KxObject *self = kxgtk_check_wrapper((GObject*)data);
	if (self != NULL) {
		return self;
	}

	KxObject *proto = kxcore_get_prototype(core, &kxgtklabel_extension);
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

	KxObject *proto = kxcore_get_prototype(core, &kxgtklabel_extension);
	self = kxobject_raw_clone(proto);

	//g_object_ref_sink(data);
	self->data.ptr = data;

	kxgtk_set_wrapper(self, G_OBJECT(data));

	return self;
*/	
}

static KxObject *
gtk_label_clone (KxObject *self, KxMessage *message)
{
	KxObject *clone = kxobject_raw_clone(self);
	clone->data.ptr = gtk_label_new("Label");
	kxgtk_set_wrapper(clone, clone->data.ptr);
	return clone;
}
static KxObject *
kxgtklabel_text_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_text(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_markup_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_markup(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_markupWithMnemonic_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_markup_with_mnemonic(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_pattern_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_pattern(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_widthChars_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_label_set_width_chars(self->data.ptr,(gint)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_maxWidthChars_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	gtk_label_set_max_width_chars(self->data.ptr,(gint)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_parseUline_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXINTEGER((guint)gtk_label_parse_uline(self->data.ptr,(gchar*)param0));
}
static KxObject *
kxgtklabel_lineWrap_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_label_set_line_wrap(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_getMnemonicKeyval (KxObject *self, KxMessage *message)
{

	return KXINTEGER((guint)gtk_label_get_mnemonic_keyval(self->data.ptr));
}
static KxObject *
kxgtklabel_isSelectable (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_label_get_selectable(self->data.ptr));
}
static KxObject *
kxgtklabel_getText (KxObject *self, KxMessage *message)
{

	return KXSTRING((gchar*)gtk_label_get_text(self->data.ptr));
}
static KxObject *
kxgtklabel_cloneWithMnemonic_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	return KXGTKWIDGET((GtkWidget*)gtk_label_new_with_mnemonic((gchar*)param0));
}
static KxObject *
kxgtklabel_selectRegionStart_End_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_INT(param0,0);
	KXPARAM_TO_INT(param1,1);
	gtk_label_select_region(self->data.ptr,(gint)param0,(gint)param1);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_mnemonicWidget_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GTKWIDGET(param0,0);
	gtk_label_set_mnemonic_widget(self->data.ptr,(GtkWidget*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_selectable_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_label_set_selectable(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_textWithMnemonic_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_text_with_mnemonic(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_getWidthChars (KxObject *self, KxMessage *message)
{

	return KXINTEGER((gint)gtk_label_get_width_chars(self->data.ptr));
}
static KxObject *
kxgtklabel_getMaxWidthChars (KxObject *self, KxMessage *message)
{

	return KXINTEGER((gint)gtk_label_get_max_width_chars(self->data.ptr));
}
static KxObject *
kxgtklabel_getLabel (KxObject *self, KxMessage *message)
{

	return KXSTRING((gchar*)gtk_label_get_label(self->data.ptr));
}
static KxObject *
kxgtklabel_isLineWrap (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_label_get_line_wrap(self->data.ptr));
}
static KxObject *
kxgtklabel_getMnemonicWidget (KxObject *self, KxMessage *message)
{

	return KXGTKWIDGET((GtkWidget*)gtk_label_get_mnemonic_widget(self->data.ptr));
}
static KxObject *
kxgtklabel_isUseMarkup (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_label_get_use_markup(self->data.ptr));
}
static KxObject *
kxgtklabel_isUseUnderline (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_label_get_use_underline(self->data.ptr));
}
static KxObject *
kxgtklabel_isSingleLineMode (KxObject *self, KxMessage *message)
{

	return KXGBOOLEAN((gboolean)gtk_label_get_single_line_mode(self->data.ptr));
}
static KxObject *
kxgtklabel_getAngle (KxObject *self, KxMessage *message)
{

	return KXFLOAT((gdouble)gtk_label_get_angle(self->data.ptr));
}
static KxObject *
kxgtklabel_label_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param0,0);
	gtk_label_set_label(self->data.ptr,(gchar*)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_useMarkup_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_label_set_use_markup(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_useUnderline_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_label_set_use_underline(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_singleLineMode_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_GBOOLEAN(param0,0);
	gtk_label_set_single_line_mode(self->data.ptr,(gboolean)param0);
	KXRETURN(self);
}
static KxObject *
kxgtklabel_angle_ (KxObject *self, KxMessage *message)
{
	KXPARAM_TO_DOUBLE(param0,0);
	gtk_label_set_angle(self->data.ptr,(gdouble)param0);
	KXRETURN(self);
}



static void
kxgtklabel_add_method_table(KxObject *self)
{
	KxObjectExtension *ext = self->extension;
	self->extension = NULL;

	KxMethodTable table[] = {
		{"clone",0,gtk_label_clone},
		{"text:",1,kxgtklabel_text_},
		{"markup:",1,kxgtklabel_markup_},
		{"markupWithMnemonic:",1,kxgtklabel_markupWithMnemonic_},
		{"pattern:",1,kxgtklabel_pattern_},
		{"widthChars:",1,kxgtklabel_widthChars_},
		{"maxWidthChars:",1,kxgtklabel_maxWidthChars_},
		{"parseUline:",1,kxgtklabel_parseUline_},
		{"lineWrap:",1,kxgtklabel_lineWrap_},
		{"getMnemonicKeyval",0,kxgtklabel_getMnemonicKeyval},
		{"isSelectable",0,kxgtklabel_isSelectable},
		{"getText",0,kxgtklabel_getText},
		{"cloneWithMnemonic:",1,kxgtklabel_cloneWithMnemonic_},
		{"selectRegionStart:End:",2,kxgtklabel_selectRegionStart_End_},
		{"mnemonicWidget:",1,kxgtklabel_mnemonicWidget_},
		{"selectable:",1,kxgtklabel_selectable_},
		{"textWithMnemonic:",1,kxgtklabel_textWithMnemonic_},
		{"getWidthChars",0,kxgtklabel_getWidthChars},
		{"getMaxWidthChars",0,kxgtklabel_getMaxWidthChars},
		{"getLabel",0,kxgtklabel_getLabel},
		{"isLineWrap",0,kxgtklabel_isLineWrap},
		{"getMnemonicWidget",0,kxgtklabel_getMnemonicWidget},
		{"isUseMarkup",0,kxgtklabel_isUseMarkup},
		{"isUseUnderline",0,kxgtklabel_isUseUnderline},
		{"isSingleLineMode",0,kxgtklabel_isSingleLineMode},
		{"getAngle",0,kxgtklabel_getAngle},
		{"label:",1,kxgtklabel_label_},
		{"useMarkup:",1,kxgtklabel_useMarkup_},
		{"useUnderline:",1,kxgtklabel_useUnderline_},
		{"singleLineMode:",1,kxgtklabel_singleLineMode_},
		{"angle:",1,kxgtklabel_angle_},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
	self->extension = ext;
}
