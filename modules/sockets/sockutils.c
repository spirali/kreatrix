/*
   sockutils.c
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
	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/poll.h>

#include "sockutils.h"
#include "kxcore.h"

// read, write = 1-include to poll, 0-not include to poll
// return 0 - err, 1 - ok
int 
sock_poll_single(int sock,int read, int write, int timeout, int *revents)
{
	struct pollfd pfd;
	pfd.fd = sock;
	pfd.events = 0;
	pfd.revents = 0;
	if (read)
		pfd.events |= POLLIN;
	if (write)
		pfd.events |= POLLOUT;
	if (poll(&pfd,1,timeout) < 0)
		return 0;
	*revents = pfd.revents;
	return 1;
}
