/*
   kxglobals.c
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
	
#include "../kxconfig.h"
#include <stdio.h>
#include <stdlib.h>

#include "kxglobals.h"


int kx_verbose = 0;
int kx_doc_flag = 0;
int kx_level_of_optimisation = 2;

#ifdef KX_THREADS_SUPPORT
	int kx_threads_support = 1;
#else
	int kx_threads_support = 0;
#endif //KX_THREADS_SUPPORT
