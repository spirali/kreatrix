
#ifndef __KXMUTEXS_MUTEX_H
#define __KXMUTEXS_MUTEX_H


#include <pthread.h>
#include <sys/types.h>

#include "kxobject_struct.h"
#include "kxcore.h"

#define IS_KXMUTEX(self) ((self)->extension == &kxmutex_extension)

#define KXMUTEX_VALUE(self) ((pthread_mutex_t*) (self)->data.ptr)



typedef struct KxObject KxMutex;

KxObject *kxmutex_new_prototype(KxCore *core);
void kxmutex_extension_init();

extern KxObjectExtension kxmutex_extension;

#endif
