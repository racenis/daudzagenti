// STIMULUS.H - libgoetia
// Stimulus function implementation.

#include <stimulus.h>

#include <value.h>
#include <agent.h>
#include <world.h>

#include <stdio.h>

static Stimulus stimulus_queue[100];
static int stimulus_first = 0;
static int stimulus_last = 0;

/// Inserts a stimulus into the stimulus queue.
void goetia_stimulus_Emit(Stimulus stimulus) {
	stimulus_queue[stimulus_last++] = stimulus;
	if (stimulus_last >= 100) stimulus_last = 0;
}

// this is so that you can access _stim, _obj, _subj, etc. in the stim script
static void bind_stim_vals(Context* c, Stimulus* s) {
	stim type = ATOM_NULL;
	action action = ACTION_NONE;
	agent_id subj = AGENT_NONE;
	agent_id obj = AGENT_NONE;
	chem chemical = CHEM_NONE;
	int ammount = 0;
	
	switch (s->type) {
		case STIMULUS_VISUAL: 
		case STIMULUS_SOUND: 
		case STIMULUS_SMELL: 
		case STIMULUS_TOUCH: 
		case STIMULUS_LAST: 
			printf("Don't use categories as stim types! Enum: %i \n", s->type);
			break;
		
		case STIMULUS_CHEMICAL:
			type = STIMULUS_CHEMICAL;
			break;
		
		case VISUAL_ACTION:
		case VISUAL_APPEAR:
		case VISUAL_LEAVE:
		case VISUAL_MOVE:
		case VISUAL_SECTOR:
			type = STIMULUS_VISUAL;
			break;
			
		case TOUCH_AGENT:
		case TOUCH_WALLBONK:
		case TOUCH_HEADBONK:
		case TOUCH_FLOORBONK:
		case TOUCH_SECTOR:
			type = STIMULUS_TOUCH;
			break;
	
		default:
			printf("Unrecognized stim type! Enum: %i \n", s->type);
	}
	
	
	// TODO: bind also _sector
	
	subj = s->subject;
	obj = s->object;
	action = s->action;
	chemical = s->chemical;
	ammount = s->ammount;
	
	goetia_script_BindValue(c, ATOM_AGENT_STIM, goetia_value_MakeAtom(goetia_stimulus_StimulusToAtom(type)));
	goetia_script_BindValue(c, ATOM_AGENT_SUBTYPE, goetia_value_MakeAtom(goetia_stimulus_StimulusToAtom(s->type)));
	goetia_script_BindValue(c, ATOM_AGENT_ACTION, goetia_value_MakeAtom(goetia_action_ActionToAtom(action)));
	goetia_script_BindValue(c, ATOM_AGENT_SUBJECT, goetia_value_MakeInt(subj));
	goetia_script_BindValue(c, ATOM_AGENT_OBJECT, goetia_value_MakeInt(obj));
	goetia_script_BindValue(c, ATOM_AGENT_CHEM, goetia_value_MakeAtom(goetia_chemical_ChemicalToAtom(chemical)));
	goetia_script_BindValue(c, ATOM_AGENT_AMMOUNT, goetia_value_MakeInt(ammount));	
}

/// Processes all of the queued stimuli.
void goetia_stimulus_Process() {
	int stimulus = stimulus_first;
	stimulus_first = stimulus_last;
	
	while (stimulus != stimulus_last) {
		for (Agent* it = goetia_world_BeginAgent(); it; it = goetia_world_NextAgent(it)) {
			if (!it->stimulus_script) continue;
			
			// here we should check if the agent can receive the stimulus
			// TODO: implement checking function
			
			bind_stim_vals(it->context, &stimulus_queue[stimulus]);
			goetia_script_Evaluate(it->context, it->stimulus_script);
		}
			
		stimulus++;
		if (stimulus >= 100) stimulus = 0;
	}
}

/// Converts a stimulus enum to an atom.
atom goetia_stimulus_StimulusToAtom(atom atom) {
	switch (atom) {
		case STIMULUS_VISUAL: 	return ATOM_VISUAL;
		case STIMULUS_SOUND: 	return ATOM_SOUND;
		case STIMULUS_SMELL: 	return ATOM_SMELL;
		case STIMULUS_CHEMICAL: return ATOM_CHEMICAL;
		case STIMULUS_TOUCH: 	return ATOM_TOUCH;
		
		case VISUAL_ACTION: 	return ATOM_ACTION;
		case VISUAL_APPEAR: 	return ATOM_APPEAR;
		case VISUAL_LEAVE: 		return ATOM_LEAVE;
		case VISUAL_MOVE: 		return ATOM_MOVE;
		case VISUAL_SECTOR: 	return ATOM_SECTOR;
		
		case TOUCH_AGENT:		return ATOM_AGENT;
		case TOUCH_WALLBONK:	return ATOM_WALLBONK;
		case TOUCH_HEADBONK:	return ATOM_HEADBONK;
		case TOUCH_FLOORBONK:	return ATOM_FLOORBONK;
		case TOUCH_SECTOR:		return ATOM_SECTOR;
		
		default: 				return ATOM_NULL;
	}
}

// idk if we even use this function??
/// Converts an atom to a stimulus enum.
stim goetia_stimulus_AtomToStimulus(stim stim) {
	switch (stim) {
		case ATOM_VISUAL: 		return STIMULUS_VISUAL;
		case ATOM_SOUND: 		return STIMULUS_SOUND;
		case ATOM_SMELL: 		return STIMULUS_SMELL;
		case ATOM_CHEMICAL: 	return STIMULUS_CHEMICAL;
		case ATOM_TOUCH: 		return STIMULUS_TOUCH;
		
		case ATOM_ACTION: 		return VISUAL_ACTION;
		case ATOM_APPEAR: 		return VISUAL_APPEAR;
		case ATOM_LEAVE: 		return VISUAL_LEAVE;
		case ATOM_MOVE: 		return VISUAL_MOVE;
		case ATOM_SECTOR: 		return VISUAL_SECTOR;
		
		case ATOM_AGENT:		return TOUCH_AGENT;
		case ATOM_WALLBONK:		return TOUCH_WALLBONK;
		case ATOM_HEADBONK:		return TOUCH_HEADBONK;
		case ATOM_FLOORBONK:	return TOUCH_FLOORBONK;
		//case ATOM_SECTOR:		return TOUCH_SECTOR;
		
		
		default: 				return STIMULUS_LAST; // idk if this is ?
	}
}