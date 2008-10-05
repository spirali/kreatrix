/*
   kxparser.c
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
	
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "kxparser.h"

#define GET_CHAR (*self->pos)
#define NEXT_CHAR (*(++self->pos))
#define SKIP_CHAR ++self->pos;
#define SAFE_SKIP_CHAR if(*self->pos) ++self->pos;

#define IDENTIFIER_SIZE 1024
#define STRING_SIZE 4096

static int
is_white_char(char c) {
	return (((c) == ' ') || ((c) == '\t') || ((c) == '\n'));
}

static int
is_msg_name_char(char c) {
	return isalnum(c) || c == '_';
}

static int 
is_binary_msg_char(char c)
{
	switch(c) {
		case '-':
		case '+':
		case '/':
		case '\\':
		case '*':
		case ',':
		case '&':
		case '|':
		case '!':
		case '<':
		case '>':
		case '=':
		case '@':
		case '%':
			return 1;
		default:
			return 0;
	}
}



static KxParser * 
kxparser_new()
{
	KxParser *p = calloc(sizeof(KxParser), 1);
	return p;
}

void 
kxparser_free(KxParser *self)
{
	if (self->buffer)
		free(self->buffer);
	if (self->token_value_string)
		free(self->token_value_string);
	if (self->documentation_string)
		free(self->documentation_string);
	free(self);
}

static void 
kxparser_init(KxParser *self)
{
	self->pos = self->buffer;
	self->line_number = 1;
}

KxParser *
kxparser_parse_string(char *string, int parse_documentation) 
{
	KxParser *self = kxparser_new();
	self->buffer = strdup(string);
	self->parse_documentation = parse_documentation;
	kxparser_init(self);
	return self;
}

static
void
kxparser_set_error(KxParser *self, char *error_message)
{
	self->token_value_string = strdup(error_message);
}

static void
kxparser_skip_blank(KxParser *self)
{
	char chr = GET_CHAR;
	while (is_white_char(chr)) {
		if (chr == '\n') 
			self->line_number++;
		chr = NEXT_CHAR;
	} 
}

static int kxparser_is_alias(KxParser *self)
{
	char *chr = self->pos;

	while(is_white_char(*chr)) {
		chr++;
	}
	switch(*chr) {
		case '=': 
			chr++;
			if (!is_binary_msg_char(*chr)) {
				self->pos = chr;
				return TOKEN_SET_SLOT;
			}
			break;
		case '<':
			chr++;
			if (*chr == '<') {
				chr++;
				if (!is_binary_msg_char(*chr)) {
					self->pos = chr;
					return TOKEN_UPDATE_SLOT;
				}
			}
			if (*chr == '-') {
				chr++;
				if (!is_binary_msg_char(*chr)) {
					self->pos = chr;
					return TOKEN_UPDATE_PAIR;
				}
			}
	}

	return 0;
}

static int
kxparser_message_name(KxParser *self) 
{
	char tmp[IDENTIFIER_SIZE+2]; 	
	int pos = 1;
	tmp[0] = GET_CHAR;
	char chr = NEXT_CHAR;
	while(pos < IDENTIFIER_SIZE && is_msg_name_char(chr)) {
		tmp[pos++] = chr;
		chr = NEXT_CHAR;
	}
	
	int type;
	if (chr == ':') {
		tmp[pos++] = ':';
		chr = NEXT_CHAR;
		if (isalpha(chr)) {
			while((isalnum(chr) || chr == ':') && pos < IDENTIFIER_SIZE) {
				tmp[pos++] = chr;
				chr = NEXT_CHAR;
			}
			type = -1;
		} else  {
			type = TOKEN_KEYWORD_MSG_PART;
		}
	} else {
		type = TOKEN_UNARY_MSG;
	}
	tmp[pos] = 0;
	if (!strcmp(tmp, "resend")) {
		return TOKEN_RESEND;
	}

	int t = kxparser_is_alias(self);
	if (t) 
		type = t;

	if (type == -1) {
		kxparser_set_error(self, "Invalid message name");
		return TOKEN_ERROR;
	}
	self->token_value_string = strdup(tmp);
	return type;
}

static void
escape_sequence(KxParser *self, char c, char *out)
{
	switch(c) {
		case 'n': *out = '\n'; return;
		case 't': *out = '\t'; return;
		case 'r': *out = '\r'; return;
		case '0': *out = 0; return;

		case '\n': ++self->line_number;
		default: *out = c;
	}
}

static char
kxparser_look_ahead(KxParser *self, int k) 
{
	char *p = self->pos;
	while(k > 0) {
		if (*p == 0)
			return 0;
		k--;
		p++;
	}
	return *p;
}


static int 
kxparser_token_string(KxParser *self)
{
	char tmp[STRING_SIZE+1]; 	
	int pos = 0;
	char chr = NEXT_CHAR;
	char chr2 = kxparser_look_ahead(self, 1);

	if (chr == '"' && chr2 == '"') {
		// Long string
		pos = 0;
		SKIP_CHAR;
		SKIP_CHAR;
		for(;;) {
			while(GET_CHAR != '"' && pos < STRING_SIZE-2) {
				char chr = GET_CHAR;
				if (chr == '\n') 
					self->line_number++;
				if (chr == '\\') {
					char c = NEXT_CHAR;
					if (c == 0) {
						kxparser_set_error(self, "Invalid escape sequence");
						return TOKEN_ERROR;
					}
					escape_sequence(self,c, &chr);
				}

				tmp[pos++] = chr;
				SKIP_CHAR;
			}
			if (pos == STRING_SIZE-2) {
				// TODO: write error, string is too long
				break;
			}
			if (NEXT_CHAR != '"') {
				tmp[pos++] = '"';
				continue;
			}
			if (NEXT_CHAR != '"') {
				tmp[pos++] = '"';
				tmp[pos++] = '"';
				continue;
			}
			SKIP_CHAR;
			break;
		}
	} else {
	  // Normal string
	  while(chr != '"' && pos < STRING_SIZE) {
	  	if (chr == '\n') {
			self->next_token = TOKEN_STRING;
			kxparser_set_error(self, "Unterminated string");
			return TOKEN_ERROR;
		}

		if (chr == '\\') {
			char c = NEXT_CHAR;
			if (c == 0) {
				kxparser_set_error(self, "Invalid escape sequence");
				return TOKEN_ERROR;
			}
			escape_sequence(self, c, &chr);
		}
		tmp[pos++] = chr;
		chr = NEXT_CHAR;
      }
	  if (chr == '"')
	    SKIP_CHAR;
		//chr = NEXT_CHAR;
	}
	tmp[pos] = 0;
	
	self->token_value_string = strdup(tmp);

	return TOKEN_STRING;

}


static int
kxparser_token_number(KxParser *self, int negative)
{
	char tmp[IDENTIFIER_SIZE]; 	
	int pos = 1;
	tmp[0] = GET_CHAR;
	char chr = NEXT_CHAR;
	while(isdigit(chr) && pos < IDENTIFIER_SIZE) {
		tmp[pos++] = chr;
		chr = NEXT_CHAR;
	}

	if (chr == '.') {
		if (isdigit(kxparser_look_ahead(self, 1))) {
			tmp[pos++] = '.';
			chr = NEXT_CHAR;
			while((isdigit(chr) || chr=='-' || chr == 'e' || chr == 'E') && pos < IDENTIFIER_SIZE) {
				tmp[pos++] = chr;
				chr = NEXT_CHAR;
			}
			tmp[pos] = 0;

			//TODO: Check for errors
			self->token_value.float_val = atof(tmp);
			if (negative)
				self->token_value.float_val *= -1;

			return TOKEN_FLOAT;

		}
	}
	tmp[pos] = 0;
    self->token_value.int_val = atoi(tmp);
	if (negative)
		self->token_value.int_val *= -1;
	return TOKEN_INTEGER;
}

static int
kxparser_token_binary_message(KxParser *self)
{
	char tmp[IDENTIFIER_SIZE+1]; 	
	int pos = 1;
	tmp[0] = GET_CHAR;
	char chr = NEXT_CHAR;
	while(is_binary_msg_char(chr) && pos < IDENTIFIER_SIZE) {
		tmp[pos++] = chr;
		chr = NEXT_CHAR;
	}
	tmp[pos] = 0;
	self->token_value_string = strdup(tmp);
	
	int t = kxparser_is_alias(self);
	return t?t:TOKEN_BINARY_MSG;
}

static int kxparser_token_symbol_or_list(KxParser *self)
{
	char chr = NEXT_CHAR;
	if (chr == '(') {
		SKIP_CHAR; return TOKEN_LIST;
	}

	if (!isalnum(chr) && chr != '_' && !is_binary_msg_char(chr)) {
			kxparser_set_error(self, "Invalid symbol name");
			return TOKEN_ERROR;
	}

	char tmp[IDENTIFIER_SIZE+1]; 	
	int pos = 1;
	tmp[0] = chr;
	chr = NEXT_CHAR;
	
	if (is_binary_msg_char(tmp[0])) {
		while(is_binary_msg_char(chr) && pos < IDENTIFIER_SIZE) {
			tmp[pos++] = chr;
			chr = NEXT_CHAR;
		}
	} else {
		while((is_msg_name_char(chr) || chr == ':') && pos < IDENTIFIER_SIZE) {
			tmp[pos++] = chr;
			chr = NEXT_CHAR;
		}
	}

	tmp[pos] = 0;

	self->token_value_string = strdup(tmp);
	return TOKEN_SYMBOL;
}


static int
kxparser_token_character(KxParser *self)
{
	self->token_value.char_val = NEXT_CHAR;

	switch(self->token_value.char_val) {
		 case '\n': ++self->line_number;
		 case '\t':
		 case '\'': SKIP_CHAR; SAFE_SKIP_CHAR;
		 case 0   :
		 			kxparser_set_error(self, "Invalid character");
					return TOKEN_ERROR;
		 	
		 case '\\': {
			char c = NEXT_CHAR;
			if (c == 0) {
				kxparser_set_error(self, "Invalid escape sequence");
				return TOKEN_ERROR;
			}
			escape_sequence(self, c, &self->token_value.char_val);

		}
	}
	if (NEXT_CHAR != '\'') {
		kxparser_set_error(self,"Character literal isn't closed");
		return TOKEN_ERROR;
	}
	SKIP_CHAR;
	return TOKEN_CHARACTER;
}

static void 
kxparser_one_line_comment(KxParser *self)
{
	SKIP_CHAR;
	char chr = NEXT_CHAR;
	while(chr != '\n' && chr != 0) {
		chr = NEXT_CHAR;
	}
	if (chr == '\n') {
		self->line_number++;
		SKIP_CHAR;
	}
}

static int
kxparser_multi_line_comment(KxParser *self)
{
	char chr=GET_CHAR;
	do {
		while(chr!='*') {
			switch (chr) {
				case 0: kxparser_set_error(self,"Unterminated comment"); return 0;
				case '\n': self->line_number++;
			}
			chr = NEXT_CHAR;
		};
		chr = NEXT_CHAR;
	} while(chr != '/');
	SKIP_CHAR;
	return 1;
}

static int
kxparser_documentation(KxParser *self)
{
	self->documentation_type = (GET_CHAR == '*')?KXPARSER_DOC_OBJ:KXPARSER_DOC_SLOT;
	char tmp[STRING_SIZE+1];
	int pos = 0;
	char chr=NEXT_CHAR;
	for(;;) {
		while(chr!='*') {
			switch (chr) {
				case 0: kxparser_set_error(self,"Unterminated comment"); return 0;
				case '\n': self->line_number++;
			}
			if (pos < STRING_SIZE) {
				tmp[pos++] = chr;
			}
			chr = NEXT_CHAR;
		};
		chr = NEXT_CHAR;
		if (chr == '/')
			break;

		if (pos < STRING_SIZE) {
			tmp[pos++] = '*';
		}
	}	
	tmp[pos] = 0;
	if (self->documentation_string)
		free(self->documentation_string);
	self->documentation_string = strdup(tmp);
	SKIP_CHAR;
	return 1;
}

char *
kxparser_get_documentation_string(KxParser *self, int *type)
{
	char *string = self->documentation_string;
	self->documentation_string = NULL;
	*type = self->documentation_type;
	return string;
}

void
kxparser_reset_documentation_string(KxParser *self)
{
	if (self->documentation_string) {
		free(self->documentation_string);
		self->documentation_string = NULL;
	}
}


static int
kxparser_token_param(KxParser *self)
{
	char tmp[IDENTIFIER_SIZE+1]; 	
	int pos = 1;
	tmp[0] = NEXT_CHAR;
	if (!tmp[0] == '_' && !isalpha(tmp[0])) {
		kxparser_set_error(self, "Invalid name of parameter\n");
		return TOKEN_ERROR;
	}
	char chr = NEXT_CHAR;
	while(is_msg_name_char(chr) && pos < IDENTIFIER_SIZE) {
		tmp[pos++] = chr;
		chr = NEXT_CHAR;
	}
	tmp[pos] = 0;
	self->token_value_string = strdup(tmp);
	
	return TOKEN_PARAM;
}


static int
kxparser_read_token(KxParser *self)
{
	for(;;) {
		kxparser_skip_blank(self);
		
		char chr = GET_CHAR;

		switch (chr) {
			case '=': 
				// Must be before binary message test
				if (!is_binary_msg_char(kxparser_look_ahead(self,1))) {
					SKIP_CHAR;
					kxparser_set_error(self, "Invalid usage of alias '='");
					return TOKEN_ERROR;
				}
				break;
			case '<': {
				// Must be before binary message test
					char chr1 = kxparser_look_ahead(self, 1);
					char chr2 = kxparser_look_ahead(self, 2);

					if ((chr1 == '<') && !is_binary_msg_char(chr2)) {
						SKIP_CHAR;
						kxparser_set_error(self, "Invalid usage of alias '<<'");
						return TOKEN_ERROR;
					}
					if ((chr1 == '-') && !is_binary_msg_char(chr2)) {
						SKIP_CHAR;
						kxparser_set_error(self, "Invalid usage of alias '<-'");
						return TOKEN_ERROR;
					}
					break;
				}
			
			case '-': {
				// Unary minus
				// Must be before binary message test
				if (isdigit(kxparser_look_ahead(self, 1))) {
					SKIP_CHAR;
					return kxparser_token_number(self, 1);
				}
				break;
			}

			case '"': return kxparser_token_string(self);
			case '#': return kxparser_token_symbol_or_list(self);
			case '\'': return kxparser_token_character(self);

			case '/': {
				char chr = kxparser_look_ahead(self, 1);
				if (chr == '/') {
					kxparser_one_line_comment(self);
					continue;
				}

				if (chr == '*') {
					SKIP_CHAR;
					chr = NEXT_CHAR;
					int r;
					if (self->parse_documentation && (chr == '*' || chr == '=')) {
						r = kxparser_documentation(self);
					} else {
						r = kxparser_multi_line_comment(self);
					}
					if (r) 
						continue;
					else 
						return TOKEN_ERROR;
				}
				break;
			}

			case ':': {
				return kxparser_token_param(self);
			}

			case '|':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case '.':
			case '^':
				SKIP_CHAR;
				return chr;
			case 0:
				return TOKEN_END;


		}



		if (isalpha(chr) || chr == '_') {
			return kxparser_message_name(self);
		}
		if (isdigit(chr)) {
			return kxparser_token_number(self,0);
		}

		if (is_binary_msg_char(chr)) {
			return kxparser_token_binary_message(self);
		}

		kxparser_set_error(self,"Unknown character");
		SKIP_CHAR;
		return TOKEN_ERROR;
	}
}

int 
kxparser_get_token(KxParser *self)
{
	if (self->token_value_string) {
		free(self->token_value_string);
		self->token_value_string = NULL;
	}
	if (self->next_token) {
		int t = self->next_token;
		self->next_token = 0;
		self->token_value_string = strdup("");
		return t;
	}
	return kxparser_read_token(self);
}

char *
kxparser_token_string_value(KxParser *self)
{
	char *str = self->token_value_string;
	self->token_value_string = NULL;
	return str;
}

int
kxparser_token_integer_value(KxParser *self)
{
	return self->token_value.int_val;
}

double
kxparser_token_float_value(KxParser *self)
{
	return self->token_value.float_val;
}

char
kxparser_token_char_value(KxParser *self)
{
	return self->token_value.char_val;
}
