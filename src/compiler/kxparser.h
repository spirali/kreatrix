/*
   kxparser.h
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
	
#ifndef __KREATRIX_PARSER_H
#define __KREATRIX_PARSER_H

typedef struct KxParser KxParser;

#define TOKEN_ERROR -1
#define TOKEN_END 0

#define TOKEN_STRING 200
#define TOKEN_INTEGER 201
#define TOKEN_FLOAT 202
#define TOKEN_SYMBOL 203
#define TOKEN_LIST 204
#define TOKEN_CHARACTER 205

#define TOKEN_UNARY_MSG 220
#define TOKEN_BINARY_MSG 221
#define TOKEN_KEYWORD_MSG_PART 222

#define TOKEN_PARAM 240

#define TOKEN_SET_SLOT 300
#define TOKEN_UPDATE_SLOT 301
#define TOKEN_UPDATE_PAIR 302

#define TOKEN_RESEND 400

#define TOKEN_DOCUMENTATION 500

#define KXPARSER_DOC_OBJ  0
#define KXPARSER_DOC_SLOT 1

struct KxParser {
	char *buffer;
	char *pos;

	int line_number;
	char *token_value_string;
	union { int int_val; double float_val; char char_val;} token_value;

	int next_token;

	int parse_documentation;
	char *documentation_string;
	int documentation_type;
};

KxParser *kxparser_parse_string(char *string, int parser_documentation);

int kxparser_get_token(KxParser *self);

char *kxparser_token_string_value(KxParser *self);
int kxparser_token_integer_value(KxParser *self);
double kxparser_token_float_value(KxParser *self);
char kxparser_token_char_value(KxParser *self);



char *kxparser_get_documentation_string(KxParser *self, int *type);
void kxparser_reset_documentation_string(KxParser *self);




void kxparser_free(KxParser *self);



#endif
