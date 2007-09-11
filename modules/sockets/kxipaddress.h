
#ifndef __KXIPADDRESS_H
#define __KXIPADDRESS_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXIPADDRESS_DATA(self) ((KxIPAddressData*) ((self)->data.ptr))

#define IS_KXIPADDRESS(self) ((self)->extension == &kxipaddress_extension)

#define KXIPADDRESS(addr) (kxipaddress_new_with_addr(KXCORE,addr))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct KxObject KxIPAddress;

typedef struct KxIPAddressData KxIPAddressData;

struct KxIPAddressData {
	struct sockaddr_in addr;
	//socklen_t addr_len;
};

void kxipaddress_extension_init();
KxObject *kxipaddress_new_prototype(KxCore *core);
KxIPAddress *kxipaddress_new_with_addr(KxCore *core, struct sockaddr_in addr);

struct sockaddr_in kxipaddress_to_sockaddr(KxIPAddress *self);


extern KxObjectExtension kxipaddress_extension;

#endif // __KXIPADDRESS_H
