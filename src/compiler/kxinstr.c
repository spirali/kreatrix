#include "kxinstr.h"

KxInstructionInfo kxinstructions_info[KXCI_INSTRUCTIONS_COUNT] = {
	{"send_unary", 1},          // 00 KXCI_UNARY_MSG 
	{"send_binary", 1},         // 01 KXCI_BINARY_MSG 
	{"send_keyword", 2},        // 02 KXCI_BINARY_MSG 
	{"send_local_unary", 1},    // 03 KXCI_LOCAL_UNARY_MSG
	{"send_local_keyword", 2},  // 04 KXCI_LOCAL_KEYWORD_MSG
	{"send_resend_unary", 1},   // 05 KXCI_RESEND_UNARY_MSG
	{"send_resend_keyword", 2}, // 06 KXCI_RESEND_KEYWORD_MSG

	{"reserved", 0},            // 07
	{"reserved", 0},            // 08
	{"reserved", 0},            // 09
	{"reserved", 0},            // 10
	{"reserved", 0},            // 11
	{"reserved", 0},            // 12
	{"reserved", 0},            // 13
	{"reserved", 0},            // 14
	
	{"pop", 0},                 // 15 KXCI_POP
	{"push_literal", 1},        // 16 KXCI_PUSH_LITERAL
	{"push_method", 1},         // 17 KXCI_PUSH_METHOD
	{"push_block", 1},          // 18 KXCI_PUSH_BLOCK
	{"push_list", 1},           // 19 KXCI_PUSH_LIST
	{"return_stack_top", 0},    // 20 KXCI_RETURN_STACK_TOP
	{"return_self", 0},         // 21 KXCI_RETURN_SELF
	{"long_return", 0},         // 22 KXCI_LONGRETURN

	{"reserved", 0},            // 23
	{"reserved", 0},            // 24
	{"reserved", 0},            // 25
	{"reserved", 0},            // 26
	{"reserved", 0},            // 27
	{"reserved", 0},            // 28
	{"reserved", 0},            // 29
	{"reserved", 0},            // 30
	{"reserved", 0},            // 31

	{"push_local", 1},          // 32 KXCI_PUSH_LOCAL
	{"push_outer_local", 2},    // 33 KXCI_PUSH_OUTER_LOCAL
	{"push_self", 0},           // 34 KXCI_PUSH_SELF
	{"push_activation", 0},     // 35 KXCI_PUSH_ACTIVATION
	{"update_local", 1},        // 36 KXCI_UPDATE_LOCAL
	{"update_outer_local", 2}   // 37 KXCI_UPDATE_OUTER_LOCAL
};

