// IPC.H - libgoetia
// TCP/IP server start/stop/update functions.

#ifndef IPC_H
#define IPC_H

#include <agent.h>

void goetia_ipc_StartServer();
void goetia_ipc_StopServer();
void goetia_ipc_ProcessServer();

#endif // IPC_H