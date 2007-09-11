
#include <stdlib.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

#include "kxipaddress.h"
#include "kxobject.h"
#include "utils/utils.h"
#include "kxstring.h"
#include "kxinteger.h"
#include "kxexception.h"

KxObjectExtension kxipaddress_extension;

static KxObject *kxipaddress_clone(KxIPAddress *self);

static void kxipaddress_add_method_table(KxObject *self);
static void kxipaddress_free(KxObject *self);



void kxipaddress_extension_init() {
	kxobjectext_init(&kxipaddress_extension);
	kxipaddress_extension.type_name = "IPAddress";
	kxipaddress_extension.free = kxipaddress_free;
	kxipaddress_extension.clone = kxipaddress_clone;
}

static void kxipaddress_free(KxIPAddress *self) {
	KxIPAddressData *data = KXIPADDRESS_DATA(self);
	free(data);
}

static KxObject *
kxipaddress_clone(KxIPAddress *self)
{
	KxIPAddressData *old_data = KXIPADDRESS_DATA(self);

	KxIPAddress *clone = kxobject_raw_clone(self);
	
	KxIPAddressData *data = malloc(sizeof(KxIPAddressData));
	ALLOCTEST(data);
	
	memcpy(&data->addr,&old_data->addr,sizeof(data->addr));
	//data->addr_len = old_data->addr_len;
	
	clone->data.ptr = data;
	return clone;
}

KxObject * 
kxipaddress_new_prototype(KxCore *core) 
{
	KxObject *self = kxcore_clone_base_object(core);
	self->extension = &kxipaddress_extension;

	KxIPAddressData *data = malloc(sizeof(KxIPAddressData));
	ALLOCTEST(data);
	
	bzero(&data->addr, sizeof(data->addr));
	data->addr.sin_family = AF_INET;
	//data->addr_len = 0;

	self->data.ptr = data;

	kxipaddress_add_method_table(self);
	return self;
}

KxIPAddress *
kxipaddress_new_with_addr(KxCore *core, struct sockaddr_in addr)
{
	KxIPAddress *proto = kxcore_get_prototype(core, &kxipaddress_extension);
	KxObject *clone = kxobject_raw_clone(proto);

	KxIPAddressData *data = malloc(sizeof(KxIPAddressData));
	ALLOCTEST(data);
	
	memcpy(&data->addr,&addr, sizeof(data->addr));


	clone->data.ptr = data;
	return clone;
}

struct sockaddr_in 
kxipaddress_to_sockaddr(KxIPAddress *self) 
{
	KxIPAddressData *data = KXIPADDRESS_DATA(self);

	/*int t;
	char *c = (char*)(&data->addr);
	printf(">>");
	for (t=0;t<sizeof(struct sockaddr_in);t++) {
		printf("%i ",c[t]);
	}
	printf("\n",c[t]);*/
	return data->addr;
}

static KxObject *
kxipaddress_from_hostname(KxIPAddress *self, KxMessage *message)
{

	KXPARAM_TO_CSTRING(param,0);
	
	struct hostent *host;

	KX_THREADS_BEGIN
	host = gethostbyname(param);
	KX_THREADS_END

	if (host == NULL) {
		char *str = (char*) hstrerror(h_errno);
		KxException *excp = kxexception_new_with_text(KXCORE,
		"Hostname '%s': %s",param,str);
		KXTHROW(excp);
	}

	KxObject *kxipaddr = kxcore_clone_prototype(KXCORE, &kxipaddress_extension);

	KxIPAddressData *data = KXIPADDRESS_DATA(kxipaddr);
	
	memcpy(&data->addr.sin_addr, host->h_addr, host->h_length);
	//printf("SZ = %i %i\n",host->h_length, sizeof(struct sockaddr_in));
	//data->addr_len = host->h_length;
	
	//KXRETURN(kxipaddr);
	return kxipaddr;
}




static KxObject *
kxipaddress_as_string(KxIPAddress *self, KxMessage *message)
{
	KxIPAddressData *data = KXIPADDRESS_DATA(self);
	char *str = inet_ntoa(data->addr.sin_addr);

	/*KxString *string;

	if (data->addr.sin_port > 0) {
		char tmp[250];
		snprintf(tmp,250,"%s:%i",str, ntohs(data->addr.sin_port));
		string = KXSTRING(tmp);
	} else 
		string = KXSTRING(str);
	return string;*/
	return KXSTRING(str);
}

static KxObject *
kxipaddress_set_port(KxIPAddress *self, KxMessage *message)
{
	KxIPAddressData *data = KXIPADDRESS_DATA(self);

	KXPARAM_TO_LONG(port,0);

	data->addr.sin_port = htons(port);
	KXRETURN(self);
}

static KxObject *
kxipaddress_port(KxIPAddress *self, KxMessage *message)
{
	KxIPAddressData *data = KXIPADDRESS_DATA(self);
	
	return KXINTEGER(ntohs(data->addr.sin_port));
}



void static
kxipaddress_add_method_table(KxObject *self) {
	KxMethodTable table[] = {
		{"fromHostName:",1, kxipaddress_from_hostname },
		{"asString",0, kxipaddress_as_string },
		{"port:",1,kxipaddress_set_port},
		{"port",0,kxipaddress_port},
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
