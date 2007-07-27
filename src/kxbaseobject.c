/**********************************************************************
 *
 *	$Id$
 *
 *  Implementation of Object
 *
 **********************************************************************/


/*KXobject Base Object
	[Basic prototypes] Base object. 
	Parent for all objects.
  
*/


#include <stdio.h>
#include <stdlib.h>

#include "kxbaseobject.h"
#include "kxobject.h"
#include "kxsymbol.h"
#include "kxstring.h"
#include "kxexception.h"
#include "kxcodeblock.h"
#include "kxlist.h"
#include "kxinteger.h"

KxObject * kxobject_new();

KxObject *
kxbaseobject_new(KxCore *core)
{
	KxObject *object = kxobject_new();
	object->core = core;


	return object;
}

/*KXdoc slot:set: aSymbol anObject
  [Slots] Creates new slot in the receiver.
  Method creates new slot with name aSymbol in receiver. anObject is placed into new slot.
  If slot already exists in receiver, slot is overriden.<br/>
  NOTE: Alias "=" use this message.
*/
static KxObject *
kxbaseobject_set_slot(KxObject *self, KxMessage *message)
{
	KxObject *symbol = message->params[0];
	/*if (!IS_KXSYMBOL(symbol)) {
		KXTHROW_EXCEPTION("Symbol expected\n");
	}*/
	KXCHECK_SYMBOL(symbol);
	kxobject_set_slot(self,symbol,message->params[1]);

	KXRETURN(self);
}

/*KXdoc slot:update: aSymbol anObject
  [Slots] Update slot in the receiver.
  Looks for slot with name aSymbol in receiver and it's parents. 
  If slot is found, anObject is placed into slot.
  Exception is thrown if slot does't exist.
  <br/>
  NOTE: Alias "<<" use this message.
*/
static KxObject *
kxbaseobject_update_slot(KxObject *self, KxMessage *message)
{
	KxObject *symbol = message->params[0];

	KXCHECK_SYMBOL(symbol);

	if (kxobject_update_slot(message->start_search,symbol,message->params[1])) {
		KXRETURN(message->params[1]);
	} else {
		KxException *excp = kxexception_new_with_text(KXCORE, "Slot '%s' not found", (char*)symbol->data.ptr);
		KXTHROW(excp);
	}
}

static KxObject *
kxbaseobject_as_string(KxObject *self, KxMessage *message)
{
	KxString *typename = kxobject_type_name(self);
	char tmp[255];
	if (IS_KXSTRING(typename)) 
		snprintf(tmp,255,"%p_%s",self, KXSTRING_VALUE(typename));
	else
		snprintf(tmp,255,"%p_<invalid type>",self);
	REF_REMOVE(typename);
	return KXSTRING(tmp);
}

/*KXdoc parent
  [Parents] Returns first parent of receiver.
  Returns first parent of receiver. Returns nil if receiver has no parents.
*/
static KxObject *
kxbaseobject_parent(KxObject *self, KxMessage *message)
{
	if (self->parent_slot.parent == NULL)
		KXRETURN(KXCORE->object_nil);
	KXRETURN(self->parent_slot.parent);
}

/*KXdoc parents
  [Parents] Returns list with receiver's parents.
*/
static KxObject *
kxbaseobject_parents(KxObject *self, KxMessage *message)
{
	List *list = list_new();
	KxParentSlot *pslot = &self->parent_slot;
	while (pslot != NULL) {
		KxObject *parent = pslot->parent;
		REF_ADD(parent);
		list_append(list, parent);
		pslot = pslot->next;
	}
	return KXLIST(list);
}


/*KXdoc throw: anObject
  Throws anObject. 
  This is primary designed for childs of Exception, but every object can be thrown.
  If object isn't catched in program, slot "message" in anObject is read.
*/
static KxObject *
kxbaseobject_throw(KxObject *self, KxMessage *message) 
{
	REF_ADD(message->params[0]);
	KXTHROW(message->params[0]);
}

/*KXdoc clone
 Returns clone of receiver. 
 Returns new object, clone of receiver. Clone is empty object where parent is target of this message.
 Sends message "init" to clone.
*/
static KxObject *
kxbaseobject_clone(KxObject *self, KxMessage *message)
{
	return kxobject_clone(self);
}

/*KXdoc copy
 Return shallow copy of receiver. Objects with internal data should redefine this message.
*/
static KxObject *
kxbaseobject_copy(KxObject *self, KxMessage *message)
{
	if (self->extension) {
		// Object with inner data should override this method!
		KXTHROW(kxexception_new_with_text(KXCORE,"Can't create copy of object %s", kxobject_raw_type_name(self)));
	}
	return kxobject_raw_copy(self);
}


/*KXdoc type
 Returns string contains type of object.
*/
static KxObject *
kxbaseobject_type(KxObject *self, KxMessage *message)
{
	char *typename = kxobject_raw_type_name(self);
	return KXSTRING(typename);
}

/*KXdoc isKindOf: anObject
 [Comparing] Test of inheritance. 
 Returns true if anObject is receiver or one of receiver's ancestors. Otherwise returns false.
*/
static KxObject *
kxbaseobject_is_kind_of(KxObject *self, KxMessage *message)
{
	int cond = kxobject_is_kind_of(self,message->params[0]);
	KXRETURN_BOOLEAN(cond);
}

/*KXdoc do: aMethod
  Process aMethod in context of receiver.
  All messages without target is directly send to receiver (without sending to activation context).
  <pre>
  myObject slot1 = "something".
  myObject myslot = space clone.
  <br/>
  // is equivalent to:
  <br/>
  myObject do: {
		slot1 = "something".
		myslot = space clone.
  }.
  </pre>
*/
static KxObject *
kxbaseobject_do(KxObject *self, KxMessage *message)
{
	KxObject *param = message->params[0];
	if (!IS_KXCODEBLOCK(param)) {
		KXTHROW_EXCEPTION("Parameter of 'do:' must be method");
	}
	
	KxMessage msg;
	msg.message_name = NULL;
	msg.params_count = 0;
	msg.target = self;
	msg.slot_holder = self;
	return kxcodeblock_run(param, self, &msg, KXCB_RUN_NORMAL);
}

/*KXdoc slots
  [Slots] Returns slot's names of receiver
  Returns list with names (symbols) of all receiver's slots.
*/
static KxObject *
kxbaseobject_slots(KxObject *self, KxMessage *message)
{
	List *list = kxobject_slots_to_list(self);
	return kxlist_new_with(KXCORE, list);
}

/*KXdoc perform: aMessageName
  Send unary message.
  Send unary message to receiver. aMessageName is name (symbol) of message.
*/
static KxObject * 
kxbaseobject_perform(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KXCHECK_SYMBOL(param);
	KxMessage msg;
	kxmessage_init(&msg, self, 0, param);
	return kxmessage_send(&msg);
}

/*KXdoc perform:with: aMessageName aParameter
  Send message with one parameter.
  Send message with one parameter to receiver. aMessageName is name (symbol) of message.
  aParameter is parameter for message.
*/
static KxObject * 
kxbaseobject_perform_with(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KXCHECK_SYMBOL(param);
	KxMessage msg;
	kxmessage_init(&msg, self, 1, param);
	msg.params[0] = message->params[1];
	REF_ADD(msg.params[0]);
	return kxmessage_send(&msg);
}

/*KXdoc perform:withList: aMessageName aParameterList
  Send message with more parameters.
  Send message to receiver. aMessageName is name (symbol) of message.
  aParameterList is list parameters for message.

*/
static KxObject * 
kxbaseobject_perform_with_list(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KxObject *param1 = message->params[1];

	KXCHECK_SYMBOL(param);

	if (!IS_KXLIST(param1)) {
		kxexception_type_error("List",param1);
	}
	
	List *list = param1->data.ptr;
	
	KxMessage msg;
	kxmessage_init(&msg, self, list->size, param);

	int t;
	for (t=0;t<list->size;t++) {
		msg.params[t] = list->items[t];
		REF_ADD(msg.params[t]);
	}

	return kxmessage_send(&msg);
}


/*KXdoc slot: aSlotName
  [Slots] Return receiver's slot.
  Return object in receiver's slot with name aSlotName.
 */
static KxObject * 
kxbaseobject_get_slot(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KXCHECK_SYMBOL(param);
	KxObject *object = kxobject_find_slot(message->start_search, param);
	if (object) {
		KXRETURN(object);
	} else {
		KxException *excp = kxexception_new_with_text(KXCORE,"Slot '%s' not found", param->data);
		KXTHROW(excp);
	}
}

/*KXdoc respondsTo: aMessageName
 Test if object is able to receive message.
 Return true if target responds to slot with name aMessageName. Otherwise returns false.
 Slots of reciver and receiver's ancestors are searched.
*/
static KxObject *
kxbaseobject_responds_to(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KXCHECK_SYMBOL(param);

	KxObject *object = kxobject_find_slot(message->start_search, param);
	KXRETURN_BOOLEAN(object != NULL);
}

/*KXdoc hasSlot: aMessageName
 [Slots] Answer whether receiver has specific slot.
 Returns true if receiver has slot with name aMessageName. (Target's ancestors aren't searched).
*/
static KxObject *
kxbaseobject_has_slot(KxObject *self, KxMessage *message) 
{
	KxObject *param = message->params[0];
	KXCHECK_SYMBOL(param);

	KxObject *object = kxobject_get_slot(self, param);
	KXRETURN_BOOLEAN(object != NULL);
}

/*KXdoc isActivable
 Answer whether receiver is activable object.
 Returns true if receiver is activable object. It means that object is performed when slot is lookuped.
 If object isn't activable, it returns self if slot is lookuped.
*/
static KxObject *
kxbaseobject_is_activable(KxObject *self, KxMessage *message) 
{
	KXRETURN_BOOLEAN(self->extension && self->extension->activate);
}


/*KXdoc addParent: anObject
 [Parents] Add parent to receiver.
 Add aObject at the end of receivers's list of parents.
*/
static KxObject *
kxbaseobject_add_parent(KxObject *self, KxMessage *message)
{
	kxobject_add_parent(self, message->params[0]);

	KXRETURN(self);
}

/*KXdoc removeParent: anObject
 [Parents] Remove receiver's parent.
 Remove anObject from receiver's list of parents.
*/
static KxObject *
kxbaseobject_remove_parent(KxObject *self, KxMessage *message)
{
	kxobject_remove_parent(self, message->params[0]);
	KXRETURN(self);
}

/*KXdoc insertParent: anObject
 [Parents] Add parent at the beginning of receivers's list of parents.
 Add anObject at the beginning of receiver's list of parents.
*/
static KxObject *
kxbaseobject_insert_parent(KxObject *self, KxMessage *message)
{
	kxobject_insert_parent(self, message->params[0]);
	KXRETURN(self);
}

/**
 *   Not directly called, use help function for baseobject_slot_update_pair_set
 *   Update slot with name symbol_frame[0]
 */ 
static KxObject *
kxbaseobject_slot_updater(KxObject *self, KxMessage *message)
{
	KxObject *obj = message->params[0];

	KxSymbol **frame = KXCFUNCTION_OBJECTS;
	kxobject_set_slot(self, frame[0], obj);

	KXRETURN(self);
}
/*Kxdoc slot:updater:set:
 [Slots]
*/
static KxObject *
kxbaseobject_slot_updater_set(KxObject *self, KxMessage *message)
{
	KxObject *symbol = message->params[0];
	KXCHECK_SYMBOL(symbol);

	KxObject *update_pair = message->params[1];
	KXCHECK_SYMBOL(update_pair);

	kxobject_set_slot(self,symbol,message->params[2]);

	KxObject *prototype = kxcore_get_basic_prototype(KXCORE,KXPROTO_CFUNCTION);
	KxCFunction *cfunction = kxcfuntion_clone_with(prototype, self->extension, 1, kxbaseobject_slot_updater);
	kxcfunction_objects_set_one(cfunction, symbol);

	kxobject_set_slot(self,update_pair,cfunction);

	REF_REMOVE(cfunction);

	KXRETURN(message->params[2]);
}

/*KXdoc isSame: anObject
[Comparing] Identity test.
Tests if receiver is exactly same object as anObject.
*/
static KxObject *
kxbaseobject_is_same(KxObject *self, KxMessage *message)
{
	KXRETURN_BOOLEAN(self == message->params[0]);
}

static KxObject *
kxbaseobject_identity_hash(KxObject *self, KxMessage *message) 
{
	return KXINTEGER((long)self);
}

/*KXdoc freezeSlot: aSlotName
 [Slots] Activable object is no longer evaluated in this slot.
 If slot is "freezed" than slot returns directly it's content though
 this object is activable. For example CodeBlock or CFunction is no evaluated in
 this freezed slot. Note: Slot "self", local slots and parameter is freezed by default.
 */
static KxObject *
kxbaseobject_freeze_slot(KxObject *self, KxMessage *message)
{
	KxObject *symbol = message->params[0];
	KXCHECK_SYMBOL(symbol);
	kxobject_update_slot_flags(message->start_search, symbol, KXOBJECT_SLOTFLAG_FREEZE);
	KXRETURN(self);
}

/*KXdoc unfreezeSlot: aSlotName
  [Slots] Cancel effect of "freezed" slot. 
  Returns slot to normal behavior after freezing. See "freezeSlot:".
*/
static KxObject *
kxbaseobject_unfreeze_slot(KxObject *self, KxMessage *message)
{
	KxObject *symbol = message->params[0];
	KXCHECK_SYMBOL(symbol);
	kxobject_update_slot_flags(message->start_search, symbol, 0);
	KXRETURN(self);
}


void 
kxbaseobject_add_method_table(KxObject *self)
{
	KxMethodTable table[] = {
		{"slot:set:",2, kxbaseobject_set_slot},
		{"slot:update:",2, kxbaseobject_update_slot},
		{"slot:updater:set:", 3, kxbaseobject_slot_updater_set},
		{"asString",0, kxbaseobject_as_string },
		{"parent",0, kxbaseobject_parent },
		{"parents",0, kxbaseobject_parents },
		{"clone",0, kxbaseobject_clone },
		{"copy",0, kxbaseobject_copy },
		{"slot:",1, kxbaseobject_get_slot},
		{"throw:",1, kxbaseobject_throw},
		{"type",0, kxbaseobject_type},
		{"isKindOf:",1, kxbaseobject_is_kind_of},
		{"slots",0, kxbaseobject_slots},
		{"do:",1, kxbaseobject_do},
		{"isSame:",1, kxbaseobject_is_same},
		{"respondsTo:", 1, kxbaseobject_responds_to},
		{"hasSlot:", 1, kxbaseobject_has_slot},
		{"isActivable", 0, kxbaseobject_is_activable},
		{"addParent:", 1, kxbaseobject_add_parent},
		{"removeParent:", 1, kxbaseobject_remove_parent},
		{"insertParent:", 1, kxbaseobject_insert_parent},

		{"perform:",1, kxbaseobject_perform},
		{"perform:with:",2, kxbaseobject_perform_with},
		{"perform:withList:",2, kxbaseobject_perform_with_list},
		{"identityHash",0, kxbaseobject_identity_hash },
		{"freezeSlot:",1, kxbaseobject_freeze_slot },
		{"unfreezeSlot:",1, kxbaseobject_unfreeze_slot },
		{NULL,0, NULL}
	};
	kxobject_add_methods(self, table);
}
