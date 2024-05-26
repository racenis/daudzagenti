// WORLD.C - libgoetia
// Sector, world function implementations, large part of the simulation.

#include <world.h>
#include <stdio.h>
#include <stdlib.h>
#include <api.h>

#define MAX_SECTORS 100
#define MAX_AGENTS 100
#define MAX_PROTOTYPES 100

static Sector sectors[MAX_SECTORS];
static Agent agents[MAX_AGENTS];
static Agent prototypes[MAX_PROTOTYPES];

static int sector_count = 1; // teehee
static int agent_count = 0;
static int prototype_count = 0;

/// Loads a world definition from a file.
int goetia_world_LoadFromFile(const char* filename) {
	List* list = goetia_ParseListFromFile(filename);

	List* prop = list->data.list;

	do switch(prop->data.atom) {
		case ATOM_SECTOR:
			goetia_world_InsertSector(goetia_world_SectorFromList(prop->next->data.list));
			break;
		default:
			printf("Invalid world command: %s \n", goetia_atom_ToString(prop->data.atom));
			break;
	} while ((list = list->next) && (prop = list->data.list));
}

/// Saves the world to a file.
int goetia_world_SaveToFile(const char* filename) {
	// TODO: implement
}

/// Creates a new sector.
/// @return Sector with default values.
Sector goetia_world_MakeSector() {
	Sector sector;
	sector.id = 0;
	sector.name = ATOM_NULL;
	sector.flags = 0;
	sector.flags |= SECTOR_SOLID_WALL_1 | SECTOR_SOLID_WALL_2 | SECTOR_SOLID_WALL_3;
	sector.flags |= SECTOR_SOLID_WALL_4 | SECTOR_SOLID_CEILING | SECTOR_SOLID_FLOOR;
	sector.x[0] = sector.x[1] = sector.x[2] = sector.x[3] = 0.0f;
	sector.z[0] = sector.z[1] = sector.z[2] = sector.z[3] = 0.0f;
	sector.y_min[0] = sector.y_min[1] = sector.y_min[2] = sector.y_min[3] = 0.0f;
	sector.y_max[0] = sector.y_max[1] = sector.y_max[2] = sector.y_max[3] = 0.0f;
	sector.connections[0] = 0;
	sector.connections[1] = 0;
	sector.connections[2] = 0;
	sector.connections[3] = 0;
	sector.connections[4] = 0;
	sector.connections[5] = 0;
	sector.chemicals = goetia_chemical_MakeContainer();
	return sector;
}

static float get_float(Value data) {
	if (data.type == VALUE_FLOAT) {
		return data.float32;
	} 
	
	if (data.type == VALUE_INT) {
		return data.int32;
	}
	
	return 0;
}

/// Creates a new sector from a list.
/// @return Sector with parameters taken from list.
Sector goetia_world_SectorFromList(List* list) {
	Sector sector = goetia_world_MakeSector();
	List* prop = list->data.list;
	
	sector.flags = 0;
	
	do switch (prop->data.atom) {
		case ATOM_NAME:
			sector.name = prop->next->data.atom; break;
		case ATOM_WALL:
			if (prop->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_WALL_1;
			if (prop->next->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_WALL_2;
			if (prop->next->next->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_WALL_3;
			if (prop->next->next->next->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_WALL_4;
			if (prop->next->next->next->next->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_CEILING;
			if (prop->next->next->next->next->next->next->data.atom == ATOM_TRUE) sector.flags |= SECTOR_SOLID_FLOOR;
			break;
		case ATOM_X:
			sector.x[0] = get_float(prop->next->data);
			sector.x[1] = get_float(prop->next->next->data);
			sector.x[2] = get_float(prop->next->next->next->data);
			sector.x[3] = get_float(prop->next->next->next->next->data);
			break;
		case ATOM_Z:
			sector.z[0] = get_float(prop->next->data);
			sector.z[1] = get_float(prop->next->next->data);
			sector.z[2] = get_float(prop->next->next->next->data);
			sector.z[3] = get_float(prop->next->next->next->next->data);
			break;
		case ATOM_Y_MAX:
			sector.y_max[0] = get_float(prop->next->data);
			sector.y_max[1] = get_float(prop->next->next->data);
			sector.y_max[2] = get_float(prop->next->next->next->data);
			sector.y_max[3] = get_float(prop->next->next->next->next->data);
			break;
		case ATOM_Y_MIN:
			sector.y_min[0] = get_float(prop->next->data);
			sector.y_min[1] = get_float(prop->next->next->data);
			sector.y_min[2] = get_float(prop->next->next->next->data);
			sector.y_min[3] = get_float(prop->next->next->next->next->data);
			break;
		case ATOM_FLAGS:
			while (prop = prop->next) switch (prop->data.atom) {
				case ATOM_AIR:		sector.flags |= SECTOR_AIR;		break;
				case ATOM_WATER:	sector.flags |= SECTOR_WATER;	break;
				default:
					printf("Unrecognized sector flag: %s.\n", goetia_atom_ToString(prop->data.atom));
			} break;
		break;
		default:
			goetia_PrintList(prop); printf(" | %i\n", prop->data.atom);
	} while ((list = list->next) && (prop = list->data.list));
	
	if (sector.flags & SECTOR_AIR) sector.chemicals->properties &= ~CHEM_ALLOW_SOLUBLE;
	
	return sector;
}

// TODO: move this into vector.c
typedef struct Plane {
	float x, y, z, w;
} Plane;

static Plane get_plane(Sector* sec, int p) {
	Vector3 a, b, c;
	switch(p) {
		case 0:
		case 1:
		case 2:
		case 3:
			a.x = sec->x[p];
			a.y = sec->y_min[p];
			a.z = sec->z[p];
			b.x = sec->x[p];
			b.y = sec->y_max[p];
			b.z = sec->z[p];
			c.x = sec->x[(p+1)%4];
			c.y = sec->y_min[(p+1)%4];
			c.z = sec->z[(p+1)%4];
			break;
		case 4:
			a.x = sec->x[0];
			a.y = sec->y_max[0];
			a.z = sec->z[0];
			b.x = sec->x[1];
			b.y = sec->y_max[1];
			b.z = sec->z[1];
			c.x = sec->x[2];
			c.y = sec->y_max[2];
			c.z = sec->z[2];
			break;
		case 5:
			a.x = sec->x[0];
			a.y = sec->y_min[0];
			a.z = sec->z[0];
			b.x = sec->x[2];
			b.y = sec->y_min[2];
			b.z = sec->z[2];
			c.x = sec->x[1];
			c.y = sec->y_min[1];
			c.z = sec->z[1];
	}
	
	Vector3 ab = goetia_vec3_sub(b, a);
	Vector3 ac = goetia_vec3_sub(c, a);
	Vector3 cr = goetia_vec3_nrm(goetia_vec3_crs(ab, ac));
	
	float d = -(cr.x*a.x + cr.y*a.y + cr.z*a.z);
	
	Plane plane;
	plane.x = -cr.x;
	plane.y = -cr.y;
	plane.z = -cr.z;
	plane.w = -d;
	
	return plane;
}

static float plane_dist(Plane p, Vector3 v) {
	return p.x * v.x + p.y * v.y + p.z * v.z + p.w;
}

static Vector3 get_point(Sector* sec, int p) {
	Vector3 point;
	
	point.x = sec->x[p%4];
	point.z = sec->z[p%4];
	
	if (p >= 4) {
		point.y = sec->y_min[p%4];
	} else {
		point.y = sec->y_max[p%4];
	}
	
	return point;
}

/// Inserts a sector into the world.
/// @return ID number of the inserted sector.
sector_id goetia_world_InsertSector(Sector sector) {
	if (sector_count >= MAX_SECTORS) {
		printf("Sector insertion aborted; MAX_SECTORS exceeded.\n");
		return 0;
	}
	
	sector.id = sector_count;
	
	// this bit here will check if there is any sector adjacent to this one,
	// and if there are such sectors, they will be connected together.
	
	// TODO: completely replace this algorithm with something that is not bad
	for (int sec = 1; sec < sector_count; sec++) {
		int points_in_this = 0;
		
		for (int point = 0; point < 8; point++) {
			
			int fail = 0;
			for (int plane = 0; plane < 6; plane++) {
				Plane test_plane = get_plane(&sectors[sec], plane);
				Vector3 test_point = get_point(&sector, point);
				
				if (plane_dist(test_plane, test_point) < -0.7f) {
					fail++;
				}
			}
			
			if (!fail) {
				points_in_this++;
			}

		}
		
		if (points_in_this >= 3) {			
			// TODO: make this bit a bit nicer
			int inserted = 0;
			for (int i = 0; i < 6; i++) {
				if (sector.connections[i]) continue;
				
				sector.connections[i] = sectors[sec].id;
				inserted = 1;
				break;
			}
			
			if (!inserted) {
				printf("Failed to connect, maximum connections!\n");
			}
			
			inserted = 0;
			for (int i = 0; i < 6; i++) {
				if (sectors[sec].connections[i]) continue;
				
				sectors[sec].connections[i] = sector.id;
				inserted = 1;
				break;
			}
			
			if (!inserted) {
				printf("Failed to connect, maximum connections!\n");
			}
		}
	}
	
	sectors[sector_count] = sector;
	
	return sector_count++;
}

/// Finds an arbitrary first sector in the world.
/// @return Either a pointer to a sector or a NULL.
Sector* goetia_world_BeginSector() {
	return sector_count > 1 ? sectors+1 : NULL;
}

/// Finds the next arbitrarily ordered sector in the world.
/// @return Either a pointer to a sector or a NULL.
Sector* goetia_world_NextSector(Sector* prev) {
	return ++prev >= &sectors[sector_count] ? NULL : prev;
}

// TODO: change the parameter of this to Sector*
/// Finds the midpoint of a sector.
Vector3 goetia_world_GetSectorMidpoint(sector_id sector) {
	Vector3 mid = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

	for (int i = 0; i < 8; i++) {
		Vector3 point = get_point(&sectors[sector], i);
		mid = goetia_vec3_add(mid, point);
	}
	
	return goetia_vec3_div(mid, goetia_vec3_scl(8.0f));
}

/// Injects an agent into the world.
/// @return ID number of the injected agent.
agent_id goetia_world_InjectAgent(Agent agent) {
	if (agent_count >= MAX_AGENTS) {
		printf("Agent injection aborted; MAX_AGENTS exceeded.\n");
		return 0;
	}

	// check for id collision
	while (!agent.id || goetia_world_FindAgent(agent.id)) agent.id = rand();
	
	// bind _self variable with new id
	Value self_id_int = goetia_value_MakeInt(agent.id);
	goetia_script_BindValue(agent.context, ATOM_AGENT_SELF, self_id_int);
	
	// insert into agent list
	agents[agent_count++] = agent;
	
	// run init script
	if (agent.init_script) {
		goetia_script_Evaluate(agents[agent_count-1].context, agent.init_script);
	}
	
	goetia_api_FireCallbackAgentMoved(agent.id, agent.position);
	goetia_api_FireCallbackAgentRotated(agent.id, agent.rotation);
	
	return agent.id;
}

/// Injects a prototype in the world.
/// @return Prototype index. Not very useful.
agent_id goetia_world_InjectPrototype(Agent agent) {
	if (prototype_count >= MAX_PROTOTYPES) {
		printf("Prototype injection aborted; MAX_PROTOTYPES exceeded.\n");
		return 0;
	}
	
	prototypes[prototype_count] = agent;
	
	return prototype_count++;
}

/// Finds the agent in the world by its ID number.
/// @return Pointer to agent, if found, NULL otherwise.
Agent* goetia_world_FindAgent(agent_id agent) {
	for (int i = 0; i < agent_count; i++) {
		if (agents[i].id == agent) return &agents[i];
	}
	return NULL;
}

/// Finds the agent in the world by its name.
/// @return Pointer to agent, if found, NULL otherwise.
Agent* goetia_world_FindAgentByName(atom name) {
	for (int i = 0; i < agent_count; i++) {
		if (agents[i].name == name) return &agents[i];
	}
	return NULL;
}

/// Finds an arbitrary first agent in the world.
/// @return Either a pointer to an agent or a NULL.
Agent* goetia_world_BeginAgent() {
	return agent_count ? agents : NULL;
}

/// Finds the next arbitrarily ordered agent in the world.
/// @return Either a pointer to an agent or a NULL.
Agent* goetia_world_NextAgent(Agent* prev) {
	return ++prev >= &agents[agent_count] ? NULL : prev;
}

/// Removes an agent from the world.
/// @return True, if the agent was found and removed.
bool goetia_world_RemoveAgent(agent_id agent_id) {
	int agent_index = -1;
	
	printf("Removing agent %i...\n", agent_id);
	
	// find agent's index
	for (int i = 0; i < agent_count; i++) {
		if (agents[i].id == agent_id) {
			agent_index = i;
			break;
		}
	}
	
	// check if it was found
	if (agent_index == -1) {
		printf("Agent %i not found!\n", agent_id);
		return false;
	}
	
	// run uninit script
	Agent* agent = &agents[agent_index];
	if (agent->uninit_script) {
		printf("Running uninit...\n");
		goetia_script_Evaluate(agent->context, agent->uninit_script);
	} else {
		printf("uninit not found!\n");
	}
	
	goetia_api_FireCallbackAgentRemoved(agent->id);
	
	goetia_agent_Delete(agent);

	// move all subsequent agents up in the list
	for (int i = agent_index; i < agent_count - 1; i++) {
		agents[i] = agents[i + 1];
	}
	
	agent_count--;
	return true;
}

/// Finds the prototype in the world by its name.
/// @return Pointer to prototype, if found, NULL otherwise.
Agent* goetia_world_FindPrototype(atom name) {
	for (int i = 0; i < prototype_count; i++) {
		if (prototypes[i].name == name) return &prototypes[i];
	}
	return NULL;
}

/// Finds an arbitrary first prototype in the world.
/// @return Either a pointer to an prototype or a NULL.
Agent* goetia_world_BeginPrototype() {
	return prototype_count ? prototypes : NULL;
}

/// Finds the next arbitrarily ordered prototype in the world.
/// @return Either a pointer to an prototype or a NULL.
Agent* goetia_world_NextPrototype(Agent* prev) {
	return ++prev >= &prototypes[prototype_count] ? NULL : prev;
}

static void apply_physics(Agent* agent) {
	if (agent->flags & (FLAG_DYNAMICS_ASLEEP | FLAG_DYNAMICS_DISABLE)) return;
	
	Vector3 initial_pos = agent->position;
	sector_id sector = 0;
	
	// add gravity
	bool on_floor = false;
	for (Sector* sec = goetia_world_BeginSector(); sec; sec = goetia_world_NextSector(sec)) {
		if (!(sec->flags & SECTOR_SOLID_FLOOR)) continue;
		
		// check if in sector
		for (int p = 0; p < 6; p++) {
			Plane plane = get_plane(sec, p);
			
			if (plane_dist(plane, agent->position) < 0.0f) {
				goto next1;
			}
		}
		
		// check if in floor
		Plane plane = get_plane(sec, 5);
		float dist = plane_dist(plane, agent->position);
		
		if (dist < agent->collision.radius + 0.05f) {
			on_floor = true;
			break;
		}
next1:
	}
	
	if (!on_floor) {
		agent->velocity.y -= 0.001f;
	}
	
	agent->position = goetia_vec3_add(agent->position, agent->velocity);
	
	for (Agent* other = goetia_world_BeginAgent(); other; other = goetia_world_NextAgent(other)) {
		if (agent == other || other->flags & FLAG_DEAD) continue;
		
		// this only does spherical collisions
		// TODO: implement other collisions properly
		
		float distance = goetia_vec3_dst(agent->position, other->position);
		float min_dist = agent->collision.radius + other->collision.radius;
	
		if (distance < min_dist) {
			// push agent out of the other one!
			float penetration = min_dist - distance;
			
			Vector3 push_dir = goetia_vec3_sub(agent->position, other->position);
			push_dir = goetia_vec3_nrm(push_dir);
			push_dir = goetia_vec3_mul(push_dir, goetia_vec3_scl(penetration));
			
			agent->position = goetia_vec3_add(agent->position, push_dir);
			
			agent->velocity.y = 0.0f; // TODO: fix
			
			Stimulus collision;
			collision.type = 0;
	
			collision.type = TOUCH_AGENT;
			//collision.sector = sec->id;
			collision.sector = 0;
			collision.position = other->position;
			
			collision.subject = agent->id;
			collision.object = other->id;
			collision.action = 0;
			
			goetia_stimulus_Emit(collision);
		}
		
	}
	
	// this will be filled in if the agent wallbonks
	Stimulus wallbonk;
	wallbonk.type = 0;
	
	// iterate through all sectors.
	// in the future it would be a good idea to use a BVH or something to narrow
	// down possible collidable sectors, instead of iterating all of them and
	// checking for collisions with each one.
	for (Sector* sec = goetia_world_BeginSector(); sec; sec = goetia_world_NextSector(sec)) {
		
		// check if agent's center is inside of this sector
		for (int p = 0; p < 6; p++) {
			Plane plane = get_plane(sec, p);
			
			if (plane_dist(plane, agent->position) < 0.0f) {
				goto next; // this fails the test
			}
		}
		
		sector = sec->id;
		
		// check collisions for all walls and push the agent if necessary
		for (int p = 0; p < 6; p++) {
			switch (p) {
				case 0:
					if (!(sec->flags & SECTOR_SOLID_WALL_1)) continue; break;				
				case 1:
					if (!(sec->flags & SECTOR_SOLID_WALL_2)) continue; break;				
				case 2:
					if (!(sec->flags & SECTOR_SOLID_WALL_3)) continue; break;				
				case 3:
					if (!(sec->flags & SECTOR_SOLID_WALL_4)) continue; break;				
				case 4:
					if (!(sec->flags & SECTOR_SOLID_CEILING)) continue; break;				
				case 5:
					if (!(sec->flags & SECTOR_SOLID_FLOOR)) continue; break;
			}
			
			
			Plane plane = get_plane(sec, p);
			float dist = plane_dist(plane, agent->position);
			
			// this collision check only works for spheres
			// TODO: add checks for all other collision shapes
			if (dist < agent->collision.radius) {
				float push = agent->collision.radius - dist;
				Vector3 dir;
				dir.x = plane.x;
				dir.y = plane.y;
				dir.z = plane.z;
				dir = goetia_vec3_mul(dir, goetia_vec3_scl(push));
				agent->position = goetia_vec3_add(agent->position, dir);
				agent->velocity.y = 0.0f;
				
				// TODO: add code to differentiate different bonk stimuli.
				// like wallbonk/ceiling bonk/floor bonk
				
				wallbonk.type = TOUCH_WALLBONK;
				wallbonk.sector = sec->id;
				wallbonk.position = agent->position;
				
				wallbonk.subject = agent->id;
				wallbonk.object = 0;
				wallbonk.action = 0;
			}
		}
		
		break;
next:
	}
	
	// check if agent moved to a different sector
	if (sector && agent->sector != sector) {
		
		// fire the 'leave' stimulus, if agent was inside of a sector
		if (agent->sector) {
			Stimulus leave;
			
			leave.type = VISUAL_LEAVE;
			leave.sector = agent->sector;
			leave.position = initial_pos;
			
			leave.subject = agent->id;
			leave.object = 0;
			leave.action = 0;
			
			goetia_stimulus_Emit(leave);
		}
		
		// fire the 'appear' stimulus, if agent has entered a new sector
		if (sector) {
			Stimulus appear;
			
			appear.type = VISUAL_APPEAR;
			appear.sector = agent->sector;
			appear.position = agent->position;
			
			appear.subject = agent->id;
			appear.object = 0;
			appear.action = 0;
			
			goetia_stimulus_Emit(appear);
		}
		
		// set the new sector
		agent->sector = sector;
	}
	
	// fire callback and stimulus if the agent moved
	if (initial_pos.x != agent->position.x || 
		initial_pos.y != agent->position.y || 
		initial_pos.z != agent->position.z
	) {
		Stimulus move;
			
		move.type = VISUAL_MOVE;
		move.sector = agent->sector;
		move.position = agent->position;
		
		move.subject = agent->id;
		move.object = 0;
		move.action = 0;
		
		goetia_stimulus_Emit(move);
		
		if (wallbonk.type) {
			goetia_stimulus_Emit(wallbonk);
		}
		
		goetia_api_FireCallbackAgentMoved(agent->id, agent->position);
	} else {
		agent->flags |= FLAG_DYNAMICS_ASLEEP;
		//printf("fell asleep %i \n", agent->id);
	}
}

/// Finds the sector in which the position is in.
/// @return Pointer to a Sector, or NULL if position not in any sector.
Sector* goetia_world_FindSectorByPosition(Vector3 position) {
	for (Sector* sec = goetia_world_BeginSector(); sec; sec = goetia_world_NextSector(sec)) {
		for (int p = 0; p < 6; p++) {
			Plane plane = get_plane(sec, p);
			if (plane_dist(plane, position) < 0.0f) {
				goto next;
			}
		}
		
		return sec;
next:
	}
	return NULL;
}

/// Finds a sector by its ID number.
/// @return Pointer to a Sector, or NULL if invalid ID.
Sector* goetia_world_FindSector(sector_id sector) {
	for (int i = 0; i < sector_count; i++) {
		if (sectors[i].id == sector) return &sectors[i];
	}
	return NULL;
}

/// Finds a sector by its name.
/// @return Pointer to a Sector, or NULL if invalid name.
Sector* goetia_world_FindSectorByName(atom name) {
	for (int i = 0; i < sector_count; i++) {
		if (sectors[i].name == name) return &sectors[i];
	}
	return NULL;
}

/// Runs the simulation for a single tick.
void goetia_world_Tick() {
	agent_id expired = AGENT_NONE;
	for (Agent* it = agents; it != &agents[agent_count]; it++) {
		if (it->flags & FLAG_DEAD) {
			expired = it->id;
			continue;
		}
		
		if (it->tick_script) {
			goetia_script_Evaluate(it->context, it->tick_script);
		}
				
		if (it->chemicals) {
			goetia_chemical_UpdateContainer(it->chemicals);
		}
		
		// perform agent's action if it has enough chemical energy, or if it has
		// the free energy flag.
		// TODO: put this in a seperate function
		if (it->flags & FLAG_ALL_ENERGY) {
			goetia_agent_PerformAction(it);
		} else if (it->chemicals && it->chemicals->chems[CHEM_ATP] && it->action.type) {
			it->chemicals->chems[CHEM_ATP]--;
			it->chemicals->chems[CHEM_ADP]++;
			
			goetia_agent_PerformAction(it);
		}
		
		
		apply_physics(it);
	}
	
	if (expired) {
		printf("Agent got expired!\n");
		goetia_world_RemoveAgent(expired);
	}
	
	
	// we should add some sort of a diffusion rate parameter to the chemical
	// diffusion function
	
	// this rate should also be calculated from the size of the shared 
	// connection area
	
	for (Sector* it = sectors; it != &sectors[sector_count]; it++) {
		// diffuse chemicals
		for (int i = 0; i < 6; i++) {
			if (!it->connections[i]) continue;
			
			goetia_chemical_Diffuse(it->chemicals, sectors[it->connections[i]].chemicals);
		}
	}
}