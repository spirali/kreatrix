/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/


#include "kxnil.h"

KxObject *
kxnil_new(KxCore *core) {
	return kxcore_clone_base_object(core);
}
