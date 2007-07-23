
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/poll.h>

#include "sockutils.h"
#include "kxcore.h"
/*
 1 = ok
 0 = nonblock
 -1 = shutdown
 -2 = error
*/
int 
sock_readchar(KxSocketData *data, char *c) 
{
	int size = data->readbuffer_size;
	int pos = data->readbuffer_pos;
	char *buffer = data->readbuffer;
	if (pos < size) {
		*c = buffer[pos];
		data->readbuffer_pos++;
		return 1;
	}
	int readed;
	
	readed = recv(data->socket,buffer,KXSOCKET_READBUFFER_SIZE, 0);

	if (readed > 0) {
		data->readbuffer_pos = 1;
		data->readbuffer_size = readed;
		*c = buffer[0];
		return 1;
	} 
	
	if (readed == 0) {
		return -1;
	}
	return -2;
}

int sock_readline(KxSocketData *data, char *str, int length)
{
	char c;
	int pos = 0;
	for (;;) {
		int ret = sock_readchar(data, &c);
		if (ret == 1) {
			str[pos++] = c;
			if (c == '\n') {
	/*			if (pos > 1) {
					if (str[pos-2] == '\r') {
						pos--;
					}
				}*/
				str[pos] = 0;
				return pos;
			}
			if (pos == length-1) {
				str[pos] = 0;
				return pos;
			}
		} else if (ret == -1) {
			str[pos] = 0;
			return pos;
		} else return ret;
	}
}

int 
sock_readbytearray(KxSocketData *data, int read_size,ByteArray **bytearray)
{
	int size = data->readbuffer_size;
	int pos = data->readbuffer_pos;
	char *buffer = data->readbuffer;
	int bsize = size - pos;
	
	// buffer is empy
	if (pos == size) {
		char buf[read_size];
		int readed = recv(data->socket,buf,read_size, 0);
		if (readed > 0) {
			*bytearray = bytearray_new_from_data(buf, readed);
			return 1;
		} else 
			return (readed==0)?0:-1;
	}

	// all in buffer
	if (read_size <= bsize) {
		*bytearray = bytearray_new_from_data(buffer + pos, read_size);
		data->readbuffer_pos += read_size;
		return 1;
	}
	
	char buf[read_size];
	memcpy(buf, buffer, bsize);
	
	data->readbuffer_pos = 0;
	data->readbuffer_size = 0;

	int readed = recv(data->socket,buf+bsize,read_size-bsize, 0);
	if (readed >= 0) {
		*bytearray = bytearray_new_from_data(buf, bsize+readed);
		return 1;
	} else
		return -1;

	

}

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
