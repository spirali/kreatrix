
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
	if (self->data.intval != -1)
		close(self->data.intval);
}

static KxObject *
kxsocket_clone(KxSocket *self)
{
	KxSocket *clone = kxobject_raw_clone(self);
	
	/*KxSocketData *data = calloc(sizeof(KxSocketData),1);
	ALLOCTEST(data);
	
	self->data.intval = -1;
	clone->data.ptr = data; */
	clone->data.intval = -1;
	return clone;
}

KxObject * 
kxsocket_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxsocket_extension;

/*	KxSocketData *data = malloc(sizeof(KxSocketData));
	ALLOCTEST(data);
	
	self->data.intval = -1;

	data->readbuffer_pos = 0;
	data->readbuffer_size = 0;

	self->data.ptr = data;*/

	self->data.intval = -1;
	kxsocket_add_method_table(self);
	return self;
}

KxSocket *
kxsocket_new_with_socket(KxCore *core, int socket)
{
	KxSocket *proto = kxcore_get_prototype(core, &kxsocket_extension);
	KxObject *clone = kxobject_raw_clone(proto);

/*	KxSocketData *data = calloc(sizeof(KxSocketData),1);
	ALLOCTEST(data);
	
	self->data.intval = socket;*/
	/*data->readbuffer_pos = 0;
	data->readbuffer_size = 0;*/


/*	clone->data.ptr = data;*/
	clone->data.intval = socket;
	return clone;
}

static KxObject *
kxsocket_as_integer(KxSocket *self, KxMessage *message)
{
	/*KxSocketData *data = KXSOCKET_DATA(self);
	return KXINTEGER(self->data.intval);*/
	return KXINTEGER(self->data.intval);
}


static KxObject *
kxsocket_as_string(KxSocket *self, KxMessage *message)
{
	char tmp[50];
	snprintf(tmp,50,"%i",self->data.intval);
	return KXSTRING(tmp);
}



static KxObject *
kxsocket_init_tcp(KxSocket *self, KxMessage *message)
{
	if (self->data.intval != -1) {
		KXTHROW_EXCEPTION("Socket is already inicialized");
	}
	
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Socket init failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	
	self->data.intval = sock;
	KXRETURN(self);
}

static KxObject * 
kxsocket_close(KxSocket *self, KxMessage *message)
{
	if (self->data.intval != -1) {
		close(self->data.intval);
		self->data.intval = -1;
	}
	KXRETURN(self);
}

static KxObject *
kxsocket_bind(KxSocket *self, KxMessage *message)
{
	KXPARAM_TO_LONG(port, 0);

	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
	struct sockaddr_in sockname;
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = INADDR_ANY;
    if (bind(self->data.intval, (struct sockaddr *)&sockname, 
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
	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
    if (listen(self->data.intval, KXLISTEN_QUEUE) == -1)	{
		KxException *excp = kxexception_new_with_text(KXCORE,
			"Listen failed: %s", strerror(errno));
		KXTHROW(excp);
    }
	KXRETURN(self);
}

static KxObject *
kxsocket_reuse_address(KxSocket *self, KxMessage *message)
{
	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
	int socket = self->data.intval;
    int one = 1;
    int retval = 
		setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));

	if (retval == -1)	{
		KxException *excp = kxexception_new_with_text(KXCORE,
			"setsockopt failed: %s", strerror(errno));
		KXTHROW(excp);
    }


	KXRETURN(self);
}


static KxObject *
kxsocket_accept(KxSocket *self, KxMessage *message)
{
	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	addrlen = sizeof(sizeof(clientaddr));
	int client;
	
	KX_THREADS_BEGIN
	client = accept(self->data.intval, (struct sockaddr*)&clientaddr, &addrlen);
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
	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in peeraddr;
	int addr_len = sizeof(struct sockaddr_in);
	bzero(&peeraddr, sizeof(struct sockaddr_in));
	
	if (getpeername(self->data.intval, (struct sockaddr*)&peeraddr, 
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

	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	if (param->size == 0)
		return KXINTEGER(0);
	
	errno = 0;
	int sended;

	KX_THREADS_BEGIN
	sended = send(self->data.intval,param->array, param->size, 0);
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
	//	KXTHROW_EXCEPTION("Parameter must be IPAddress");
		return kxobject_type_error(param, &kxipaddress_extension);
	}

	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}

	struct sockaddr_in addr = kxipaddress_to_sockaddr(param);

	if (addr.sin_port <= 0) {
		KXTHROW_EXCEPTION("Port not setted or incorect port number");
	}
	errno = 0;
	int ret;
	
	KX_THREADS_BEGIN
	ret = connect(self->data.intval, (struct sockaddr*) &addr, sizeof(addr));
	KX_THREADS_END

	if (ret == -1) {
		KxException *excp = kxexception_new_with_text(KXCORE,
			"connect failed: %s", strerror(errno));
		KXTHROW(excp);
	}
	
	KXRETURN(self);
}

/*static KxObject *
kxsocket_recv_line(KxSocket *self, KxMessage *message) 
{
	if (self->data.intval == -1) {
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
}*/

static KxObject *
kxsocket_recv_bytearray_with_size(KxSocket *self, KxMessage *message) 
{

	KXPARAM_TO_LONG(size, 0);

	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}
	
	ByteArray *ba;

	int readed;
	char *buffer = malloc(size);
	ALLOCTEST(buffer);
	KX_THREADS_BEGIN
	//ret = sock_readbytearray(data, param_size,&ba);
	readed = recv(self->data.intval,buffer,size, 0);
	KX_THREADS_END
	if(readed>0) {
		return KXBYTEARRAY(bytearray_new_set(buffer, readed));
	}
	free(buffer);
	if (readed == 0) {
		KXRETURN(KXCORE->object_nil);
	}
	KxException *excp = kxexception_new_with_text(KXCORE,"%s", strerror(errno));
	KXTHROW(excp);
}

static KxObject *
kxsocket_as_file_descriptor(KxSocket *self, KxMessage *message) 
{
	return KXINTEGER(self->data.intval);
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

	int revents = 0;
	int r;
	KX_THREADS_BEGIN
	r = sock_poll_single(self->data.intval, 1, 0, param, &revents);
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
	int revents = 0;

	int r;
	KX_THREADS_BEGIN
	r = sock_poll_single(self->data.intval, 0, 1, param, &revents);
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

	/*if (data->readbuffer_pos != data->readbuffer_size) {
		return kxsocket_new_pollresult(self,POLLIN);
	}*/
	int revents = 0;
	int r;

	KX_THREADS_BEGIN
	r = sock_poll_single(self->data.intval, 1, 1, param, &revents);
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
	if (self->data.intval == -1) {
		KXTHROW_EXCEPTION("Socket isn't inicialized");
	}


	socklen_t sz;
	int error = 0;

	getsockopt(self->data.intval,SOL_SOCKET,SO_ERROR,&error,&sz);
	
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
		{"receiveByteArraySize:",1, kxsocket_recv_bytearray_with_size},
		{"connectTo:",1, kxsocket_connect_to},
		{"isError",0, kxsocket_is_error},
		{"asFileDescriptor",0,kxsocket_as_file_descriptor},
		{"waitForReadingTimeout:",1, kxsocket_wait_for_reading_with_timeout},
		{"waitForWritingTimeout:",1, kxsocket_wait_for_writing_with_timeout},
		{"waitForReadingWritingTimeout:",1, kxsocket_wait_for_reading_writing_with_timeout},
		{"setReuseAddress", 0, kxsocket_reuse_address},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}

