
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "kxsocket.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxinteger.h"
#include "kxipaddress.h"
#include "kxbytearray.h"
#include "sockutils.h"
#include "kxpoll.h"

KxObjectExtension kxsocket_extension;

static KxObject *kxsocket_clone(KxSocket *self);

static void kxsocket_add_method_table(KxObject *self);
static void kxsocket_free(KxObject *self);



void kxsocket_extension_init() {
	kxobjectext_init(&kxsocket_extension);
	kxsocket_extension.type_name = "Socket";
	kxsocket_extension.free = kxsocket_free;
	kxsocket_extension.clone = kxsocket_clone;
}

static void kxsocket_free(KxSocket *self) {
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket != -1)
		close(data->socket);
	free(data);
}

static KxObject *
kxsocket_clone(KxSocket *self)
{
	KxSocketData *old_data = KXSOCKET_DATA(self);

	KxSocket *clone = kxobject_raw_clone(self);
	
	KxSocketData *data = calloc(sizeof(KxSocketData),1);
	ALLOCTEST(data);
	
	data->socket = -1;

	clone->data.ptr = data;
	return clone;
}

KxObject * 
kxsocket_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxsocket_extension;

	KxSocketData *data = malloc(sizeof(KxSocketData));
	ALLOCTEST(data);
	
	data->socket = -1;

	data->readbuffer_pos = 0;
	data->readbuffer_size = 0;

	self->data.ptr = data;
	kxsocket_add_method_table(self);
	return self;
}

KxSocket *
kxsocket_new_with_socket(KxCore *core, int socket)
{
	KxSocket *proto = kxcore_get_prototype(core, &kxsocket_extension);
	KxObject *clone = kxobject_raw_clone(proto);

	KxSocketData *data = calloc(sizeof(KxSocketData),1);
	ALLOCTEST(data);
	
	data->socket = socket;
	/*data->readbuffer_pos = 0;
	data->readbuffer_size = 0;*/


	clone->data.ptr = data;
	return clone;
}

static KxObject *
kxsocket_as_integer(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	return KXINTEGER(data->socket);
}


static KxObject *
kxsocket_as_string(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	char tmp[150];
	snprintf(tmp,150,"%i",data->socket);
	/*KxString *string = KXSTRING(tmp);
	RETURN(string);*/
	return KXSTRING(tmp);
}



static KxObject *
kxsocket_init_tcp(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	
	if (data->socket != -1) {
		KXTHROW_EXCEPTION("Socket is already inicialized");
	}
	
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Socket init failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	
	data->socket = sock;
	KXRETURN(self);
}

static KxObject * 
kxsocket_close(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket != -1) {
		close(data->socket);
		data->socket = -1;
	}
	KXRETURN(self);
}

static KxObject *
kxsocket_bind(KxSocket *self, KxMessage *message)
{
	KXPARAM_TO_LONG(port, 0);

	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
	struct sockaddr_in sockname;
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = INADDR_ANY;
    if (bind(data->socket, (struct sockaddr *)&sockname, 
		sizeof(sockname)) == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Bind failed (port %i): %s", port,strerror(errno));
		KXTHROW(excp);
    }
	KXRETURN(self);
}

static KxObject *
kxsocket_listen(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
    if (listen(data->socket, KXLISTEN_QUEUE) == -1)	{
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Listen failed: %s", strerror(errno));
		KXTHROW(excp);
    }
	KXRETURN(self);
}

static KxObject *
kxsocket_accept(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	addrlen = sizeof(sizeof(clientaddr));
	int client;
	
	KX_THREADS_BEGIN
	client = accept(data->socket, (struct sockaddr*)&clientaddr, &addrlen);
	KX_THREADS_END

	if (client == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Accept failed: %s", strerror(errno));
		KXTHROW(excp);
   }
 //  KxIPAddress *kxaddr = KXIPADDRESS(clientaddr); 
    return KXSOCKET(client);
}

static KxObject *
kxsocket_peer_address(KxSocket *self, KxMessage *message)
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in peeraddr;
	int addr_len = sizeof(struct sockaddr_in);
	bzero(&peeraddr, sizeof(struct sockaddr_in));
	
	if (getpeername(data->socket, (struct sockaddr*)&peeraddr, 
		&addr_len) == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"getpeername failed: %s", strerror(errno));
		KXTHROW(excp);
	}

	return KXIPADDRESS(peeraddr);
}

static KxObject *
kxsocket_send_bytearray(KxSocket *self, KxMessage *message) 
{
	KXPARAM_TO_BYTEARRAY(param,0);

	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	if (param->size == 0)
		return KXINTEGER(0);
	
	errno = 0;
	int sended;

	KX_THREADS_BEGIN
	sended = send(data->socket,param->array, param->size, 0);
	KX_THREADS_END

	if (sended == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"send failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	return KXINTEGER(sended);
}

static KxObject * 
kxsocket_connect_to(KxSocket *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	if (!IS_KXIPADDRESS(param)) {
		KXTHROW_EXCEPTION("Parameter must be IPAddress");
	}

	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in addr = kxipaddress_to_sockaddr(param);

	if (addr.sin_port <= 0) {
		KXTHROW_EXCEPTION("Port not setted or incorect port number");
	}
	errno = 0;
	int ret;
	
	KX_THREADS_BEGIN
	ret = connect(data->socket, (struct sockaddr*) &addr, sizeof(addr));
	KX_THREADS_END

	if (ret == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"connect failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	
	KXRETURN(self);
}

static KxObject *
kxsocket_recv_line(KxSocket *self, KxMessage *message) 
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	char line[4096];

	int ret;

	KX_THREADS_BEGIN 
	ret = sock_readline(data, line, 4096);
	KX_THREADS_END

	if (ret > 0) {
		return KXSTRING(line);
	}
	if (ret == 0) {
		KXRETURN(KXCORE->object_nil);
	}
	
	KxException *excp = kxexception_new_with_text(KXCORE,
			"%s", strerror(errno));
	KXTHROW(excp);
}

static KxObject *
kxsocket_recv_bytearray_with_size(KxSocket *self, KxMessage *message) 
{

	KXPARAM_TO_LONG(param_size, 0);

	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
	ByteArray *ba;

	int ret;
	KX_THREADS_BEGIN
	ret = sock_readbytearray(data, param_size,&ba);
	KX_THREADS_END
	switch(ret) {
		case 1: {
			return KXBYTEARRAY(ba);
		}
		case 0: { 
			KXRETURN(KXCORE->object_nil);
		}
		default: {
			KxException *excp = kxexception_new_with_text(KXCORE,"%s", strerror(errno));
			KXTHROW(excp);
		}
	}
}

static KxObject *
kxsocket_as_file_descriptor(KxSocket *self, KxMessage *message) 
{
	KxSocketData *data = KXSOCKET_DATA(self);
	return KXINTEGER(data->socket);
}

KxObject *
kxsocket_new_pollresult(KxSocket *self, int revents)
{
	char *path[3] = { "sockets","Poll","PollResult" };
	KxObject *obj = kxobject_find_object_by_path(self,path,3);
	if (obj) {
		return kxpollresult_clone_with(obj,revents);
	} else 
		KXRETURN(KXCORE->object_nil);
}

static KxObject *
kxsocket_wait_for_reading_with_timeout(KxSocket *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	KxSocketData *data = KXSOCKET_DATA(self);

	if (data->readbuffer_pos != data->readbuffer_size) {
		return kxsocket_new_pollresult(self,POLLIN);
	}

	int revents = 0;
	int r;
	KX_THREADS_BEGIN
	r = sock_poll_single(data->socket, 1, 0, param, &revents);
	KX_THREADS_END
	if (!r) {
			KxException *excp = kxexception_new_with_text(KXCORE,
				"poll failed: %s", strerror(errno));
			KXTHROW(excp);
	}
	
	return kxsocket_new_pollresult(self,revents);
}

static KxObject *
kxsocket_wait_for_writing_with_timeout(KxSocket *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	KxSocketData *data = KXSOCKET_DATA(self);
	int revents = 0;

	int r;
	KX_THREADS_BEGIN
	r = sock_poll_single(data->socket, 0, 1, param, &revents);
	KX_THREADS_END
	if (!r) {
			KxException *excp = kxexception_new_with_text(KXCORE,
				"poll failed: %s", strerror(errno));
			KXTHROW(excp);
	}

	return kxsocket_new_pollresult(self, revents);
}

static KxObject *
kxsocket_wait_for_reading_writing_with_timeout(KxSocket *self, KxMessage *message)
{
	KXPARAM_TO_LONG(param,0);
	KxSocketData *data = KXSOCKET_DATA(self);

	if (data->readbuffer_pos != data->readbuffer_size) {
		return kxsocket_new_pollresult(self,POLLIN);
	}
	int revents = 0;
	int r;

	KX_THREADS_BEGIN
	r = sock_poll_single(data->socket, 1, 1, param, &revents);
	KX_THREADS_END
	if (!r) {
			KxException *excp = kxexception_new_with_text(KXCORE,
				"poll failed: %s", strerror(errno));
			KXTHROW(excp);
	}
	return kxsocket_new_pollresult(self,revents);
	//RETURN_AND_REFREMOVE(obj);
}

static KxObject *
kxsocket_is_error(KxSocket *self, KxMessage *message) 
{
	KxSocketData *data = KXSOCKET_DATA(self);
	if (data->socket == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}


	socklen_t sz;
	int error = 0;

	getsockopt(data->socket,SOL_SOCKET,SO_ERROR,&error,&sz);
	
	KXRETURN_BOOLEAN(error);
}

void static
kxsocket_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"asString",0, kxsocket_as_string },
		{"initTCP",0, kxsocket_init_tcp },
		{"close",0, kxsocket_close },
		{"bindPort:",1, kxsocket_bind },
		{"listen",0, kxsocket_listen},
		{"accept",0, kxsocket_accept},
		{"peerAddress",0, kxsocket_peer_address},
		{"sendByteArray:",1, kxsocket_send_bytearray},
		{"receiveLine",0, kxsocket_recv_line},
		{"receiveByteArrayWithSize:",1, kxsocket_recv_bytearray_with_size},
		{"connectTo:",1, kxsocket_connect_to},
		{"isError",0, kxsocket_is_error},
		{"asFileDescriptor",0,kxsocket_as_file_descriptor},
		{"waitForReadingWithTimeout:",1, kxsocket_wait_for_reading_with_timeout},
		{"waitForWritingWithTimeout:",1, kxsocket_wait_for_writing_with_timeout},
		{"waitForReadingWritingWithTimeout:",1, kxsocket_wait_for_reading_writing_with_timeout},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

