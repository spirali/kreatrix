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
