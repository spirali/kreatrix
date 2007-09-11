#ifndef __SOCKUTILS_H
#define __SOCKUTILS_H

#include "kxsocket.h"
#include "utils/bytearray.h"

int sock_poll_single(int sock,int read, int write, int timeout, int *revents);


#endif
