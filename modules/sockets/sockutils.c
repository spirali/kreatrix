
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
