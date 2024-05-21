// ACTION.H - libgoetia
// Action conversion function implementation.

#include <action.h>
#include <value.h>

/// Converts an action enum to an atom.
atom goetia_action_ActionToAtom(action action) {
	switch(action) {
		case ACTION_NONE: 		return ATOM_NULL;
		case ACTION_PUSH: 		return ATOM_PUSH;
		case ACTION_PULL: 		return ATOM_PULL;
		case ACTION_DEACTIVATE: return ATOM_DEACTIVATE;
		case ACTION_GET: 		return ATOM_GET;
		case ACTION_DROP: 		return ATOM_DROP;
		case ACTION_LOOKAT: 	return ATOM_LOOKAT;
		case ACTION_EAT: 		return ATOM_EAT;
		case ACTION_HIT: 		return ATOM_HIT;
		case ACTION_REST: 		return ATOM_REST;
		case ACTION_APPROACH: 	return ATOM_APPROACH;
		case ACTION_RETREAT: 	return ATOM_RETREAT;
		case ACTION_LEFT: 		return ATOM_LEFT;
		case ACTION_RIGHT: 		return ATOM_RIGHT;
		case ACTION_UP: 		return ATOM_UP;
		case ACTION_DOWN: 		return ATOM_DOWN;
		case ACTION_FORWARD: 	return ATOM_FORWARD;
		case ACTION_BACKWARD: 	return ATOM_BACKWARD;
		
		default: 				return ATOM_NULL;
	}
}

/// Converts an atom to an action enum.
action goetia_action_AtomToAction(atom atom) {
	switch(atom) {
		case ATOM_NULL: 		return ACTION_NONE;
		case ATOM_PUSH: 		return ACTION_PUSH;
		case ATOM_PULL: 		return ACTION_PULL;
		case ATOM_DEACTIVATE: 	return ACTION_DEACTIVATE;
		case ATOM_GET: 			return ACTION_GET;
		case ATOM_DROP: 		return ACTION_DROP;
		case ATOM_LOOKAT: 		return ACTION_LOOKAT;
		case ATOM_EAT: 			return ACTION_EAT;
		case ATOM_HIT: 			return ACTION_HIT;
		case ATOM_REST: 		return ACTION_REST;
		case ATOM_APPROACH: 	return ACTION_APPROACH;
		case ATOM_RETREAT: 		return ACTION_RETREAT;
		case ATOM_LEFT: 		return ACTION_LEFT;
		case ATOM_RIGHT: 		return ACTION_RIGHT;
		case ATOM_UP: 			return ACTION_UP;
		case ATOM_DOWN: 		return ACTION_DOWN;
		case ATOM_FORWARD: 		return ACTION_FORWARD;
		case ATOM_BACKWARD: 	return ACTION_BACKWARD;
		
		default: 				return -1;
	}
}