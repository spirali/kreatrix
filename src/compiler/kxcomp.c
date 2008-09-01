/*****************************************
 *
 *  TODO: write error when string or message name is too long 
 *        
 *  Grammar:
 *  
 *  start: commands
 *  
 *  commands: e | expression | expression '.' commands
 *
 *  expression: keyword_msg
 *            | '^' keyword_msg
 *            | keyword_msg? TOKEN_SET_SLOT keyword_msg
 *            | keyword_msg? TOKEN_UPDATE_PAIR keyword_msg
 *            | keyword_msg? TOKEN_UPDATE_SLOT keyword_msg
 *
 *  keyword_msg: binary_msg 
 *             | 'TOKEN_KEYWORD_MSG_PART' keyword_msg_next 
 *             | binary_msg 'TOKEN_KEYWORD_MSG_PART' keyword_msg_next
 *
 *  keyword_msg_next: binary_msg (e | 'TOKEN_KEYWORD_MSG_PART' keyword_msg_next)
 *
 *  binary_msg: unary_msg | unary_msg TOKEN_BINARY_MSG binary_msg
 *
 *  unary_msg: target (TOKEN_UNARY_MSG)*
 *
 *  target: TOKEN_STRING
 *  	  | TOKEN_SYMBOL
 *        | TOKEN_INTEGER
 *        | TOKEN_FLOAT
 *        | TOKEN_UNARY_MSG
 *        | TOKEN_RESEND
 *        | '(' expression ')'
 *        | '{' commands '}'
 *        | '[' commands ']'
 *        | TOKEN_LIST (target)* ')'
 *
 ****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <errno.h>

#include "kxcomp.h"
#include "../utils/list.h"

#define CHECK(x) if (!(x)) return 0;

static int grammar_expression(KxCompiler *self);
static int grammar_commands(KxCompiler *self);
static int grammar_target(KxCompiler *self);


static KxCompiler *
kxcomp_new()
{
	KxCompiler *self = calloc(sizeof(KxCompiler), 1);
	self->errors = list_new();

	// Create root block
	self->block = kxcblock_new(KXCBLOCK_TYPE_METHOD, NULL, NULL, NULL, self->errors);
	return self;
}

static void
kxcomp_free(KxCompiler *self)
{
	if (self->parser)
		kxparser_free(self->parser);
	if (self->errors)
		list_free(self->errors);
	free(self);
}

static void 
kxcomp_add_error(KxCompiler *self, char *form, ...)
{
	char buf[1024];

	va_list arg;
	va_start(arg,form);
	vsnprintf(buf,1024, form, arg);
	va_end(arg);

	char buf2[1024];
	snprintf(buf2,1024,"%s:%i: %s\n", self->filename, self->parser->line_number, buf);
	
	list_append(self->errors, strdup(buf2));
}

static void
kxcomp_check_resend(KxCompiler *self)
{
	if (self->resend) {
		kxcomp_add_error(self,"Invalid use of resend\n");
		self->resend = 0;
	}
}

static int 
kxcomp_next_token(KxCompiler *self)
{
	int token = kxparser_get_token(self->parser);
	while(token == TOKEN_ERROR || token == TOKEN_DOCUMENTATION) {
		if (token == TOKEN_DOCUMENTATION) {
			token = kxparser_get_token(self->parser);
			continue;
		}
		char *str = kxparser_token_string_value(self->parser);
		kxcomp_add_error(self, str);
		free(str);
		token = kxparser_get_token(self->parser);
	}
	self->token = token;
//	printf("token %i\n", token);
	return token;
}


static int
kxcomp_test_token(KxCompiler *self, int token)
{
	if (self->token == token) {
		kxcomp_next_token(self);
		return 1;
	} 
	kxcomp_add_error(self,"Token '%c' exptected\n", token);
	return 0;
}

static int 
grammar_list(KxCompiler *self)
{
	kxcomp_next_token(self);
	int count = 0;
	while(self->token != ')') {
		count ++;
		CHECK(grammar_target(self));
	}
	kxcomp_next_token(self);
	kxcblock_put_list(self->block, count);

	return 1;
}

static int
grammar_params(KxCompiler *self, List **params)
{
	if (self->token != TOKEN_PARAM) {
		*params = NULL;
		return 1;
	}
	*params = list_new();
	do {
		list_append(*params, kxparser_token_string_value(self->parser));
		kxcomp_next_token(self);
	} while(self->token == TOKEN_PARAM);
	if (self->token == '|') {
		kxcomp_next_token(self);
		return 1;
	}
	list_free(*params);
	*params = NULL;
	kxcomp_add_error(self, "Invalid definition of parameter\n");

	return 0;
}

static int
grammar_locals(KxCompiler *self, List **locals)
{
	if (self->token != '|') {
		*locals = NULL;
		return 1;
	}
	kxcomp_next_token(self);

	*locals = list_new();

	while(self->token == TOKEN_UNARY_MSG) {
		list_append(*locals, kxparser_token_string_value(self->parser));
		kxcomp_next_token(self);
	}

	if (!kxcomp_test_token(self, '|')) {
		list_free(*locals);
		*locals = NULL;
	}
	return 1;
}

static int
kxcomp_put_codeblock(KxCompiler *self, int type, char close_char)
{
	kxcomp_next_token(self);

	List *params, *locals;
	CHECK(grammar_params(self, &params));
	CHECK(grammar_locals(self, &locals));

	KxcBlock *old = self->block;
	self->block = kxcblock_put_new_block(self->block,type, params,locals, self->errors); 

	if (!grammar_commands(self)) {
		self->block = old;
		return 0;
	}
	
	kxcblock_end_of_code(self->block);
	self->block = old;

	CHECK(kxcomp_test_token(self, close_char));
	return 1;
}

static int
grammar_target(KxCompiler *self)
{
	switch(self->token) {
		case TOKEN_INTEGER:
			kxcblock_put_integer(self->block, kxparser_token_integer_value(self->parser));
			break;
		case TOKEN_FLOAT:
			kxcblock_put_float(self->block, kxparser_token_float_value(self->parser));
			break;
		case TOKEN_STRING: {
			kxcblock_put_string(self->block, kxparser_token_string_value(self->parser));
			break;
		}
		case TOKEN_SYMBOL:
			kxcblock_put_symbol(self->block, kxparser_token_string_value(self->parser));
			break;

		case TOKEN_CHARACTER:
			kxcblock_put_character(self->block, kxparser_token_char_value(self->parser));
			break;

		case TOKEN_UNARY_MSG: {
			char *str = kxparser_token_string_value(self->parser);
			kxcblock_put_message(self->block,KXCI_LOCAL_UNARY_MSG,str, self->parser->line_number);
			break;
			}

		case '(': // PARENTHESIS 
			{
				kxcomp_next_token(self);
				CHECK(grammar_expression(self));
				CHECK(kxcomp_test_token(self, ')'));
				return 1;
			}
		case '{': // METHOD
			return kxcomp_put_codeblock(self, KXCBLOCK_TYPE_METHOD, '}');
		case '[': // BLOCK
			return kxcomp_put_codeblock(self, KXCBLOCK_TYPE_BLOCK, ']');
		case TOKEN_LIST:
			CHECK(grammar_list(self));
			return 1;
		
		case TOKEN_RESEND: 
			self->resend = 1;
			break;
	
		default:
			kxcomp_add_error(self, "Syntax error");
			return 0;
	}
	kxcomp_next_token(self);
	return 1;
}

static int 
grammar_unary_message(KxCompiler *self)
{
	CHECK(grammar_target(self));
	while (self->token == TOKEN_UNARY_MSG) {
		char *str = kxparser_token_string_value(self->parser);
		if (self->resend) {
			kxcblock_put_message(self->block,KXCI_RESEND_UNARY_MSG,str, self->parser->line_number); 		
			self->resend = 0;
		} else {
			kxcblock_put_message(self->block,KXCI_UNARY_MSG,str, self->parser->line_number); 		
		}

		kxcomp_next_token(self);
	}
	return 1;
}

static int 
grammar_binary_message(KxCompiler *self)
{
	CHECK(grammar_unary_message(self));
	if (self->token == TOKEN_BINARY_MSG) {
		kxcomp_check_resend(self);
		char *binary_message = kxparser_token_string_value(self->parser);
		kxcomp_next_token(self);

		CHECK(grammar_unary_message(self));
		kxcblock_put_message(self->block, KXCI_BINARY_MSG,strdup(binary_message), self->parser->line_number); 

		while(self->token == TOKEN_BINARY_MSG) {
			char *msg = kxparser_token_string_value(self->parser);
			if (strcmp(binary_message,msg)) {
				free(msg);	free(binary_message);
				kxcomp_add_error(self,"Precedence of binary messages is ambiguous\n");
				return 0;
			}
			kxcomp_next_token(self);
			if (grammar_unary_message(self) == 0) {
				free(binary_message);
				return 0;
			}
			kxcblock_put_message(self->block, KXCI_BINARY_MSG,msg, self->parser->line_number); 
		}
		kxcomp_check_resend(self);
		free(binary_message);
	}
	return 1;
}

static int 
grammar_keyword_message(KxCompiler *self)
{
	int local_send;
	if (self->token != TOKEN_KEYWORD_MSG_PART) {
		CHECK(grammar_binary_message(self));
		if (self->token != TOKEN_KEYWORD_MSG_PART) {
			kxcomp_check_resend(self);
			return 1;
		}
		local_send = 0;
	} else {
		local_send = 1;
	}
	
	char *msg_name = NULL;

	int resend = self->resend;
	self->resend = 0;

	while (self->token == TOKEN_KEYWORD_MSG_PART) {
		char *part = kxparser_token_string_value(self->parser);
		if (msg_name == NULL) {
			msg_name = part;
		} else {
			int msg_len = strlen(msg_name);
			msg_name = realloc(msg_name, msg_len+strlen(part)+1);
			ALLOCTEST(msg_name);
			strcpy(msg_name + msg_len, part);
			free(part);
		}
		kxcomp_next_token(self);
		CHECK(grammar_binary_message(self));
	}
	if (resend) {
		self->resend = 0;
		kxcblock_put_message(self->block, KXCI_RESEND_KEYWORD_MSG,msg_name, self->parser->line_number); 
	} else {
		if (local_send) {
			kxcblock_put_message(self->block, KXCI_LOCAL_KEYWORD_MSG,msg_name, self->parser->line_number); 
		} else {
			kxcblock_put_message(self->block, KXCI_KEYWORD_MSG,msg_name, self->parser->line_number); 
		}
	}
	return 1;
}

static int
kxcomp_put_alias(KxCompiler *self, int message_type, char *message_name, int substitution_check)
{
	char *symbol = kxparser_token_string_value(self->parser);

	int is_local;
	KxcLocal local;

	if (substitution_check) {
		is_local = kxcblock_find_local(self->block, symbol, &local);
	} else {
		is_local = 0;
	}

	int doc_type;
	char *doc_string = NULL;
	
	if (!is_local) {
		doc_string = kxparser_get_documentation_string(self->parser, &doc_type);
		if (doc_string) {
			kxcblock_put_symbol(self->block, strdup(symbol));
		} else {
			kxcblock_put_symbol(self->block, symbol);
		}
	}

	kxcomp_next_token(self);
	if(grammar_keyword_message(self) == 0) {
		if (doc_string) {
			free(doc_string);
			free(symbol);
		}
		return 0;
	}

	if (!is_local) {
		kxcblock_put_message(self->block,message_type,strdup(message_name), self->parser->line_number); 
	} else {
		kxcblock_put_local_update(self->block, &local);
		kxcblock_push_local(self->block, &local);
		free(symbol);
	}

	if (doc_string) {
		kxcblock_put_symbol(self->block,symbol);
		kxcblock_put_string(self->block,doc_string);

		char *msg;
		if (doc_type == KXPARSER_DOC_OBJ) {
			msg = "__slot:mainDoc:";
		} else {
			msg = "__slot:doc:";
			}
		kxcblock_put_message(self->block,KXCI_KEYWORD_MSG,strdup(msg), self->parser->line_number); 
	}
	return 1;
}

static char *
get_updater_name(char *slot_name) 
{
	int len = strlen(slot_name);
	char *new = malloc(len+2);
	ALLOCTEST(new);
	strcpy(new, slot_name);
	new[len] = ':';
	new[len+1] = 0;
	return new;
}

static kxcomp_put_update_pair(KxCompiler *self, int message_type) 
{
	char *symbol = kxparser_token_string_value(self->parser);
	char *symbol2 = get_updater_name(symbol);

	kxcblock_put_symbol(self->block,symbol);
	kxcblock_put_symbol(self->block,symbol2);
 
	kxcomp_next_token(self);
	CHECK(grammar_keyword_message(self));
	kxcblock_put_message(self->block,message_type,strdup("slot:updater:set:"), self->parser->line_number); 
	return 1;
}

static int 
grammar_expression2(KxCompiler *self)
{
	switch(self->token) {
		case TOKEN_SET_SLOT:
			return kxcomp_put_alias(self, KXCI_LOCAL_KEYWORD_MSG, "slot:set:",0);
		case TOKEN_UPDATE_SLOT:
			return kxcomp_put_alias(self, KXCI_LOCAL_KEYWORD_MSG, "slot:update:",1);
		case TOKEN_UPDATE_PAIR:
			return kxcomp_put_update_pair(self, KXCI_LOCAL_KEYWORD_MSG);
	}

	CHECK(grammar_keyword_message(self));

	switch(self->token) {
		case TOKEN_SET_SLOT:
			return kxcomp_put_alias(self, KXCI_KEYWORD_MSG, "slot:set:",0);
		case TOKEN_UPDATE_SLOT:
			return kxcomp_put_alias(self, KXCI_KEYWORD_MSG, "slot:update:",0);
		case TOKEN_UPDATE_PAIR:
			return kxcomp_put_update_pair(self, KXCI_KEYWORD_MSG);
		default:
			kxparser_reset_documentation_string(self->parser);
	}
	return 1;
}

static int
grammar_expression(KxCompiler *self)
{
	int method_return = 0;

	if (self->token == '^') {
		kxcomp_next_token(self);
		method_return = 1;
	}

	CHECK(grammar_expression2(self));

	if (method_return) {
		kxcblock_put_return(self->block);
	}
	return 1;
}

static int
grammar_commands(KxCompiler *self)
{
	// Pri zotaveni vyresetovat resend
	for(;;) {
		switch(self->token) {
			case TOKEN_END:
			case '}':
			case ']':
				return 1;
		}

		if (!grammar_expression(self)) {
			while(self->token != '.' && self->token != '}' && self->token != ']' && self->token != 0) {
				kxcomp_next_token(self);
			}
			if (self->token == 0) {
				return 0;
			}
		}
		kxcblock_put_pop(self->block);
		switch(self->token) {
			case TOKEN_END:
			case '}':
			case ']':
				return 1;
			case '.': 
				kxcomp_next_token(self);
				break;
			default:
				kxcomp_add_error(self,"Syntax error, '.' is expected\n");
		}
	}
}



static int
kxcomp_start(KxCompiler *self)
{
	kxcomp_next_token(self);
	CHECK(grammar_commands(self));

	kxcblock_end_of_main_block(self->block);

	kxcblock_optimise_root_block(self->block, self->level_of_optimisation);
	return 1;
}

List *
kxcomp_one_error_list(char *form, ...) 
{
	char buf[1024];

	va_list arg;
	va_start(arg,form);
	vsnprintf(buf,1024, form, arg);
	va_end(arg);

	List *list = list_new();
	list_append(list, strdup(buf));
	return list;
}

List *
kxcomp_compile_string(char *string, char *filename, 
	int compile_docs, int level_of_optimisation, KxcBlock **block)
{
	*block = NULL;
	KxCompiler *self = kxcomp_new();
	self->filename = filename;
	self->compile_docs = compile_docs;
	self->level_of_optimisation = level_of_optimisation;
	self->parser = kxparser_parse_string(string, compile_docs);

	int r = kxcomp_start(self);
	
	List *errors = self->errors;
	self->errors = NULL;

	if (errors->size == 0) {
		list_free(errors);
		errors = NULL;
	}

	*block = self->block;

	kxcomp_free(self);

	if (!errors && r == 0) {
		return kxcomp_one_error_list("Internal error, compilation failed but there is no error message\n");
	}

	return errors;
}

List * 
kxcomp_compile_file(char *filename, int compile_docs, 
	int level_of_optimisation, KxcBlock **block) 
{
	*block = NULL;
	FILE *file = fopen(filename,"r");
	if (file == NULL) {
		return kxcomp_one_error_list("Can't open file '%s': %s", filename,strerror(errno));
	}
	unsigned long size;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *content = malloc(size+1);
	ALLOCTEST(content);

	if (size > 0) {
		if (fread(content, size, 1, file) != 1) {
			fclose(file);
			return kxcomp_one_error_list("Reading from file '%s' failed: %s", filename,strerror(errno));
		}
	}

	fclose(file);
	content[size] = 0;

	char *start = content;
	if (*start == '#') {
	    /* Skip first line, if file beginns with # */
		while(*start != '\n' && *start != 0) {
			start++;
		}
	}
	
	List *errors = kxcomp_compile_string(start, filename, compile_docs, 
		level_of_optimisation, block);

	free(content);
	return errors;
}
