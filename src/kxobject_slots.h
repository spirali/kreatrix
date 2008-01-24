#ifndef __KXOBJECT_SLOTS_H
#define __KXOBJECT_SLOTS_H

#include "kxobject_struct.h"

#define KXOBJECT_SLOTFLAG_FREEZE 1

//#define kxobject_slots_count(self) ((self)->slots_count)

void kxobject_slots_init(KxObject *self);
void kxobject_slots_free(KxObject *self);
void kxobject_slots_clean(KxObject *self);

KxSlot *
kxobject_slot_find(KxObject *self, KxObject *key);

void kxobject_slots_copy(KxObject *self, KxObject *copy);
void kxobject_slot_add(KxObject *self, KxObject *key, KxObject *value, int flags);

List *kxobject_slots_to_list(KxObject *self);

void kxobject_slots_mark(KxObject *self);

void kxobject_slots_dump(KxObject *self);


#endif // __KXOBJECT_SLOTS_H
