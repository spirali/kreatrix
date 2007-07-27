/**********************************************************************
 *
 *	$Id$
 *
 **********************************************************************/

/*KXobject Base nil
   [Singletons]
*/


#include "kxnil.h"

KxObject *
kxnil_new(KxCore *core) {
	return kxcore_clone_base_object(core);
}
