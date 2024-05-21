#include <stdio.h>
#include <string.h>

#include <list.h>
#include <eval.h>

#include <agent.h>
#include <world.h>

#include <api.h>
#include <ipc.h>

void print_agent_model_set_callback(agent_id a, atom b) {
	printf("agent %i set model to %i\n", a, b);
}

void print_agent_model_scale(agent_id a, float b) {
	printf("agent %i set scale to %f\n", a, b);
}

void print_agent_model_color(agent_id a, float b, float c, float d) {
	printf("agent %i set scale to %f %f %f\n", a, b, c, d);
}

void print_agent_model_animation(agent_id a, atom b, int c) {
	printf("agent %i set animation to %i with %i repets\n", a, b, c);
}

void print_agent_removed(agent_id a) {
	printf("agent %i removed\n", a);
}

void print_agent_moved(agent_id b, Vector3 a) {
	printf("agent %i moved to %f %f %f\n", b, a.x, a.y, a.z);
}

void print_agent_rotated(agent_id b, float a) {
	printf("agent %i rotated to %f\n", b, a);
}


int main () {
	goetia_api_AddCallbackAgentModelSet(print_agent_model_set_callback);
	goetia_api_AddCallbackAgentScaleSet(print_agent_model_scale);
	goetia_api_AddCallbackAgentColorSet(print_agent_model_color);
	goetia_api_AddCallbackAgentAnimationSet(print_agent_model_animation);
	goetia_api_AddCallbackAgentRemoved(print_agent_removed);
	goetia_api_AddCallbackAgentMoved(print_agent_moved);
	goetia_api_AddCallbackAgentRotated(print_agent_rotated);
	
	goetia_chemical_RegisterDefaultChemicals();
	goetia_chemical_RegisterDefaultReactions();
	
	Context* context = goetia_script_MakeContext();
	
	//List* bepis_file = goetia_ParseListFromFile("../script.lisp");
	//Value vel = goetia_script_Evaluate(context, bepis_file);
	//goetia_value_Print(vel);printf("\n");
	
	goetia_world_LoadFromFile("../worlds/gridworld.lisp");
	
	List* agent_definition = goetia_ParseListFromFile("../agents/tablet.lisp");
	//goetia_PrintList(agent_definition);
	Agent agent = goetia_agent_AgentFromList(agent_definition);
	agent_id agent_inj = goetia_world_InjectAgent(agent);
	agent.id = 42;
	goetia_world_InjectAgent(agent);
	agent.id = 43;
	agent.position.x -= 2.0f;
	goetia_world_InjectAgent(agent);
	
	goetia_world_InjectPrototype(agent);
	
	Sector sector_definition = goetia_world_MakeSector();
	sector_definition.name = goetia_atom_FromString("my-sector");
	sector_definition.x[0] = -10.0f;
	sector_definition.x[1] = 10.0f;
	sector_definition.x[2] = 10.0f;
	sector_definition.x[3] = -10.0f;
	sector_definition.z[0] = -10.0f;
	sector_definition.z[1] = -10.0f;
	sector_definition.z[2] = 10.0f;
	sector_definition.z[3] = 10.0f;
	sector_definition.y_min[0] = 0.0f;
	sector_definition.y_min[1] = 0.0f;
	sector_definition.y_min[2] = 0.0f;
	sector_definition.y_min[3] = 0.0f;
	sector_definition.y_max[0] = 10.0f;
	sector_definition.y_max[1] = 10.0f;
	sector_definition.y_max[2] = 10.0f;
	sector_definition.y_max[3] = 10.0f;
	
	sector_id sector = goetia_world_InsertSector(sector_definition);
	goetia_world_InsertSector(sector_definition);
	
	//goetia_agent_PrintInfo(&agent);
	
	for (Agent* it = goetia_world_BeginAgent(); it; it = goetia_world_NextAgent(it)) {
		printf("agent %i %s %s %s %s \n", it->id, goetia_atom_ToString(it->name),
			goetia_atom_ToString(it->category),
			goetia_atom_ToString(it->category_visual),
			goetia_atom_ToString(it->category_tactile));
	}
	
	for (Sector* it = goetia_world_BeginSector(); it; it = goetia_world_NextSector(it)) {
		//printf("sector %i %s \n", it->connection_floor, goetia_atom_ToString(it->name));
		//printf("%f %f %f %f\n", it->x[0], it->x[1], it->x[2], it->x[3]);
	}
	
	//Stimulus stim;
	//stim.position.x = 0.0f;
	//stim.position.y = 0.0f;
	//stim.position.z = 0.0f;
	//stim.type = STIMULUS_VISUAL;
	//stim.visual.type = VISUAL_ACTION;
	//stim.visual.subject = 420;
	//stim.visual.object = 69;
	//stim.visual.action = 3;
	//goetia_stimulus_Emit(stim);
	
	//goetia_chemical_PrintContainer(goetia_world_BeginAgent()->chemicals);
	
	goetia_stimulus_Process();
	goetia_world_Tick();
	goetia_world_Tick();
	goetia_stimulus_Process();
	
	//goetia_chemical_PrintContainer(goetia_world_BeginAgent()->chemicals);
	
	goetia_ipc_StartServer();
	
	while (1) {
		goetia_world_Tick();
		goetia_stimulus_Process();
		
		goetia_ipc_ProcessServer();
#if true
		char buffer[200];
		fgets(buffer, 200, stdin);
		
		if (strcmp(buffer, "exit\n") == 0) break;
		
		if (strcmp(buffer, "\n") != 0) {
		List* list = goetia_ParseList(buffer);
		Value val = goetia_script_Evaluate(context, list);
		goetia_value_Print(val);
		printf("\n");}
		
#else
		Sleep(1000);
#endif
	}
	
	goetia_ipc_StopServer();
	
	return 0;
}