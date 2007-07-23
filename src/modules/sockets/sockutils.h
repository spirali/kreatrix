#ifndef __SOCKUTILS_H
#define __SOCKUTILS_H

#include "kxsocket.h"
#include "utils/bytearray.h"

int sock_readline(KxSocketData *data, char *str, int length);
int sock_readbytearray(KxSocketData *data, int read_size,ByteArray **bytearray);

int sock_poll_single(int sock,int read, int write, int timeout, int *revents);


#endif
