// AGENT.H - libgoetia
// Agent, function definitions.

#ifndef AGENT_H
#define AGENT_H

#include <list.h>
#include <eval.h>
#include <chemical.h>
#include <vector.h>
#include <action.h>
#include <stimulus.h>

typedef int agent_id;
typedef int sector_id;
static const agent_id AGENT_NONE = 0;

enum {
	FLAG_FLOAT_IN_AIR = 1,
	FLAG_FLOAT_IN_WATER = 2,
	FLAG_ALL_ENERGY = 4,
	FLAG_ALL_SENSOR = 8,
	FLAG_DEAD = 16,
	FLAG_IMMORTAL = 32,
	FLAG_DISALLOW_PICKUP = 64,
	FLAG_DYNAMICS_ASLEEP = 128,
	FLAG_DYNAMICS_DISABLE = 256
};

typedef struct Agent {
	agent_id id;
	atom name;
	
	const char* description;
	
	atom category;
	atom category_visual;
	atom category_tactile;
	
	float move_speed;
	float turn_speed;
	float accl_speed;
	
	Vector3 position;
	Vector3 velocity;
	Shape collision;
	float rotation;
	float mass;
	
	Action action;
	
	agent_id held_by;
	agent_id holding;
	
	sector_id sector;
	
	int flags;
	
	ChemContainer* chemicals;
	
	Context* context;
	List* init_script;
	List* uninit_script;
	List* tick_script;
	List* stimulus_script;
	List* action_script;
} Agent;

Agent goetia_agent_New();
Agent goetia_agent_Copy(Agent* agent);
void goetia_agent_Delete(Agent* agent);

Agent goetia_agent_AgentFromList(List* list);
List* goetia_agent_ListFromAgent(Agent* agent);

void goetia_agent_PerformAction(Agent* agent);

void goetia_agent_PrintInfo(Agent* agent);
#endif // AGENT_H
