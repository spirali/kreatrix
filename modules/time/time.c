#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include "kxobject.h"
#include "kxmodule.h"
#include "kxcore.h"
#include "kxexception.h"
#include "kxinteger.h"

static void kxclock_add_method_table(KxObject *self);


KxObject *
kxmodule_main(KxModule *self, KxMessage *message)
{
	kxclock_add_method_table(self);
	KXRETURN(self);
}

void
kxmodule_unload(KxModule *self)
{
	
}

static KxObject *
kxclock_millisecond_clock_value(KxObject *self, KxMessage *message)
{
	struct timeval tv;
	errno = 0;
	if (gettimeofday(&tv, NULL)) {
		KXTHROW(kxexception_from_errno(KXCORE,"millisecondClockValue"));
	}
	return KXINTEGER(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static void
kxclock_add_method_table(KxObject *self)
{
	KxMethodTable table[] = {
		{"millisecondClockValue",0,kxclock_millisecond_clock_value},

		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
