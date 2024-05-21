// STIMULUS.H - libgoetia
// Stimulus definition, function definitions.

#ifndef STIMULUS_H
#define STIMULUS_H

#include <vector.h>

typedef int atom;
typedef int agent_id;
typedef int sector_id;
typedef int action;
typedef unsigned char chem;

// we can have a little bit of an enum
enum {
	STIMULUS_VISUAL,
	STIMULUS_SOUND,		// not used (yet)
	STIMULUS_SMELL,		// not used, might be yeeted
	STIMULUS_CHEMICAL,
	STIMULUS_TOUCH,
	STIMULUS_LAST,
	
	VISUAL_ACTION,		// an agent has performed an action
	VISUAL_APPEAR,		// an agent has appeared into field of view
	VISUAL_LEAVE,		// an agent has left the field of view
	VISUAL_MOVE,		// an agent has moved
	VISUAL_SECTOR,		// an agent perceives a sector
	
	TOUCH_AGENT,		// agent collides with another agent
	TOUCH_WALLBONK,		// agent collides with wall
	TOUCH_HEADBONK,		// agent collides with ceiling
	TOUCH_FLOORBONK,	// agent collides with floor
	TOUCH_SECTOR		// agent moves into a sector
};

typedef int stim;

typedef struct Stimulus {
	stim type;
	
	sector_id sector;
	Vector3 position;
	
	agent_id subject;
	agent_id object;
	action action;
	
	chem chemical;
	int ammount;	
} Stimulus;

void goetia_stimulus_Emit(Stimulus stimulus);
void goetia_stimulus_Process();


atom goetia_stimulus_StimulusToAtom(atom atom);
stim goetia_stimulus_AtomToStimulus(stim stim);

#endif // STIMULUS_H