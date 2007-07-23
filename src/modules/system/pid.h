
#ifndef __KXSYSTEM_PID_H
#define __KXSYSTEM_PID_H

#include "kxobject_struct.h"
#include "kxcore.h"

#define KXPID_VALUE(self) ((pid_t) ((self)->data.intval))

#define IS_KXPID(self) ((self)->extension == &kxpid_extension)

#include <sys/types.h>


typedef struct KxObject KxPid;

KxObject *kxpid_new_prototype(KxCore *core);
KxObject *kxpid_set_pid(KxPid *self, pid_t pid);
void kxpid_extension_init();

extern KxObjectExtension kxpid_extension;

#endif
