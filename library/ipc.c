// IPC.H - libgoetia
// TCP/IP server implementation.

#include <stdio.h>

#include <ipc.h>

#include <api.h>
#include <world.h>

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <winsock2.h>
	
	#define ERROR_VALUE WSAGetLastError()
	#define close(X) closesocket(X)
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h> 
	
	#define ERROR_VALUE errno
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
#endif

static struct sockaddr_in server_info;
static int server_socket = 0;

static int connections[10];
static int connection_count = 0;

static bool set_nonblocking(int socket) {
#ifdef _WIN32
	unsigned long non_zero = 1;
	return (ioctlsocket(socket, FIONBIO, &non_zero) == 0);
#else
	int flags = fcntl(socket, F_GETFL, 0);
	if (flags == -1) return false;
	flags = flags | O_NONBLOCK;
	return (fcntl(socket, F_SETFL, flags) == 0);
#endif
}

/// Starts the TCP/IP server.
void goetia_ipc_StartServer() {
	int error;
#ifdef _WIN32
	WSADATA wsa_data;
	error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (error != 0) {
		printf("Winsock startup failed: %i\n", error);
		abort();
	}
#endif

	memset(&server_info, 0, sizeof(server_info));
	
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(4444); // change port to whatever
	server_info.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		printf("Error opening server socket: %i\n", ERROR_VALUE);
		abort();
	}
	
	error = bind(server_socket, (struct sockaddr*)&server_info, sizeof(server_info));
    if (error == SOCKET_ERROR) {
        printf("Error binding server socket: %i\n", ERROR_VALUE);
        abort();
    }
	
	if (!set_nonblocking(server_socket)) {
		printf("error setting nonblcoking socket\n");
		abort();
	}
	
	error = listen(server_socket, 1);
	if (error == SOCKET_ERROR) {
        printf("Error listening server socket: %i\n", ERROR_VALUE);
        abort();
    }
}

/// Stops the TCP/IP server.
void goetia_ipc_StopServer() {
	int error = close(server_socket);
    if (error == SOCKET_ERROR) {
        printf("Server socket closing failed: %i\n", ERROR_VALUE);
        abort();
    }

#ifdef _WIN32
	WSACleanup();
#endif
}

static void send_message(int socket, const char* msg) {
	int msg_len = strlen(msg) + 1;
	int bytes_sent = send(socket, msg, msg_len, 0);
	if (bytes_sent == -1) {
		printf("Error sneidng socket data: %i\n", ERROR_VALUE);
		abort();
	}
	if (bytes_sent != msg_len) {
		printf("Expected len: %i sent len: %i ERROR ERROR!!!\n", msg_len, bytes_sent);
	}
}

/// Processes the TCP/IP server incoming requests.
void goetia_ipc_ProcessServer() {
	int new_connection = accept(server_socket, NULL, NULL);
	
	if (new_connection == INVALID_SOCKET) {
		//printf("No new connections...\n");
	} else {
		printf("New TCP/IP connection!\n");
		if (!set_nonblocking(new_connection)) {
			printf("Couldn't set nonblocking on new connection!\n");
			abort();
		}
		
		send_message(new_connection, "SKIP Turnip growing simulator v0.0.1 TCP/IP IPC interface\r\nSKIP Input command 'HELP' to get help\r\nEND\r\n");
		
		connections[connection_count++] = new_connection;
	}
	
	
	for (int i = 0; i < connection_count; i++) {
		int socket = connections[i];
		
		char msg[100];
		int bytes_received = recv(socket, msg, 100, 0);
		
		if (bytes_received == -1) {
			if (ERROR_VALUE != WSAEWOULDBLOCK) {
				printf("Connection terminated!\n");
				
				for (int k = i; k < connection_count - 1; k++) {
					connections[k] = connections[k+1];
				}
				
				connection_count--;
				break;
			}
			
			continue;
		}
		
		if (bytes_received == 0) {
			printf("EMPTY MESSAGE! NOT GOOD\n");
			continue;
		}
		
		//for (int i = 0; i < bytes_received; i++) printf("%c", msg[i]);
		//printf("\n");
		
		msg[bytes_received] = '\0';
		
		const char* delims = " \t\r\n";
		char* cmd = strtok(msg, delims);
		
		//printf("bytes: %i received: '%s' \ncommand: '%s'\n", bytes_received, msg, cmd);
		
		if (strcmp(cmd, "HELP") == 0) {
			//printf("helpin\n");
			send_message(socket, "SKIP 'GET AGENTS'\tto get list of all agents.\r\n");
			send_message(socket, "SKIP 'GET SECTORS'\tto get list of all setors.\r\n");
			send_message(socket, "SKIP 'GET PROTOTYPES'\tto get list of all prototpyes.\r\n");
			
			send_message(socket, "SKIP 'GET AGENT [id]'\tto get info about agent with the [id].\r\n");
			send_message(socket, "SKIP 'GET SECTOR [id]'\tto get info about secots with the [id].\r\n");
			
			send_message(socket, "SKIP 'SET AGENT [id] [property] [value]'\tset agent [id]'s [property] to [value].\r\n");
			send_message(socket, "END\r\n");
		} else if (strcmp(cmd, "GET") == 0) {
			char* type = strtok(NULL, delims);
			
			if (strcmp(type, "AGENTS") == 0) {
				for (Agent* it = goetia_world_BeginAgent(); it; it = goetia_world_NextAgent(it)) {
					char msg[100];
					sprintf(msg, "AGENT %i %s %s %s %s\r\n",
						it->id,
						goetia_atom_ToString(it->name),
						goetia_atom_ToString(it->category),
						"idk",
						"idk"
					);
					send_message(socket, msg);
				}
				
				send_message(socket, "END\r\n");
			} else if (strcmp(type, "SECTORS") == 0) {
				for (Sector* it = goetia_world_BeginSector(); it; it = goetia_world_NextSector(it)) {
					char msg[100];
					sprintf(msg, "SECTOR %i %s %i\r\n",
						it->id,
						goetia_atom_ToString(it->name),
						it->flags
					);
					send_message(socket, msg);
				}
				
				send_message(socket, "END\r\n");
			} else if (strcmp(type, "PROTOTYPES") == 0) {
				for (Agent* it = goetia_world_BeginPrototype(); it; it = goetia_world_NextPrototype(it)) {
					char msg[100];
					sprintf(msg, "PROTOTYPE %s %s \"%s\" %s\r\n",
						goetia_atom_ToString(it->name),
						goetia_atom_ToString(it->category),
						it->description,
						"idk"
					);
					send_message(socket, msg);
				}
				
				send_message(socket, "END\r\n");
			} else if (strcmp(type, "CHEMICALS") == 0) {
				for (chem c = CHEM_NONE; c < CHEM_LAST; c++) {
					char msg[100];
					sprintf(msg, "CHEMICAL %s %i\r\n",
						goetia_atom_ToString(goetia_chemical_ChemicalToAtom(c)),
						goetia_chemical_Property(c)
					);
					send_message(socket, msg);
				}
				
				send_message(socket, "END\r\n");
			}else if (strcmp(type, "CHEMICAL") == 0) {
				char* type = strtok(NULL, delims);
				char* id = strtok(NULL, delims);
				ChemContainer* chem_bag = NULL;
				
				if (strcmp(type, "AGENT") == 0) {
					Agent* agent = goetia_world_FindAgent(atoi(id));
					if (agent) {
						chem_bag = agent->chemicals;
					}
				} else if (strcmp(type, "SECTOR") == 0) {
					Sector* sector = goetia_world_FindSector(atoi(id));
					if (sector) {
						chem_bag = sector->chemicals;
					}
				}
				
				if (!chem_bag) {
					send_message(socket, "ERROR Chemical container not found!\r\nEND\r\n");
				} else {
					for (int i = 0; i < CHEM_LAST; i++) {
						//if (!chem_bag->chems[i]) continue;
						char msg[100];
						sprintf(msg, "CHEMICAL %s %i\r\n", 
							goetia_atom_ToString(goetia_chemical_ChemicalToAtom(i)),
							chem_bag->chems[i]
						);
						send_message(socket, msg);
					}
					send_message(socket, "END\r\n");
				}
			} else if (strcmp(type, "AGENT") == 0) {
				char* id = strtok(NULL, delims);
				Agent* agent = goetia_world_FindAgent(atoi(id));

				if (!agent) {
					send_message(socket, "ERROR Agent not found!\r\nEND\r\n");
				} else {
					char msg[100];
					sprintf(msg, "PROPERTY id %i\r\n", agent->id); send_message(socket, msg);
					sprintf(msg, "PROPERTY name %s\r\n", goetia_atom_ToString(agent->name)); send_message(socket, msg);
					sprintf(msg, "PROPERTY category-1 %s\r\n", goetia_atom_ToString(agent->category)); send_message(socket, msg);
					sprintf(msg, "PROPERTY category-2 %s\r\n", goetia_atom_ToString(agent->category_visual)); send_message(socket, msg);
					sprintf(msg, "PROPERTY category-3 %s\r\n", goetia_atom_ToString(agent->category_tactile)); send_message(socket, msg);
					sprintf(msg, "PROPERTY move-speed %f\r\n", agent->move_speed); send_message(socket, msg);
					sprintf(msg, "PROPERTY turn-speed %f\r\n", agent->turn_speed); send_message(socket, msg);
					sprintf(msg, "PROPERTY accl-speed %f\r\n", agent->accl_speed); send_message(socket, msg);
					sprintf(msg, "PROPERTY position-x %f\r\n", agent->position.x); send_message(socket, msg);
					sprintf(msg, "PROPERTY position-y %f\r\n", agent->position.y); send_message(socket, msg);
					sprintf(msg, "PROPERTY position-z %f\r\n", agent->position.z); send_message(socket, msg);
					sprintf(msg, "PROPERTY collision-shape %f\r\n", goetia_atom_ToString(goetia_shape_ToAtom(agent->collision.type))); send_message(socket, msg);
					sprintf(msg, "PROPERTY collision-radius %f\r\n", agent->collision.radius); send_message(socket, msg);
					sprintf(msg, "PROPERTY collision-height %f\r\n", agent->collision.height); send_message(socket, msg);
					sprintf(msg, "PROPERTY rotation %f\r\n", agent->category); send_message(socket, msg);
					sprintf(msg, "PROPERTY mass %f\r\n", agent->mass); send_message(socket, msg);
					sprintf(msg, "PROPERTY action %s\r\n", goetia_atom_ToString(goetia_action_ActionToAtom(agent->action.type))); send_message(socket, msg);
					sprintf(msg, "PROPERTY action-target %i\r\n", agent->action.target); send_message(socket, msg);
					send_message(socket, "END\r\n");
				}
			} else if (strcmp(type, "SECTOR") == 0) {
				char* id = strtok(NULL, delims);
				Sector* sector = goetia_world_FindSector(atoi(id));
				
				if (!sector) {
					send_message(socket, "ERROR Agent not found!\r\nEND\r\n");
				} else {
					char msg[100];
				}
			} else {
				char msg[100];
				sprintf(msg, "ERROR Unrecognized command 'GET %s'\r\nEND\r\n", type);
				send_message(socket, msg);
			}
			
			
		} else if (strcmp(cmd, "SET") == 0) {
				char* type = strtok(NULL, delims);
				
				if (strcmp(type, "AGENT") == 0) {
					char* id = strtok(NULL, delims);
					Agent* agent = goetia_world_FindAgent(atoi(id));

					if (!agent) {
						send_message(socket, "ERROR Agent not found!\r\nEND\r\n");
					} else {
						char* param = strtok(NULL, delims);
						char* value = strtok(NULL, delims);
						
						
						//printf("recieved: %s : %s\n", param, value);

						if (strcmp(param, "move-speed") == 0) agent->move_speed = atof(value); else
						if (strcmp(param, "turn-speed") == 0) agent->turn_speed = atof(value); else
						if (strcmp(param, "accl-speed") == 0) agent->accl_speed = atof(value); else
						if (strcmp(param, "position-x") == 0) {agent->position.x = atof(value); goetia_api_FireCallbackAgentMoved(agent->id, agent->position);} else
						if (strcmp(param, "position-y") == 0) {agent->position.y = atof(value); goetia_api_FireCallbackAgentMoved(agent->id, agent->position);} else
						if (strcmp(param, "position-z") == 0) {agent->position.z = atof(value); goetia_api_FireCallbackAgentMoved(agent->id, agent->position);} else
						if (strcmp(param, "rotation") == 0) {agent->rotation = atof(value); goetia_api_FireCallbackAgentRotated(agent->id, agent->rotation);} else
						if (strcmp(param, "mass") == 0) agent->mass = atof(value);
						else {
							send_message(socket, "ERROR Not implemented.\r\nEND\r\n");
							return;
						}
						
						
						
						send_message(socket, "OK\r\nEND\r\n");
					}
				}
				
				
		} else if (strcmp(cmd, "KILL") == 0) {
			char* id = strtok(NULL, delims);
			//printf("killing... %s\n", id);
			
			Agent* agent = goetia_world_FindAgent(atoi(id));
			
			if (!agent) {
				sprintf(msg, "ERROR Agent '%s' not found!\r\nEND\r\n", id);
				send_message(socket, msg);
			} else {
				agent->flags |= FLAG_DEAD;
				send_message(socket, "OK\r\nEND\r\n");
			}
		} else if (strcmp(cmd, "INSTANTIATE") == 0) {
			char* type = strtok(NULL, delims);
			char* name = strtok(NULL, delims);
			
			//printf("instanting type '%s' name '%s'\n", type, name);
			
			Agent* agent = goetia_world_FindPrototype(goetia_atom_FromString(name));		
			
			if (!agent) {
				sprintf(msg, "ERROR Prototype '%s' not found!\r\nEND\r\n", name);
				send_message(socket, msg);
			} else {
				// TODO: implement agent copying
				Agent new_agent = *agent;
				
				if (strcmp(type, "AT") == 0) {
					char* x = strtok(NULL, delims);
					char* y = strtok(NULL, delims);
					char* z = strtok(NULL, delims);
					
					new_agent.position.x = atof(x);
					new_agent.position.y = atof(y);
					new_agent.position.z = atof(z);
				}
				
				goetia_world_InjectAgent(new_agent);
				
				send_message(socket, "OK\r\nEND\r\n");
			}
		} else if (strcmp(cmd, "EXIT") == 0) {
			//printf("exitin\n");
			send_message(socket, "K THX BYE\r\n");
			close(socket);
			
			for (int k = i; k < connection_count - 1; k++) {
				connections[k] = connections[k+1];
			}
			
			connection_count--;
			break;
		} else {
			char msg[100];
			sprintf(msg, "ERROR Unrecognized command '%s'\r\nEND\r\n", cmd);
			send_message(socket, msg);
		}
		
	}
	
	
}