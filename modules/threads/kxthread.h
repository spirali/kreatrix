
#ifndef __KXTHREADS_THREAD_H
#define __KXTHREADS_THREAD_H


#include <pthread.h>
#include <sys/types.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXTHREAD(self) ((self)->extension == &kxthread_extension)

#define KXTHREAD_DATA(self) ((KxThreadData*) (self)->data.ptr)



typedef struct KxObject KxThread;
typedef struct KxThreadData KxThreadData;

KxObject *kxthread_new_prototype(KxCore *core);
void kxthread_extension_init();

typedef enum  { 
	KXTHREAD_STATE_INIT = 0,
	KXTHREAD_STATE_RUNNING_JOINABLE,
	KXTHREAD_STATE_RUNNING_DETACHED,
	KXTHREAD_STATE_WAITING_FOR_JOIN,
	KXTHREAD_STATE_STOPPED
} KxThreadState;

struct KxThreadData {
	KxThreadState state;
	pthread_t pthread;
	KxObject *return_value;
};

extern KxObjectExtension kxthread_extension;

#endif
