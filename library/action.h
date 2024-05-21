// ACTION.H - libgoetia
// Action, conversion function definitions.

#ifndef ACTION_H
#define ACTION_H

enum {
	ACTION_NONE,
	ACTION_PUSH,
	ACTION_PULL,
	ACTION_DEACTIVATE,
	ACTION_GET,
	ACTION_DROP,
	ACTION_LOOKAT,
	ACTION_REST,
	ACTION_EAT,
	ACTION_HIT,
	ACTION_APPROACH,
	ACTION_RETREAT,
	ACTION_LEFT,
	ACTION_RIGHT,
	ACTION_UP,
	ACTION_DOWN,
	ACTION_FORWARD, 
	ACTION_BACKWARD
};

typedef int action;
typedef int agent_id;
typedef int atom;

typedef struct Action {
	action type;
	agent_id target;
} Action;

atom goetia_action_ActionToAtom(action);
action goetia_action_AtomToAction(atom);

#endif // ACTION_H