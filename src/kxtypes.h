/**********************************************************************
 *
 *	$Id$
 *
 *  Basic global types
 *
 **********************************************************************/

#ifndef __KXTYPES_H
#define __KXTYPES_H

typedef enum { RET_OK, 
			   RET_METHODRETURN, 
			   RET_THROW, 
			   RET_BLOCKRETURN, 
			   RET_LONGRETURN, // method return from block
			   RET_EXIT // VM is exiting.
			 } KxReturn;

#endif