/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

#include <stdio.h>
#include <ctype.h>

#include "kxcharacter.h"
#include "kxobject.h"
#include "kxstring.h"
#include "kxinteger.h"

/*KXobject Base Character
 [Basic prototypes] 
*/


KxObjectExtension kxcharacter_extension;


static void kxcharacter_add_method_table(KxCharacter *self);
void kxcharacter_free(KxCharacter *self);
//static int kxcharacter_compare(KxCharacter *self, KxObject *param);


void
kxcharacter_init_extension() 
{
	kxobjectext_init(&kxcharacter_extension);
	kxcharacter_extension.type_name = "Character";
	kxcharacter_extension.is_immutable = 1;
}


KxObject *
kxcharacter_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);


	object->extension = &kxcharacter_extension;
	
	kxcharacter_add_method_table(object);

	return object;
}

KxCharacter * 
kxcharacter_new_with(KxCore *core, char ch)
{
	KxCharacter *self = kxobject_raw_clone(kxcore_get_basic_prototype(core, KXPROTO_CHARACTER));
	self->data.charval = ch;
	return self;
	
}


static KxObject *
kxcharacter_print(KxCharacter *self, KxMessage *message)
{
	char c = self->data.charval;
	printf("%c", c);
	KXRETURN(self);
}

static KxObject *
kxcharacter_as_string(KxCharacter *self, KxMessage *message)
{
 	char str[2];
	str[0] = self->data.charval;
	str[1] = 0;

	return KXSTRING(str);
}

static KxObject *
kxcharacter_as_integer(KxCharacter *self, KxMessage *message)
{
	return KXINTEGER(self->data.charval);
}


static KxObject *
kxcharacter_is_alpha_numeric(KxCharacter *self, KxMessage *message) 
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN(isalnum(c));
}

static KxObject *
kxcharacter_is_alpha(KxCharacter *self, KxMessage *message) 
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN(isalpha(c));
}

static KxObject *
kxcharacter_is_digit(KxCharacter *self, KxMessage *message) 
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN(isdigit(c));
}

static KxObject *
kxcharacter_is_blank(KxCharacter *self, KxMessage *message) 
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN( (c == ' ') || (c == '\t') || (c == '\n') );
}

static KxObject *
kxcharacter_eq(KxCharacter *self, KxMessage *message) 
{
	if (IS_KXCHARACTER(message->params[0])) {
		KXRETURN_BOOLEAN(KXCHARACTER_VALUE(self) == KXCHARACTER_VALUE(message->params[0]));
	} else {
		KXRETURN(KXCORE->object_false);
	}
}

static KxObject *
kxcharacter_lt(KxCharacter *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	if (IS_KXCHARACTER(param)) {
		KXRETURN_BOOLEAN(self->data.charval < param->data.charval);
	} else {
		kxobject_compare_exception(self, param);
		return NULL;
	}
}

static KxObject *
kxcharacter_hash(KxCharacter *self, KxMessage *message) 
{
	return KXINTEGER(self->data.charval);
}

static KxObject *
kxcharacter_as_lowercase(KxCharacter *self, KxMessage *message)
{
	int c = self->data.charval;
	return KXCHARACTER(tolower(c));
}

static KxObject *
kxcharacter_as_uppercase(KxCharacter *self, KxMessage *message)
{
	int c = self->data.charval;
	return KXCHARACTER(toupper(c));
}

static KxObject *
kxcharacter_is_uppercase(KxCharacter *self, KxMessage *message)
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN(isupper(c));
}

static KxObject *
kxcharacter_is_lowercase(KxCharacter *self, KxMessage *message)
{
	int c = self->data.charval;
	KXRETURN_BOOLEAN(islower(c));
}

static void
kxcharacter_add_method_table(KxCharacter *self) 
{
	KxMethodTable table[] = {
		{"==",1, kxcharacter_eq },
		{"hash",0, kxcharacter_hash },
		{"<",1, kxcharacter_lt },
		{"print",0, kxcharacter_print },
		{"asString",0, kxcharacter_as_string },
		{"isAlphaNumeric",0, kxcharacter_is_alpha_numeric },
		{"isAlpha",0, kxcharacter_is_alpha },
		{"isDigit",0, kxcharacter_is_digit },
		{"isBlank",0, kxcharacter_is_blank },		
		{"asLowercase",0, kxcharacter_as_lowercase },
		{"asUppercase",0, kxcharacter_as_uppercase },
		{"isLowercase",0, kxcharacter_is_lowercase },
		{"isUppercase",0, kxcharacter_is_uppercase },
		{"asInteger",0, kxcharacter_as_integer },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);


}
