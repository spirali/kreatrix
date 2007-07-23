
#ifndef __KXPOLL_H
#define __KXPOLL_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXPOLL_DATA(self) ((KxPollData*) ((self)->data.ptr))

#define IS_KXPOLL(self) ((self)->extension == &kxpoll_extension)

#define KXPOLL(addr) (kxpoll_new_with_addr(KXCORE,addr))

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/poll.h>

typedef struct KxObject KxPoll;

typedef struct KxPollData KxPollData;

struct KxPollData {
	struct pollfd *pfd;
	int pfd_count;
	int timeout;
	List *obj_list;
};

void kxpoll_extension_init();
KxObject *kxpoll_new_prototype(KxCore *core);
KxPoll *kxpoll_new_with_addr(KxCore *core, struct sockaddr_in addr);

struct sockaddr_in kxpoll_to_sockaddr(KxPoll *self);

KxObject *kxpollresult_clone_with(KxObject *self, int result);


extern KxObjectExtension kxpoll_extension;

#endif // __KXPOLL_H
