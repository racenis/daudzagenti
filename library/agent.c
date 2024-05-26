// AGENT.C - libgoetia
// Agent function implementation.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <agent.h>
#include <world.h>
#include <api.h>

/// Initializes a new Agent.
/// @return Agent with default values.
Agent goetia_agent_New() {
	Agent agent;
	
	agent.id = 0;
	agent.name = ATOM_NULL;
	
	agent.description = NULL;
	
	agent.category = ATOM_NULL;
	agent.category_visual = ATOM_NULL;
	agent.category_tactile = ATOM_NULL;
	
	agent.move_speed = 0.02f;
	agent.turn_speed = 0.05f;
	agent.accl_speed = 1.0f;
	
	agent.position.x = 0.0f;
	agent.position.y = 0.0f;
	agent.position.z = 0.0f;
	
	agent.velocity.x = 0.0f;
	agent.velocity.y = 0.0f;
	agent.velocity.z = 0.0f;
	
	agent.collision.type = SHAPE_NONE;
	agent.collision.radius = 0.0f;
	agent.collision.height = 0.0f;
	
	agent.rotation = 0.0f;
	agent.mass = 0.0f;
	
	Action action = {.type = ACTION_NONE, .target = 0};
	agent.action = action; 
	
	agent.held_by = 0;
	agent.holding = 0;
	
	agent.sector = 0;
	
	agent.flags = 0;
	
	agent.chemicals = NULL;
	
	agent.init_script = NULL;
	agent.uninit_script = NULL;
	agent.tick_script = NULL;
	agent.stimulus_script = NULL;
	agent.action_script = NULL;
	
	// create script context
	agent.context = goetia_script_MakeContext();
	
	return agent;
}

/// Creates a copy of an agent.
Agent goetia_agent_Copy(Agent* other) {
	Agent agent = goetia_agent_New();
	
	agent.description = other->description;
	
	agent.category = other->category;
	agent.category_visual = other->category_visual;
	agent.category_tactile = other->category_tactile;
	
	agent.move_speed = other->move_speed;
	agent.turn_speed = other->turn_speed;
	agent.accl_speed = other->accl_speed;
	
	agent.position = other->position;
	
	agent.velocity = other->velocity;
	
	agent.collision = other->collision;
	
	agent.rotation = other->rotation;
	agent.mass = other->mass;
	
	agent.flags = other->flags;
	
	// TODO: properly copy the script!!!
	agent.init_script = other->init_script;
	agent.uninit_script = other->uninit_script;
	agent.tick_script = other->tick_script;
	agent.stimulus_script = other->stimulus_script;
	agent.action_script = other->action_script;
	
	if (other->chemicals) {
		agent.chemicals = goetia_chemical_CopyContainer(other->chemicals);
	}
	
	return agent;
}

/// Frees up resources used by the agent.
void goetia_agent_Delete(Agent* agent) {
	// TODO: implement
}

// TODO: move these to value.c
static int get_int(List* list) {
	if (list->data.type == VALUE_FLOAT) {
		return list->data.float32;
	} 
	
	if (list->data.type == VALUE_INT) {
		return list->data.int32;
	}
	
	return 0;
}

static float get_float(List* list) {
	if (list->data.type == VALUE_FLOAT) {
		return list->data.float32;
	} 
	
	if (list->data.type == VALUE_INT) {
		return list->data.int32;
	}
	
	return 0;
}

/// Initializes a new Agent from a list definition.
/// @return Agent with parameters taken from the list.
Agent goetia_agent_AgentFromList(List* list) {
	Agent agent = goetia_agent_New();
	
	List* prop = list->data.list;
	
	do switch (prop->data.atom) {
		case ATOM_ID:
			agent.id = prop->next->data.int32; break;
		case ATOM_NAME:
			agent.name = prop->next->data.atom; break;
		case ATOM_DESCRIPTION:
			agent.description = prop->next->data.str; break;
		case ATOM_SPEED:
			if (prop && (prop = prop->next)) agent.move_speed = get_float(prop);
			if (prop && (prop = prop->next)) agent.turn_speed = get_float(prop);
			if (prop && (prop = prop->next)) agent.accl_speed = get_float(prop);
			break;
		case ATOM_POSITION:
			agent.position.x = get_float(prop->next);
			agent.position.y = get_float(prop->next->next);
			agent.position.z = get_float(prop->next->next->next);
			break;
		case ATOM_ROTATION:
			agent.rotation = get_float(prop->next);
		case ATOM_COLLISION:
			switch (prop->next->data.atom) {
				case ATOM_SPHERE:
					agent.collision.type = SHAPE_SPHERE;
					agent.collision.radius = get_float(prop->next->next);
					break;
				case ATOM_CYLINDER:
					agent.collision.type = SHAPE_CYLINDER;
					agent.collision.radius = get_float(prop->next->next);
					break;
				case ATOM_CUBE:
					agent.collision.type = SHAPE_CUBE;
					agent.collision.radius = get_float(prop->next->next);
					break;
				default: break;
			} break;
		case ATOM_MASS:
			agent.mass = get_float(prop->next); break;
		case ATOM_CHEMICAL:
			agent.chemicals = goetia_chemical_ListToContainer(prop->next->data.list); break;
		case ATOM_SKELETON: break;
		case ATOM_CONTEXT: break; // TODO: implement
		
		case ATOM_FLAGS:
			while (prop = prop->next) switch (prop->data.atom) {
				case ATOM_FLOAT_IN_AIR:		agent.flags |= FLAG_FLOAT_IN_AIR; break;
				case ATOM_FLOAT_IN_WATER:	agent.flags |= FLAG_FLOAT_IN_WATER; break;
				case ATOM_ALL_ENERGY:		agent.flags |= FLAG_ALL_ENERGY; break;
				case ATOM_ALL_SENSOR:		agent.flags |= FLAG_ALL_SENSOR; break;
				case ATOM_IMMORTAL:			agent.flags |= FLAG_IMMORTAL; break;
				case ATOM_DISALLOW_PICKUP:	agent.flags |= FLAG_DISALLOW_PICKUP; break;
				case ATOM_DYNAMICS_ASLEEP:	agent.flags |= FLAG_DYNAMICS_ASLEEP; break;
				case ATOM_DYNAMICS_DISABLE:	agent.flags |= FLAG_DYNAMICS_DISABLE; break;
				default:
					printf("Unrecognized agent flag: %s.\n", goetia_atom_ToString(prop->data.atom));
			} break;
		break;
		
		// scripts
		case ATOM_INIT: 
			agent.init_script = prop->next->data.list; break;
		case ATOM_UNINIT:
			agent.uninit_script = prop->next->data.list; break;
		case ATOM_TICK:
			agent.tick_script = prop->next->data.list; break;
		case ATOM_STIMULUS:
			agent.stimulus_script = prop->next->data.list; break;
		case ATOM_CATEGORY:
			agent.category = prop->next->data.atom;
			agent.category_visual = prop->next->next->data.atom;
			agent.category_tactile = prop->next->next->next->data.atom;
		case ATOM_ACTION:
			agent.action_script = prop->next->data.list; break;
			
		
		default:
			goetia_PrintList(prop); printf(" | %i\n", prop->data.atom);
	} while ((list = list->next) && (prop = list->data.list));

	// bind default values
	Value self_id_int;
	self_id_int.type = VALUE_INT;
	self_id_int.int32 = agent.id;
	
	goetia_script_BindValue(agent.context, ATOM_AGENT_SELF, self_id_int);
	
	return agent;
}

/// Converts and agent to a list definition.
List* goetia_agent_ListFromAgent(Agent* agent) {
	// TODO: implement
}

// TODO: move this to vector.c
static bool collision(Agent* a, Agent* b) {
	if (!a || !b) return false;
	
	float distance = goetia_vec3_dst(a->position, b->position);
	
	return distance < a->collision.radius + b->collision.radius;
}

// turns agent toward target and moves toward it by dist
static void move_toward(Agent* agent, Agent* target, float dist) {
	Vector3 dir_raw = goetia_vec3_sub(target->position, agent->position);
	Vector3 dir_nrm = goetia_vec3_nrm(dir_raw);
	// change the speed to whatever
	Vector3 dir_scl = goetia_vec3_mul(dir_nrm, goetia_vec3_scl(dist));
	Vector3 new_pos = goetia_vec3_add(dir_scl, agent->position);
	
	agent->position = new_pos;
	
	Vector3 dir_flt = dir_raw;
	dir_flt.y = 0.0f;
	dir_flt = goetia_vec3_nrm(dir_flt);
	
	agent->rotation = atan2f(-dir_flt.x, -dir_flt.z);
	
	goetia_api_FireCallbackAgentMoved(agent->id, agent->position);
	goetia_api_FireCallbackAgentRotated(agent->id, agent->rotation);
	
	agent->flags &= ~FLAG_DYNAMICS_ASLEEP;
}

// this should probably be moved to action.c
/// Simulates the action that the agent is performing.
/// @note This is already being called once per tick in the world update function.
void goetia_agent_PerformAction(Agent* agent) {
	Agent* target = goetia_world_FindAgent(agent->action.target);
	
	// for some actions the agent needs to be in contact with target, so move it
	// into contact for those actions.
	switch (agent->action.type) {
		case ACTION_NONE:
			return;
		case ACTION_PUSH:
		case ACTION_PULL:
		case ACTION_DEACTIVATE:
		case ACTION_GET:
		case ACTION_EAT:
		case ACTION_HIT:
			if (!target) {
				printf("Agent target not found; cancelling.\n");
				agent->action.type = ACTION_NONE;
				return;
			}
			
			if (!collision(agent, target)) {
				move_toward(agent, target, agent->move_speed);
				return;
			}
			
			break;
		case ACTION_DROP:
		case ACTION_LOOKAT:
		case ACTION_REST:
		case ACTION_APPROACH:
		case ACTION_RETREAT:
		case ACTION_LEFT:
		case ACTION_RIGHT:
		case ACTION_UP:
		case ACTION_DOWN:
		case ACTION_FORWARD:
		case ACTION_BACKWARD:
		default: break;
	}
	
	// perform hard-coded actions
	switch (agent->action.type) {
		case ACTION_PUSH:
		case ACTION_PULL:
		case ACTION_DEACTIVATE:
		case ACTION_EAT:
		case ACTION_HIT:
	break;
		case ACTION_LOOKAT:
		case ACTION_REST:
		case ACTION_APPROACH:
		case ACTION_RETREAT:
		default:
			printf("action %s not implemented yet\n", 
			goetia_atom_ToString(goetia_action_ActionToAtom(agent->action.type)));
			agent->action.type = ACTION_NONE;
			return;
		
		case ACTION_LEFT:
			agent->rotation += agent->turn_speed;
			goetia_api_FireCallbackAgentRotated(agent->id, agent->rotation);
			return;
		case ACTION_RIGHT:
			agent->rotation -= agent->turn_speed;
			goetia_api_FireCallbackAgentRotated(agent->id, agent->rotation);
			return;
		case ACTION_UP:
		case ACTION_DOWN:
		case ACTION_FORWARD:
		case ACTION_BACKWARD:
			Vector3 mov_dir;
			
			switch (agent->action.type) {
				case ACTION_UP:
					mov_dir.x = 0.0f;
					mov_dir.y = 1.0f;
					mov_dir.z = 0.0f;
					break;
				case ACTION_DOWN:
					mov_dir.x = 0.0f;
					mov_dir.y = -1.0f;
					mov_dir.z = 0.0f;
					break;
				case ACTION_FORWARD:
					mov_dir.x = -sin(agent->rotation);
					mov_dir.y = 0.0f;
					mov_dir.z = -cos(agent->rotation);
					break;
				case ACTION_BACKWARD:
					mov_dir.x = sin(agent->rotation);
					mov_dir.y = 0.0f;
					mov_dir.z = cos(agent->rotation);
					break;
			}
			
			mov_dir = goetia_vec3_mul(mov_dir, goetia_vec3_scl(agent->move_speed));
			
			agent->position = goetia_vec3_add(agent->position, mov_dir);
			
			goetia_api_FireCallbackAgentMoved(agent->id, agent->position);
			goetia_api_FireCallbackAgentRotated(agent->id, agent->rotation);
			
			agent->flags &= ~FLAG_DYNAMICS_ASLEEP;
			
			return;
		case ACTION_GET:
			
			// if agent is holding something already, drop it
			if (agent->holding) {
				goetia_world_FindAgent(agent->holding)->held_by = 0;
				agent->holding = 0;
			}
			
			// pick up the target
			agent->holding = agent->action.target;
			target->held_by = agent->id;
			
			return;
		case ACTION_DROP:
			if (agent->holding) {
				goetia_world_FindAgent(agent->holding)->held_by = 0;
				agent->holding = 0;
			}
			return;
	}
	
	// for scriptable actions; run their code
	
	// run the action script on the target
	if (target->action_script) {
		if (agent->holding) {
			goetia_script_BindValue(target->context, ATOM_AGENT_OBJECT, goetia_value_MakeInt(agent->holding));
		} else {
			goetia_script_BindValue(target->context, ATOM_AGENT_OBJECT, goetia_value_MakeAtom(ATOM_NULL));
		}
		
		// bind _action
		goetia_script_BindValue(target->context, ATOM_AGENT_ACTION, goetia_value_MakeAtom(goetia_action_ActionToAtom(agent->action.type)));
		
		// bind _subject
		goetia_script_BindValue(target->context, ATOM_AGENT_SUBJECT, goetia_value_MakeInt(agent->id));
		
		// evaluate target action script
		goetia_script_Evaluate(target->context, target->action_script);
	}
	
	// action finished; remove action
	agent->action.type = ACTION_NONE;
	agent->action.target = AGENT_NONE;
}


/// Prints out some info about the agent.
void goetia_agent_PrintInfo(Agent* agent) {
	printf("Agent id: %i name: %s\n", agent->id, goetia_atom_ToString(agent->name));
}