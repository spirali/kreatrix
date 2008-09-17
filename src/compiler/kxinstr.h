/*
   kxinstr.h
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
	

#ifndef __KXINSTR_H
#define __KXINSTR_H

#define KXC_INSTRUCTION_MAX_PARAMS 3

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
	KXCI_PUSH_LOCAL_CONTEXT, // 35

	KXCI_UPDATE_LOCAL, // 36
	KXCI_UPDATE_OUTER_LOCAL, // 37

	KXCI_HOTSPOT_PROBE = 40,

	KXCI_IFTRUE = 50,
	KXCI_IFFALSE,
	KXCI_IFTRUE_IFFALSE,
	KXCI_IFFALSE_IFTRUE,
	KXCI_JUMP,
	KXCI_FOREACH,
	KXCI_NEXTITER,
	KXCI_TODO,
	KXCI_TODO_END,
	KXCI_TOBYDO,
	KXCI_TOBYDO_END,
	KXCI_REPEAT,
	KXCI_REPEAT_END,
	KXCI_JUMP_IFTRUE,
	KXCI_JUMP_IFFALSE,
	KXCI_JUMP_IFNOTTRUE,
	KXCI_JUMP_IFNOTFALSE,

	KXCI_UNARY_MSG_C = 100, 
	KXCI_BINARY_MSG_C,
	KXCI_KEYWORD_MSG_C,
	KXCI_LOCAL_UNARY_MSG_C, 
	KXCI_LOCAL_KEYWORD_MSG_C,

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
