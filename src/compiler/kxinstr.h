
#ifndef __KXINSTR_H
#define __KXINSTR_H

typedef enum { 	
	
	KXCI_UNARY_MSG,  // 0
	KXCI_BINARY_MSG, 
	KXCI_KEYWORD_MSG,
	KXCI_LOCAL_UNARY_MSG,
	KXCI_LOCAL_KEYWORD_MSG, // 4
	KXCI_RESEND_UNARY_MSG,
	KXCI_RESEND_KEYWORD_MSG,


	KXCI_POP = 15, // 15

	KXCI_PUSH_LITERAL, // 16
	KXCI_PUSH_METHOD,
	KXCI_PUSH_BLOCK,  
	KXCI_LIST, 
	KXCI_RETURN_STACK_TOP,
	KXCI_RETURN_SELF,
	KXCI_LONGRETURN, // 22

	KXCI_PUSH_LOCAL = 32, // 32
	KXCI_PUSH_OUTER_LOCAL, // 33

	KXCI_PUSH_SELF,   // 34
	KXCI_PUSH_ACTIVATION, // 35

	KXCI_UPDATE_LOCAL, // 36
	KXCI_UPDATE_OUTER_LOCAL, // 37

	KXCI_IFTRUE = 50,
	KXCI_IFFALSE,

	KXCI_INSTRUCTIONS_COUNT
} KxInstructionType;

typedef enum {
	KXC_LITERAL_SYMBOL,
	KXC_LITERAL_INTEGER,
	KXC_LITERAL_FLOAT,
	KXC_LITERAL_STRING,
	KXC_LITERAL_CHAR
} KxcLiteralType;

typedef struct KxInstructionInfo KxInstructionInfo;

struct KxInstructionInfo {
	char *name;
	int params_count;
};

extern KxInstructionInfo kxinstructions_info[KXCI_INSTRUCTIONS_COUNT];

int kxcinstruction_has_linenumber(KxInstructionType instruction);

#endif
