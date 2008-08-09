/*
   kxstring.c
   Copyright (C) 2007, 2008  Stanislav Bohm

   This file is part of Kreatrix.

   Kreatrix is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2, or (at your option) any later
   version.

   Kreatrix is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty 
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Kreatrix; see the file COPYING. If not, see 
   <http://www.gnu.org/licenses/>.
*/
	
/*KXobject Base String
[Basic prototypes]	Prototype of string.
 Object String is prototype for objects which represents character strings. All strings literals are clones of this object.

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
//#include <regex.h>

#include "kxstring.h"
#include "kxobject.h"
#include "kxexception.h"
#include "kxbytearray.h"
#include "kxinteger.h"
#include "utils/utils.h"
#include "utils/list.h"
#include "kxlist.h"
#include "kxcharacter.h"
#include "kxmessage.h"

KxObjectExtension kxstring_extension;

static void kxstring_add_method_table(KxString *self);
static void kxstring_free(KxString *self);

void
kxstring_init_extension() 
{
	kxobjectext_init(&kxstring_extension);
	kxstring_extension.type_name = "String";
	kxstring_extension.free = kxstring_free;
}

static void 
kxstring_free(KxString *self) 
{
	kxfree(KXSTRING_VALUE(self));
}


KxObject *
kxstring_new_prototype(KxCore *core) 
{
	KxObject *object = kxcore_clone_base_object(core);


	object->extension = &kxstring_extension;
	object->data.ptr = strdup("");

	kxstring_add_method_table(object);

	return object;
}

/*KXdoc , anObject
  Concatenates receiver and parameter.

  Sends asString to anObject. 
  Return new string, receiver followed by string representation of parameter.
*/
KxObject *
kxstring_concat(KxString *self, KxMessage *message)
{
	
	KxObject *paramobj = message->params[0];
	
	KxObject *string;
	if (!IS_KXSTRING(paramobj)) {
		KxSymbol *message_name = KXCORE->dictionary[KXDICT_AS_STRING];
		string = kxobject_send_unary_message(paramobj,message_name);
		KXCHECK(string);
	
		if (!IS_KXSTRING(string)) {
			KXTHROW_EXCEPTION("message asString to parameter hasn't returned String object");
		}

	} else {
		string = paramobj;
		REF_ADD(string);
	}
	
	char *param = KXSTRING_VALUE(string);

	char *selfstring = KXSTRING_VALUE(self);
	char newstring[strlen(selfstring)+strlen(param)+1];
	strcpy(newstring,selfstring);
	strcat(newstring,param);
	
	REF_REMOVE(string);
	return KXSTRING(newstring);
}

KxString *
kxstring_clone_with(KxString *self, char *str)
{
	KxString *string = kxobject_raw_clone(self);
	string->data.ptr = strdup(str);

	return string;
}

// With strdup of value
KxString *
kxstring_new_with(KxCore *core, char *str) 
{
	KxObject *prototype = kxcore_get_basic_prototype(core, KXPROTO_STRING);
	KxString *string = kxobject_raw_clone(prototype);
	string->data.ptr = strdup(str);
	return string;
}

// Without strdup of value
KxString *
kxstring_from_cstring(KxCore *core, char *str) 
{
	KxObject *prototype = kxcore_get_basic_prototype(core, KXPROTO_STRING);
	KxString *string = kxobject_raw_clone(prototype);
	string->data.ptr = str;
	return string;
}

static KxObject *
kxstring_copy(KxString *self, KxMessage *message)
{
	KxObject *new = kxobject_raw_copy(self);
	new->data.ptr = strdup(self->data.ptr);
	return new;
}

/*KXdoc asString
  Returns self.
 */
static KxObject *
kxstring_as_string(KxString *self, KxMessage *message)
{
	KXRETURN(self);
}

/*KXdoc asSymbol
  Return receiver as symbol.
  Return symbol with same characters sequence as target.
 */
static KxObject *
kxstring_as_symbol(KxString *self, KxMessage *message)
{
	return kxcore_get_symbol(KXCORE,self->data.ptr);
}

/*KXdoc asByteArray
  Returns string as bytearray.
  Returns bytearray which represents string. (without ending zero)
 */
static KxObject *
kxstring_as_bytearray(KxString *self, KxMessage *message)
{
	ByteArray *ba = bytearray_new_from_string(self->data.ptr);
	return KXBYTEARRAY(ba);
}

/*KXdoc size
  Return number of characters in string.
 */
static KxObject *
kxstring_size(KxString *self, KxMessage *message)
{
	 return KXINTEGER(strlen(self->data.ptr));
}


/*KXdoc bytes
  Return number of bytes in string.
 */
static KxObject *
kxstring_bytes(KxString *self, KxMessage *message)
{
	return KXINTEGER(strlen((char*) self->data.ptr));
}

/*KXdoc trimBegin
  Return copy of receiver without leading white chars.
*/
static
KxObject *
kxstring_trim_begin(KxString *self, KxMessage *message)
{
	char *str = KXSTRING_VALUE(self);
	while( *str != 0 && ((*str) == ' ' || (*str) == '\t' || (*str) == '\n' || (*str) == '\r')) { str++; };
	return KXSTRING(str);
}

/*KXdoc trimEnd
  Returns copy of receiver without trailing white chars.
*/
static
KxObject *
kxstring_trim_end(KxString *self, KxMessage *message)
{
	char *str = KXSTRING_VALUE(self);
	int len = strlen(str);
	int t;
	for (t=len-1;t>=0;t--) {
		if (str[t] != ' ' && str[t] != '\t' && str[t] != '\n' && str[t] != '\r') 
			break;
	}
	t++;
	char c = str[t];
	str[t] = 0;
	KxString *string =  KXSTRING(str);
	str[t] = c;
	return string;
}


/*KXdoc at: index
  Return character at position of parameter.
  Return character at index's position. If index is negative number, index is counted from end of string.
  Throws exception if index is out of range.
*/
static KxObject *
kxstring_at(KxString *self, KxMessage *message) 
{
	KXPARAM_TO_LONG(param,0);

	char *str = self->data.ptr;
	int len = strlen(str);

	if (param < 0) {
		param += len;
	}

	if (param < 0 || param >= len) {

		KxException *excp = kxexception_new_with_text(KXCORE,"Index out of range");
		KXTHROW(excp);
	}

	return KXCHARACTER(str[param]);
}

/*KXdoc beginsWith: aString
  Test beginning of string.
  Returns true if receiver starts with aString prefix.
*/
static KxObject *
kxstring_begins_with(KxString *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param,0);
	char *str = self->data.ptr;
	
	int l = strlen(param);
	if (strlen(str) < l) {
		KXRETURN(KXCORE->object_false);
	}
	
	int r = !memcmp(str, param, l);
	KXRETURN_BOOLEAN(r);
}

/*KXdoc endsWith: aString
  Test end of string.
  Returns true if receiver ends with aString radix.
*/
static KxObject *
kxstring_ends_with(KxString *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param,0);
	char *str = self->data.ptr;
	
	int l = strlen(param);
	int sl = strlen(str);
	if (sl < l) {
		KXRETURN(KXCORE->object_false);
	}
	sl -= l;
	int r = !memcmp(str + sl, param, l);
	KXRETURN_BOOLEAN(r);
}

/*KXdoc sliceFrom:to: anIndexStart anIndexEnd
  Return substring of receiver
  Returns substring begins at anIndexStart and ends at (anIndexEnd-1).
  If parameter is negative number than index is counted from end of string.

  Returns empty string if first index is higher position than second index.

  Throws exception if indexes are out of range.
*/
static KxObject *
kxstring_slice_from_to(KxString *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param0, 0);
	KXPARAM_TO_LONG(param1, 1);

	char *str = self->data.ptr;
	int len = strlen(str);
	
	if (param0 < 0)
		param0 += len;
	if (param1 < 0)
		param1 += len;

	if (param0 < 0 || param0 > len || param1 < 0 || param1 >len) {
		KxException *excp = kxexception_new_with_text(KXCORE,"Index out of range");
		KXTHROW(excp);
	}
	
	if (param0>=param1) {
		return KXSTRING("");
	}
	
	char c = str[param1];
	str[param1] = 0;
	KxString *s = KXSTRING(str+param0);
	str[param1] = c;
	return s;
	
}

/*KXdoc find: aString
  Find substring in receiver.
  Find first substring aString in target,
  Return index of substring or nil if target doesn't contain aString.
*/
static KxObject *
kxstring_find(KxString *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param, 0);

	char *str = self->data.ptr;
	char *f = strstr(str, param);
	if (f == NULL) {
		KXRETURN(KXCORE->object_nil);
	}
	int pos = f - str;
	return KXINTEGER(pos);
}

/*KXdoc find:from: aString anIndex
  Find substring in receiver from certain position.
  Finds first substring aString from anIndex in target,
  Returns index of first char of substring in target or nil if target doesn't contain aString.
  Returns nil if anIndex is out of range.
*/
static KxObject *
kxstring_find_from(KxString *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param, 0);
	KXPARAM_TO_LONG(index, 1);

	char *str = self->data.ptr;
	
	if (index < 0 || index >= strlen(str)) {
		KXRETURN(KXCORE->object_nil);
	}

	char *f = strstr(str+index, param);
	if (f == NULL) {
		KXRETURN(KXCORE->object_nil);
	}
	return KXINTEGER(f - str);

}

/*KXdoc asListBy: aString
  Parse receiver.
  Separate string on tokens by characters in aString. 
  Return list with those tokens.
*/
static KxObject *
kxstring_as_list_by(KxString *self, KxMessage *message)
{
	KXPARAM_TO_CSTRING(param,0);

	List *list = list_new();

	char *sp = strdup(self->data.ptr);
	char *p = sp;
	char *str;
	do {
		str = strtok(p, param);
		p = NULL;
		if (str) {
			KxString *string = KXSTRING(str);
	//		REF_ADD(string);
			list_append(list,string);
		}

	} while(str != NULL);
	kxfree(sp);
	return KXLIST(list);
}

/*KXdoc foreach: aBlock
 Evaluate block for each character.
 aBlock is evaluated for each character in receiver. aBlock is block with one parameter where 
 parameter is character from receiver. Characters are getted from left to right from string.
*/
static KxObject *
kxstring_foreach(KxString *self, KxMessage *message) {

	char *string = self->data.ptr;

	KxObject *param = message->params[0];

	int len = strlen(string);

	if (len == 0) {
		KXRETURN(KXCORE->object_nil);
	}

	KxMessage msg;
	//msg.stack = message->stack;
	msg.message_name = NULL;
	msg.params_count = 1;
	msg.target = self;

	int t;
	for (t=0;t<len-1;t++) {
		// TODO: recylace char
		
		KxCharacter *c = KXCHARACTER(string[t]);
		msg.params[0] = c;
		KxObject * retobj =  kxobject_evaluate_block(param,&msg);
		REF_REMOVE(c);
		KXCHECK(retobj);
		REF_REMOVE(retobj);
	}
	KxCharacter *c = KXCHARACTER(string[t]);
	msg.params[0] = c;
	KxObject * retobj =  kxobject_evaluate_block(param,&msg);
	REF_REMOVE(c);
	return retobj;
	
}

/*KXdoc asList
  Return receiver as list of characters.
  Returns list where items are characters from string.
*/
static KxObject *
kxstring_as_list(KxString *self, KxMessage *message) 
{

	char *string = self->data.ptr;
	int len = strlen(string);

	List *list = list_new_size(len);

	int t;
	for (t=0;t<len;t++) {
		KxCharacter *c = KXCHARACTER(string[t]);
		list->items[t] = c;
	}

	return KXLIST(list);
}

/*KXdoc last
  Return last character. 
  Return last character in receiver. If string is empty, exception is thrown.
*/
static KxObject *
kxstring_last(KxString *self, KxMessage *message) 
{
	char *str = self->data.ptr;
	int len = strlen(str);
	if (len <= 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"String is empty");
		KXTHROW(excp);
	}
	return KXCHARACTER(str[len-1]);
}

/*KXdoc first
  Return first character
  Returns first character in receiver. If string is empty, exception is thrown.
*/
static KxObject *
kxstring_first(KxString *self, KxMessage *message) 
{
	char *str = self->data.ptr;
	int len = strlen(str);
	if (len <= 0) {
		KxException *excp = kxexception_new_with_text(KXCORE,"String is empty");
		KXTHROW(excp);
	}
	return KXCHARACTER(str[0]);
}


/*KXdoc asInteger
  Convert string into integer. 
  Convert decimal representation of integer to Integer object. If receiver isn't representation of decimal integer, exception is thrown.
*/
static KxObject *
kxstring_as_integer(KxString *self, KxMessage *message) 
{
	char *str = self->data.ptr;
	char *err;
	KxInt intval = strtol(str,&err,10);
	if (*err != '\0') {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"String '%s' doesn't contain integer", str);
		KXTHROW(excp);
	}
	return KXINTEGER(intval);
}


/*KXdoc asIntegerInBase: anInteger
  Convert string into integer in certain base. 
*/
static KxObject *
kxstring_as_integer_in_base(KxString *self, KxMessage *message) 
{
	KXPARAM_TO_LONG(base,0);
	char *str = self->data.ptr;
	char *err;
	KxInt intval = strtol(str,&err,base);
	if (*err != '\0') {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"String doesn't contain integer in base %i",base);
		KXTHROW(excp);
	}
	return KXINTEGER(intval);
}

static KxObject *
kxstring_replace_to(KxString *self, KxMessage *message) 
{
	KXPARAM_TO_CSTRING(from,0);
	KXPARAM_TO_CSTRING(to,1);

	int from_len = strlen(from);
	int to_len = strlen(to);

	char *selfstr = KXSTRING_VALUE(self);
	int len = strlen(selfstr);

	int buffer_len = len+to_len+16;
	char *buffer = kxmalloc(buffer_len+1);
	ALLOCTEST(buffer);
	int buffer_pos = 0;
	int t;
	for (t=0;t<=len-from_len;t++) {
		int s;
		for (s=0;s<from_len;s++) {
			if (selfstr[t+s] != from[s]) 
				break;
		}

		if (s == from_len) {
			if (buffer_len <= buffer_pos+to_len) {
				buffer_len *= 2;
				buffer = kxrealloc(buffer, buffer_len+1);
				ALLOCTEST(buffer);
			}
			memcpy(buffer+buffer_pos,to,to_len);
			buffer_pos += to_len;
			t+=from_len-1;
		} else {
			if (buffer_len <= buffer_pos+1) {
				buffer_len *= 2;
				buffer = kxrealloc(buffer, buffer_len+1);
				ALLOCTEST(buffer);
			}
			buffer[buffer_pos++] = selfstr[t];
		}
	}
	int s;
	for (s = t; s < len; s++) {
			if (buffer_len <= buffer_pos+1) {
				buffer_len *= 2;
				buffer = kxrealloc(buffer, buffer_len+1);
				ALLOCTEST(buffer);
			}
			buffer[buffer_pos++] = selfstr[s];
	}
	buffer[buffer_pos] = 0;
	KxString *string = KXSTRING(buffer);
	kxfree(buffer);
	return string;
}

static int
kxstring_compare_helper(KxString *self, KxObject *object, KxInt *compare) 
{
	if (IS_KXSTRING(object) || IS_KXSYMBOL(object)) {
		*compare = strcmp(self->data.ptr, object->data.ptr);
		return 1;
	} 
	kxstack_throw_object(KXSTACK,kxobject_compare_exception(self, object));
	return 0;
}

static KxObject *
kxstring_eq(KxString *self, KxMessage *message) 
{
	if (IS_KXSTRING(message->params[0])) {
		KXRETURN_BOOLEAN(!strcmp(self->data.ptr, message->params[0]->data.ptr));
	} else {
		KXRETURN(KXCORE->object_false);
	}
}

static KxObject *
kxstring_neq(KxString *self, KxMessage *message) 
{
	if (IS_KXSTRING(message->params[0])) {
		KXRETURN_BOOLEAN(strcmp(self->data.ptr, message->params[0]->data.ptr));
	} else {
		KXRETURN(KXCORE->object_true);
	}
}

KxObject *
kxstring_lt(KxString *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxstring_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare < 0);
}


static KxObject *
kxstring_gt(KxString *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxstring_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare > 0);
}

static KxObject *
kxstring_lteq(KxString *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxstring_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare <= 0);
}

static KxObject *
kxstring_gteq(KxString *self, KxMessage *message) 
{
	KxInt compare;
	if (!kxstring_compare_helper(self, message->params[0], &compare)) {
		return NULL;
	}
	KXRETURN_BOOLEAN(compare >= 0);
}

static KxObject *
kxstring_hash(KxString *self, KxMessage *message) 
{
    unsigned long hash = 5381;

	char *str = KXSTRING_VALUE(self);
    
	int c;
    while ((c = *str++))
         hash = ((hash << 5) + hash) + c; 
	return KXINTEGER(hash);
}


/** For debugging without vm-init */
/*static KxObject *
kxstring_println(KxString *self, KxMessage *message)
{
	printf("%s\n", KXSTRING_VALUE(self));
	KXRETURN(self);
}*/


static KxObject *
kxstring_dirname(KxString *self, KxMessage *message) 
{
	char *copy = strdup(KXSTRING_VALUE(self));
	KxString *string = KXSTRING(dirname(copy));
	kxfree(copy);
	return string;
}

static KxObject *
kxstring_basename(KxString *self, KxMessage *message) 
{
	char *copy = strdup(KXSTRING_VALUE(self));
	KxString *string = KXSTRING(basename(copy));
	kxfree(copy);
	return string;
}

static KxObject *
convert_to_kxstring(KxObject *self, KxObject *message_name)
{
		KxObject *string = kxobject_send_unary_message(self,message_name);
		KXCHECK(string);
	
		if (!IS_KXSTRING(string)) {
			KxException *excp = kxexception_new_with_text(KXCORE, 
				"message %s to parameter hasn't returned String object", 
				KXSYMBOL_AS_CSTRING(message_name));
			KXTHROW(excp);
		}
		return string;
}

static KxObject *
kxstring_convert_for_substition(char type, KxObject *obj)
{
	switch(type) {
		case 's':
			if (IS_KXSTRING(obj)) {
				REF_ADD(obj);
				return obj;
			}
			return convert_to_kxstring(obj, KXCORE_FROM(obj)->dictionary[KXDICT_AS_STRING]);
		case 'p':
			return convert_to_kxstring(obj, KXCORE_FROM(obj)->dictionary[KXDICT_PRINTSTRING]);
		default: {
			KxException *excp = kxexception_new_with_text(KXCORE_FROM(obj), "Invalid substitution character '%c' in string", type);
			KXTHROW(excp);
		}
	}
}


static KxObject *
kxstring_substitution(KxString *self, KxMessage *message)
{
	char *str = KXSTRING_VALUE(self);
	char *c = str;
	int count = 0;
	int pos = 0;

	char type = 0;
	int index = 0;

	while((*c) != 0) {
		if (*c == '%') {
			c++;
			if (*c != '%') {
				count++;
				if (count == 2) {
					break;
				}
				index = pos;
				type = *c;
			}
			pos++;
		}
		pos++;
		c++;
	}

	if (count == 0) {
		KXTHROW_EXCEPTION("No substition point in string");
	}


	int len = strlen(str);

	KxObject *textobj = kxstring_convert_for_substition(type, message->params[0]);
	KXCHECK(textobj);
	char *text = KXSTRING_VALUE(textobj);
	int textlen = strlen(text);


	if (count == 1) {
		// Final substition
		char *new = kxmalloc(len + textlen + 1);
		char *pos = new;


		int t;
		for (t=0; t < index; t++) {
			*pos = str[t];
			pos++;
			if (str[t] == '%') {
				t++;
			}
		}
		memcpy(pos, text, textlen);
		pos += textlen;

		for (t=index+2; t <= len; t++) {
			*pos = str[t];
			pos++;
			if (str[t] == '%') {
				t++;
			}
		}
		REF_REMOVE(textobj);
		return kxstring_from_cstring(KXCORE, new);
		
	} else {
		char *pos = text;
		int extra_size = 1;
		while(*pos != 0) {
			if (*pos == '%') {
				extra_size++;
			}
			pos++;
		}

		char *new = kxmalloc(len + textlen + extra_size);


		pos = new + index;
		memcpy(new, str, index);

		if (extra_size == 1) { // No '%' in text
			memcpy(pos, text, textlen);
			pos += textlen;
		} else { // text contains '%'
			int t;
			for (t = 0; t < textlen; t++) {
				*pos = text[t];
				pos++;
				if (text[t] == '%') {
					*pos = '%';
					pos++;
				}
			}

		}

		memcpy(pos, str + index + 2, len - index - 1); 
		REF_REMOVE(textobj);
		return kxstring_from_cstring(KXCORE, new);

	}



	/*char *new = kxmalloc(len + lentext + 1);

	char *pos = new;
	while( (*str) != 0) {
		if (*str == '%') {
			str++;

			if (*str == 's') {
				memcpy(pos, text, lentext);
				pos+=lentext;
				strcpy(pos, str + 1);
				return new;
			}

			if (*str != '%') {
				kxfree(new);
				return NULL;
			}
		}
		*pos = *str;
		str++;
		pos++;
	}
	*pos = 0;
	return new;


	KXPARAM_TO_CSTRING(param, 0);
	char *str;
	char r = kxstring_substitute_in_c_string(KXSTRING_VALUE(self), param);

	if (r > 0) {
		return kxstring_from_cstring(KXCORE, str);
	}
	
	if (r == -) {
		KXTHROW_EXCEPTION("Invalid substitution character in string");
	}

	KXTHROW_EXCEPTION("No substition point in string");*/
}

static void 
kxstring_add_method_table(KxString *self)
{
	KxMethodTable table[] = {
	//	{"println", 0, kxstring_println},
		{",",1, kxstring_concat },
		{"asString",0, kxstring_as_string },
		{"==",1, kxstring_eq},
		{"!=",1, kxstring_neq},
		{"<",1, kxstring_lt},
		{">",1, kxstring_gt},
		{"<=",1, kxstring_lteq},
		{">=",1, kxstring_gteq},
		{"hash",0, kxstring_hash},
		{"asSymbol",0, kxstring_as_symbol },
		{"asByteArray",0, kxstring_as_bytearray },
		{"trimBegin",0, kxstring_trim_begin },
		{"trimEnd",0, kxstring_trim_end },
		{"size",0, kxstring_size },
		{"bytes",0, kxstring_bytes },
		{"at:",1, kxstring_at },
		{"sliceFrom:to:",2, kxstring_slice_from_to},
		{"find:",1, kxstring_find},
		{"find:from:",2, kxstring_find_from},
		{"beginsWith:",1, kxstring_begins_with },
		{"endsWith:",1, kxstring_ends_with },
		{"asListBy:",1, kxstring_as_list_by},
		{"foreach:",1,kxstring_foreach},
		{"asList",0, kxstring_as_list},
		{"last",0, kxstring_last},
		{"first",0, kxstring_first},
		{"asInteger",0, kxstring_as_integer},
		{"asIntegerInBase:",1, kxstring_as_integer_in_base},
		{"replace:to:",2, kxstring_replace_to},
		{"copy",0, kxstring_copy},
		{"%", 1, kxstring_substitution},
		{"dirname",0, kxstring_dirname},
		{"basename",0, kxstring_basename},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

