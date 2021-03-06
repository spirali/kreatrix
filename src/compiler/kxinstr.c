/*
   kxinstr.c
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
	{"push_local_context", 0},  // 35 KXCI_PUSH_LOCAL_CONTEXT
	{"update_local", 1},        // 36 KXCI_UPDATE_LOCAL
	{"update_outer_local", 2},  // 37 KXCI_UPDATE_OUTER_LOCAL

	{"reserved", 0},            // 38
	{"reserved", 0},            // 39
	{"hotspot_probe", 0},       // 40 KXCI_HOTSPOT_PROBE
	{"reserved", 0},            // 41
	{"reserved", 0},            // 42
	{"reserved", 0},            // 43
	{"reserved", 0},            // 44
	{"reserved", 0},            // 45
	{"reserved", 0},            // 46
	{"reserved", 0},            // 47
	{"reserved", 0},            // 48
	{"reserved", 0},            // 49
	{"iftrue", 2},              // 50 KXCI_IFTRUE
	{"iffalse", 2},             // 51 KXCI_IFFALSE
	{"iftrue_iffalse", 3},      // 52 KXCI_IFTRUE_IFFALSE
	{"iffalse_iftrue", 3},      // 53 KXCI_IFFALSE_IFTRUE
	{"jump", 1},                // 54 KXCI_JUMP
	{"foreach", 3},             // 55 KXCI_FOREACH
	{"nextiter", 2},            // 56 KXCI_NEXTITER
	{"todo", 3},                // 57 KXCI_TODO
	{"todo_end", 2},            // 58 KXCI_TODO_END
	{"tobydo", 3},              // 59 KXCI_TOBYDO
	{"tobydo_end", 2},          // 60 KXCI_TOBYDO_END
	{"repeat", 2},              // 61 KXCI_REPEAT
	{"repeat_end", 1},          // 62 KXCI_REPEAT_END
	{"jump_iftrue", 1},         // 63 KXCI_JUMP_IFTRUE
	{"jump_iffalse", 1},        // 64 KXCI_JUMP_IFFALSE
	{"jump_ifnottrue", 1},      // 65 KXCI_JUMP_IFNOTTRUE
	{"jump_ifnotfalse", 1},     // 66 KXCI_JUMP_IFFALSE

	// 67 - 69
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},

	// 70 - 79
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},

	// 80 - 89
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},

	// 90 - 99
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},
	{"reserved", 0},

	{"send_unary_c", 1},
	{"send_binary_c", 1},
	{"send_keyword_c", 2},
	{"send_local_unary_c", 1},
	{"send_local_keyword_c", 2},
};


int kxcinstruction_has_linenumber(KxInstructionType instruction)
{
	switch(instruction) {
		case KXCI_UNARY_MSG:
		case KXCI_BINARY_MSG:
		case KXCI_KEYWORD_MSG:
		case KXCI_LOCAL_UNARY_MSG:
		case KXCI_LOCAL_KEYWORD_MSG:
		case KXCI_RESEND_UNARY_MSG:
		case KXCI_RESEND_KEYWORD_MSG:
		case KXCI_IFTRUE:
		case KXCI_IFFALSE:
		case KXCI_IFTRUE_IFFALSE:
		case KXCI_IFFALSE_IFTRUE:
		case KXCI_FOREACH:
		case KXCI_TODO:
		case KXCI_TOBYDO:
		case KXCI_REPEAT:
		case KXCI_UNARY_MSG_C:
		case KXCI_BINARY_MSG_C:
		case KXCI_KEYWORD_MSG_C:
		case KXCI_LOCAL_UNARY_MSG_C:
		case KXCI_LOCAL_KEYWORD_MSG_C:
			return 1;
		default:
			return 0;
	}
}

