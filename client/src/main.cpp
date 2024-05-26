#include <framework/core.h>
#include <framework/logging.h>
#include <framework/ui.h>
#include <framework/gui.h>
#include <framework/async.h>
#include <framework/event.h>
#include <framework/message.h>
#include <framework/system.h>
#include <framework/worldcell.h>
#include <framework/language.h>
#include <framework/file.h>
#include <audio/audio.h>
#include <render/render.h>
#include <render/material.h>
#include <render/api.h>
#include <physics/physics.h>
#include <entities/player.h>
#include <entities/staticworldobject.h>
#include <entities/crate.h>
#include <components/player.h>
#include <components/controller.h>
#include <components/render.h>
#include <components/light.h>
#include <extensions/menu/menu.h>

#include <map>
#include <set>

extern "C" {
#include <stdio.h>
#include <string.h>
#include <list.h>
#include <eval.h>
#include <api.h>
#include <agent.h>
#include <world.h>
#include <ipc.h>
}

using namespace tram;

static std::set<agent_id> tracked_agents;

struct AgentDisplay {
	tram::Component<tram::RenderComponent> model;
	tram::Component<tram::AnimationComponent> armature;
	std::vector<vec3> position_history;
};

std::map<agent_id, AgentDisplay> all_agents;

extern "C" void print_agent_model_set_callback(agent_id id, atom model) {
	if (all_agents[id].model) {
		all_agents[id].model->SetModel(goetia_atom_ToString(model));
	} else {
		all_agents[id].model.make();
		//all_agents[id].model->SetWorldParameters(true);
		all_agents[id].model->SetModel(goetia_atom_ToString(model));
		all_agents[id].model->Init();
	}
}

extern "C" void print_agent_model_scale(agent_id agent, float scale) {
	if (all_agents[agent].model) {
		all_agents[agent].model->SetScale({scale, scale, scale});
	}
}

extern "C" void print_agent_model_color(agent_id agent, float r, float g, float b) {
	if (all_agents[agent].model) {
		all_agents[agent].model->SetColor({r, g, b});
	}
}

extern "C" void print_agent_model_animation(agent_id id, atom animation, int repeats) {
	if (all_agents[id].armature) {
		if (!repeats) {
			all_agents[id].armature->StopAnimation(goetia_atom_ToString(animation));
		} else {
			all_agents[id].armature->PlayAnimation(goetia_atom_ToString(animation), repeats, 1.0f, 1.0f);
		}
	} else if (!all_agents[id].model) {
		std::cout << "SET MODEL BEFORE SETTING ANIMATION" << std::endl;
	} else {
		all_agents[id].armature.make();
		all_agents[id].armature->SetModel(all_agents[id].model->GetModel());
		all_agents[id].armature->Init();
		
		all_agents[id].model->SetArmature(all_agents[id].armature);
	}	
}

extern "C" void print_agent_removed(agent_id a) {
	if (auto it = all_agents.find(a); it != all_agents.end()) {
		all_agents.erase(it);
	}
}

extern "C" void print_agent_moved(agent_id agent, Vector3 pos) {
	if (all_agents[agent].model) {
		all_agents[agent].model->SetLocation({pos.x, pos.y, pos.z});
	}
	
	if (tracked_agents.contains(agent)) {
		all_agents[agent].position_history.push_back({pos.x, pos.y, pos.z});
	}
}

extern "C" void print_agent_rotated(agent_id agent, float rotation) {
	if (all_agents[agent].model) {
		all_agents[agent].model->SetRotation(tram::vec3(0.0f, rotation, 0.0f));
	}
}

bool static paused = true;
bool static draw_sector = false;
bool static draw_sector_wall = false;
bool static draw_sector_connection = false;
bool static draw_agent = false;
bool static draw_agent_paths = false;

static chem display_chemical = CHEM_NONE;

enum ViewType {
	VIEW_ABOVE,
	VIEW_SIDE
};

static ViewType view_type = VIEW_ABOVE;

static vec3 gradient_color(float x) {
	if (x < 0.5f) {
		return glm::mix(Render::COLOR_RED, Render::COLOR_YELLOW, x * 2.0f);
	} else {
		return glm::mix(Render::COLOR_YELLOW, Render::COLOR_GREEN, (x - 0.5f) * 2.0f);
	}
}

int main(int argc, const char** argv) {
	goetia_api_AddCallbackAgentModelSet(print_agent_model_set_callback);
	goetia_api_AddCallbackAgentScaleSet(print_agent_model_scale);
	goetia_api_AddCallbackAgentColorSet(print_agent_model_color);
	goetia_api_AddCallbackAgentAnimationSet(print_agent_model_animation);
	goetia_api_AddCallbackAgentRemoved(print_agent_removed);
	goetia_api_AddCallbackAgentMoved(print_agent_moved);
	goetia_api_AddCallbackAgentRotated(print_agent_rotated);
	
	goetia_chemical_RegisterDefaultChemicals();
	goetia_chemical_RegisterDefaultReactions();
	
	// load in the configuration
	std::string setup_filename = "client.setup";
	std::string background_name = "none";
	std::set<std::string> tracked_agent_names;
	
	for (int i = 1; i < argc; i++) {
		std::string argument = argv[i];
		
		if (argument == "-setup") {
			setup_filename = argv[++i];
			
		} else if (argument == "-background") {
			background_name = argv[++i];
			
		} else if (argument == "-view") {
			std::string type = argv[++i];
			
			if (type == "above") {
				view_type = VIEW_ABOVE;
			} else if (type == "side") {
				view_type = VIEW_SIDE;
			} else {
				std::cout << "Unrecognized view type: " << type << std::endl;
			}
			
		} else if (argument == "-chemical") {
			std::string chemical_name = argv[++i];
			display_chemical = goetia_chemical_AtomToChemical(goetia_atom_FromString(chemical_name.c_str()));
			
			if (display_chemical == CHEM_NONE) {
				std::cout << "Unrecognized chemical: " << chemical_name << std::endl;
			}
			
		} else if (argument == "-track") {
			tracked_agent_names.insert(argv[++i]);
		} else {
			if (argument != "--help") {
				std::cout << "Unrecognized argument: " << argument << "\n" << std::endl;
			}
			
			std::cout << "Commandline options:" << std::endl;
			std::cout << "\t-setup filename.setup" << std::endl;
			std::cout << "\t-background background-model-name" << std::endl;
			std::cout << "\t-view [above/side]" << std::endl;
			std::cout << "\t-chemical chemical-name" << std::endl;
			std::cout << "\t-track agent-name" << std::endl;
			
			return 0;
		}
		
	}
	
	SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);
	
	File setup_file(setup_filename.c_str(), MODE_READ | MODE_PAUSE_LINE);
	
	if (!setup_file.is_open()) {
		std::cout << "Couldn't open setup file: " << setup_filename << std::endl;
		return 1;
	}
	
	
	// initialize tramway sdk
	tram::Core::Init();
	tram::UI::Init();
	tram::Render::Init();
	tram::Physics::Init();
	tram::Async::Init();
	tram::Audio::Init();
	tram::GUI::Init();
	
    tram::Ext::Menu::Init();
    
	tram::Render::Material::LoadMaterialInfo("material");
	tram::Language::Load("english");
	
	tram::Render::API::SetScreenClear({0.0f, 0.0f, 0.0f}, true);
	tram::Render::SetSunDirection(glm::normalize(glm::vec3(0.0f, 1.0f, 0.5f)));
	tram::Render::SetSunColor(glm::vec3(250.0f, 214.0f, 165.0f) / 256.0f * 0.8f);
	tram::Render::SetAmbientColor((glm::vec3(250.0f, 214.0f, 165.0f) / 256.0f * 0.8f) * 0.7f);
	
	switch(view_type) {
		case VIEW_ABOVE:
			tram::Render::SetCameraPosition({4.5f, 10.0f, 4.5f});	
			tram::Render::SetCameraRotation(tram::vec3(-1.57f, 0.0f, 0.0f));
		break;
		case VIEW_SIDE:
			tram::Render::SetCameraPosition({4.5f, 10.0f, 4.5f});	
			tram::Render::SetCameraRotation(tram::vec3(0.0f, 0.0f, 0.0f));
		break;
	}
		
	
	Component<RenderComponent> background;
	if (background_name != "none") {
		background.make();
		background->SetModel(background_name);
		background->SetLightmap("fullbright");
		background->Init();
	}
	
	UI::BindKeyboardKey(UI::KEY_SPACE, [](){ paused = !paused; });
	UI::BindKeyboardKey(UI::KEY_0, [](){ paused = !paused; });
	UI::BindKeyboardKey(UI::KEY_1, [](){ draw_sector = !draw_sector; });
	UI::BindKeyboardKey(UI::KEY_2, [](){ draw_sector_wall = !draw_sector_wall; });
	UI::BindKeyboardKey(UI::KEY_3, [](){ draw_sector_connection = !draw_sector_connection; });
	UI::BindKeyboardKey(UI::KEY_4, [](){ draw_agent = !draw_agent; });
	UI::BindKeyboardKey(UI::KEY_5, [](){ draw_agent_paths = !draw_agent_paths; });
	
	UI::SetWindowTitle("Daudzaģentu sistēmas simulācijas vizualizācija");
	
	// parse config file
	while (setup_file.is_continue()) {
		name_t command = setup_file.read_name();
	
		if (command == "world") {
			goetia_world_LoadFromFile(std::string(setup_file.read_line()).c_str());
		} else if (command == "agent") {
			List* agent_definition = goetia_ParseListFromFile(std::string(setup_file.read_line()).c_str());
			Agent agent = goetia_agent_AgentFromList(agent_definition);
			agent_id agent_id = goetia_world_InjectAgent(agent);
			
			if (tracked_agent_names.contains(goetia_atom_ToString(agent.name))) {
				tracked_agents.insert(agent_id);
			}
		} else if (command == "prototype") {
			List* agent_definition = goetia_ParseListFromFile(std::string(setup_file.read_line()).c_str());
			Agent agent = goetia_agent_AgentFromList(agent_definition);
			goetia_world_InjectPrototype(agent);
		} else if (command == "light") {
			vec3 position = {setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()};
			vec3 color = {setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()};
			
			LightComponent* light = PoolProxy<LightComponent>::New();
			light->SetColor(color);
			light->SetLocation(position);
			light->Init();
		} else if (command == "ambient") {
			Render::SetSunDirection(glm::normalize(vec3({setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()})));
			Render::SetSunColor({setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()});
			Render::SetAmbientColor({setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()});
		} else if (command == "background") {
			Render::API::SetScreenClear({setup_file.read_float32(), setup_file.read_float32(), setup_file.read_float32()}, true);
		} else {
			std::cout << "Unrecognized command: " << command << " " << setup_file.read_line() << std::endl;
		}

		setup_file.skip_linebreak();	
	}
	
	goetia_ipc_StartServer();
	
	while (!tram::EXIT) {
		tram::Core::Update();
		tram::UI::Update();
		tram::Physics::Update();
		
		goetia_ipc_ProcessServer();
		
		// UPDATE SIMULATION
		
		static int tick = 0;
		
		if (!paused) {
			goetia_world_Tick();
			goetia_stimulus_Process();
		}
		
		// RENDER WIREFRAMES
		
		if (draw_agent) for (Agent* it = goetia_world_BeginAgent(); it; it = goetia_world_NextAgent(it)) {
			tram::vec3 color = glm::normalize(tram::vec3(
				it->id & 255, (it->id >> 8) & 255, (it->id >> 16) & 255
			));
			tram::Render::AddSphere({it->position.x, it->position.y, it->position.z}, it->collision.radius, color);
		}
		
		if (draw_sector) for (Sector* it = goetia_world_BeginSector(); it; it = goetia_world_NextSector(it)) {
			
			// render white boxes on the edges of sectors
			tram::vec3 b0 = {it->x[0], it->y_min[0], it->z[0]};
			tram::vec3 b1 = {it->x[1], it->y_min[1], it->z[1]};
			tram::vec3 b2 = {it->x[2], it->y_min[2], it->z[2]};
			tram::vec3 b3 = {it->x[3], it->y_min[3], it->z[3]};
			tram::vec3 t0 = {it->x[0], it->y_max[0], it->z[0]};
			tram::vec3 t1 = {it->x[1], it->y_max[1], it->z[1]};
			tram::vec3 t2 = {it->x[2], it->y_max[2], it->z[2]};
			tram::vec3 t3 = {it->x[3], it->y_max[3], it->z[3]};
			
			tram::Render::AddLine(b0, b1, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b1, b2, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b2, b3, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b3, b0, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(t0, t1, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(t1, t2, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(t2, t3, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(t3, t0, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b0, t0, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b1, t1, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b2, t2, tram::Render::COLOR_WHITE);
			tram::Render::AddLine(b3, t3, tram::Render::COLOR_WHITE);
			
			// render connections between sectors
			if (draw_sector_connection) for (int i = 0; i < 6; i++) {
				if (!it->connections[i]) continue;
				
				Vector3 g_mid1 = goetia_world_GetSectorMidpoint(it->id);
				Vector3 g_mid2 = goetia_world_GetSectorMidpoint(it->connections[i]);
				
				tram::vec3 mid1 = {g_mid1.x, g_mid1.y, g_mid1.z};
				tram::vec3 mid2 = {g_mid2.x, g_mid2.y, g_mid2.z};
				
				mid1.y += 0.1f;
				mid2.y += 0.1f;
				
				tram::Render::AddLine(mid1, mid2, tram::Render::COLOR_PINK);
			}
			
			// render mid point wall thing
			if (draw_sector_wall) {
				tram::vec3 mid0 = glm::mix(glm::mix(b0, b1, 0.5f), glm::mix(t0, t1, 0.5f), 0.5f);
				tram::vec3 mid1 = glm::mix(glm::mix(b1, b2, 0.5f), glm::mix(t1, t2, 0.5f), 0.5f);
				tram::vec3 mid2 = glm::mix(glm::mix(b2, b3, 0.5f), glm::mix(t2, t3, 0.5f), 0.5f);
				tram::vec3 mid3 = glm::mix(glm::mix(b3, b0, 0.5f), glm::mix(t3, t0, 0.5f), 0.5f);
				
				tram::vec3 mid4 = glm::mix(glm::mix(t0, t1, 0.5f), glm::mix(t2, t3, 0.5f), 0.5f);
				tram::vec3 mid5 = glm::mix(glm::mix(b0, b1, 0.5f), glm::mix(b2, b3, 0.5f), 0.5f);
				
				const tram::vec3& ok = tram::Render::COLOR_GREEN;
				const tram::vec3& no = tram::Render::COLOR_RED;
				
				tram::vec3 can0 = it->flags & SECTOR_SOLID_WALL_1 ? no : ok;
				tram::vec3 can1 = it->flags & SECTOR_SOLID_WALL_2 ? no : ok;
				tram::vec3 can2 = it->flags & SECTOR_SOLID_WALL_3 ? no : ok;
				tram::vec3 can3 = it->flags & SECTOR_SOLID_WALL_4 ? no : ok;
				
				tram::vec3 can4 = it->flags & SECTOR_SOLID_CEILING ? no : ok;
				tram::vec3 can5 = it->flags & SECTOR_SOLID_FLOOR ? no : ok;
				
				tram::Render::AddLine(mid0, glm::mix(mid0, mid2, 0.2f), can0);
				tram::Render::AddLine(mid1, glm::mix(mid1, mid3, 0.2f), can1);
				tram::Render::AddLine(mid2, glm::mix(mid2, mid0, 0.2f), can2);
				tram::Render::AddLine(mid3, glm::mix(mid3, mid1, 0.2f), can3);
				
				tram::Render::AddLine(mid4, glm::mix(mid4, mid5, 0.2f), can4);
				tram::Render::AddLine(mid5, glm::mix(mid5, mid4, 0.2f), can5);
			}
		}
		
		static int past_min = 0;
		static int past_max = 0;
		
		int this_min = past_min;
		int this_max = past_max;
		
		float difference = past_max - past_min;
		
		past_min = 99999;
		past_max = 0;
		
		if (display_chemical) for (Sector* it = goetia_world_BeginSector(); it; it = goetia_world_NextSector(it)) {
			
			vec3 b0 = {it->x[0] + 0.1f, it->y_min[0], it->z[0] + 0.1f};
			vec3 b1 = {it->x[1] - 0.1f, it->y_min[1], it->z[1] + 0.1f};
			vec3 b2 = {it->x[2] - 0.1f, it->y_min[2], it->z[2] - 0.1f};
			vec3 b3 = {it->x[3] + 0.1f, it->y_min[3], it->z[3] - 0.1f};
			
			
			int concentration = it->chemicals->chems[display_chemical];
			
			if (concentration < past_min) past_min = concentration;
			if (concentration > past_max) past_max = concentration;
			
			float relative = ((float)(concentration - past_min)) / difference;
			
			Render::AddLine(b0, b1, gradient_color(relative));
			Render::AddLine(b1, b2, gradient_color(relative));
			Render::AddLine(b2, b3, gradient_color(relative));
			Render::AddLine(b3, b0, gradient_color(relative));
		}
		
		if (draw_agent_paths) {
			for (auto agent : tracked_agents) {
				const auto& paths = all_agents[agent].position_history;
				
				if (paths.size() < 2) continue;
				
				float col = 0.0f;
				float inc = 1.0f / (float)paths.size();
				
				for (size_t i = 0; i < paths.size() - 1; i++) {
					col += inc;
					Render::AddLine(paths[i], paths[i+1], gradient_color(col));
				}
			}
		}
		
		// MOVE CAMERA AND STUFF
		
		if (UI::GetInputState() == UI::STATE_DEFAULT) {
			vec3 move = {0.0f, 0.0f, 0.0f};
			
			switch(view_type) {
			case VIEW_ABOVE:
				if (UI::PollKeyboardKey(UI::KEY_A)) move.x -= 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_D)) move.x += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_W)) move.z -= 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_S)) move.z += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_Q)) move.y += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_E)) move.y -= 0.15f;
			break;
			case VIEW_SIDE:
				if (UI::PollKeyboardKey(UI::KEY_A)) move.x -= 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_D)) move.x += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_W)) move.y += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_S)) move.y -= 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_Q)) move.z += 0.15f;
				if (UI::PollKeyboardKey(UI::KEY_E)) move.z -= 0.15f;
			break;
			}
			
			if (tram::UI::PollKeyboardKey(tram::UI::KEY_LEFT_SHIFT)) move *= 5.0f;
			
			if (move != vec3(0.0f, 0.0f, 0.0f)) {
				auto pos = Render::GetCameraPosition();
				pos += move;
				Render::SetCameraPosition(pos);
			}
		}
		
		
		//Render::AddLineMarker(vec3(0.0f, 0.0f, 0.0f), Render::COLOR_YELLOW);
		
		
		
		tram::GUI::Begin();
		tram::Ext::Menu::DebugMenu();
		tram::Ext::Menu::EscapeMenu();

		tram::GUI::End();
		tram::GUI::Update();
		
		tram::Async::ResourceLoader2ndStage();
		tram::Async::FinishResource();
		
		tram::Event::Dispatch();
		tram::Message::Dispatch();

		tram::ControllerComponent::Update();
		tram::AnimationComponent::Update();

		tram::Render::Render();
		tram::UI::EndFrame();
	}
	
	tram::Async::Yeet();
	//tram::Audio::Uninit();
	//tram::UI::Uninit();
}