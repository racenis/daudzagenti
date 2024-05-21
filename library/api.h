// API.H - libgoetia
// Callback function call, registration functions.

#ifndef API_H
#define API_H

#include <agent.h>

void goetia_api_AddCallbackAgentModelSet(void (*f)(agent_id, atom));
void goetia_api_AddCallbackAgentScaleSet(void (*f)(agent_id, float));
void goetia_api_AddCallbackAgentColorSet(void (*f)(agent_id, float, float, float));
void goetia_api_AddCallbackAgentAnimationSet(void (*f)(agent_id, atom, int));
void goetia_api_AddCallbackAgentRemoved(void (*f)(agent_id));
void goetia_api_AddCallbackAgentMoved(void (*f)(agent_id, Vector3));
void goetia_api_AddCallbackAgentRotated(void (*f)(agent_id, float));

void goetia_api_FireCallbackAgentModelSet(agent_id, atom);
void goetia_api_FireCallbackAgentScaleSet(agent_id, float);
void goetia_api_FireCallbackAgentColorSet(agent_id, float, float, float);
void goetia_api_FireCallbackAgentAnimationSet(agent_id, atom, int);
void goetia_api_FireCallbackAgentRemoved(agent_id);
void goetia_api_FireCallbackAgentMoved(agent_id, Vector3);
void goetia_api_FireCallbackAgentRotated(agent_id, float);

#endif // API_H