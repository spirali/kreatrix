
#ifndef __KXSOCKET_H
#define __KXSOCKET_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXSOCKET_DATA(self) ((KxSocketData*) ((self)->data.ptr))

#define IS_KXSOCKET(self) ((self)->extension == &kxsocket_extension)

#define KXSOCKET(sock) (kxsocket_new_with_socket(KXCORE,sock))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define KXLISTEN_QUEUE 16

typedef struct KxObject KxSocket;

typedef struct KxSocketData KxSocketData;

#define KXSOCKET_READBUFFER_SIZE 4096

struct KxSocketData {
	int socket;
	int port;
	char readbuffer[KXSOCKET_READBUFFER_SIZE];
	int readbuffer_pos;
	int readbuffer_size;
};

void kxsocket_extension_init();
KxObject *kxsocket_new_prototype(KxCore *core);
KxSocket *kxsocket_new_with_socket(KxCore *core, int socket);


extern KxObjectExtension kxsocket_extension;

#endif // __KXSOCKET_H
