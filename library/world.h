// WORLD.H - libgoetia
// Sector definition, sector and other world functions.

#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include <agent.h>

enum {
	SECTOR_AIR = 1,		// sector is filled with air
	SECTOR_WATER = 2,	// sector is filled with water

	SECTOR_SOLID_WALL_1 = 512,
	SECTOR_SOLID_WALL_2 = 1024,
	SECTOR_SOLID_WALL_3 = 2048,
	SECTOR_SOLID_WALL_4 = 4096,
	SECTOR_SOLID_CEILING = 8192,
	SECTOR_SOLID_FLOOR = 16384,
};

typedef struct Sector {
	float x[4], z[4], y_min[4], y_max[4];
	
	// TODO: replace fixed connection count with a dynamic array
	sector_id connections[6];
	
	sector_id id;
	atom name;
	int flags;
	
	ChemContainer* chemicals;
} Sector;

int goetia_world_LoadFromFile(const char* filename);
int goetia_world_SaveToFile(const char* filename);

Sector goetia_world_MakeSector();
Sector goetia_world_SectorFromList(List* list);

sector_id goetia_world_InsertSector(Sector sector);

Sector* goetia_world_FindSectorByPosition(Vector3 pos);
Sector* goetia_world_FindSector(sector_id sector);
Sector* goetia_world_FindSectorByName(atom name);
Sector* goetia_world_BeginSector();
Sector* goetia_world_NextSector(Sector* prev);

Vector3 goetia_world_GetSectorMidpoint(sector_id sector);

agent_id goetia_world_InjectAgent(Agent agent);
agent_id goetia_world_InjectPrototype(Agent agent);

bool goetia_world_RemoveAgent(agent_id agent);

Agent* goetia_world_FindAgent(agent_id agent);
Agent* goetia_world_FindAgentByName(atom name);
Agent* goetia_world_BeginAgent();
Agent* goetia_world_NextAgent(Agent* prev);

Agent* goetia_world_FindPrototype(atom name);
Agent* goetia_world_BeginPrototype();
Agent* goetia_world_NextPrototype(Agent* prev);

void goetia_world_Tick();

#endif // WORLD_H