// API.H - libgoetia
// Callback function call, registration function implementation.

#include <stdio.h>

#include <api.h>

static void (*agent_model_set_callback)(agent_id, atom) = NULL;
static void (*agent_model_scale)(agent_id, float) = NULL;
static void (*agent_model_color)(agent_id, float, float, float) = NULL;
static void (*agent_model_animation)(agent_id, atom, int) = NULL;
static void (*agent_removed)(agent_id) = NULL;
static void (*agent_moved)(agent_id, Vector3) = NULL;
static void (*agent_rotated)(agent_id, float) = NULL;

void goetia_api_AddCallbackAgentModelSet(void (*f)(agent_id, atom)) {
	agent_model_set_callback = f;
}

void goetia_api_AddCallbackAgentScaleSet(void (*f)(agent_id, float)) {
	agent_model_scale = f;
}

void goetia_api_AddCallbackAgentColorSet(void (*f)(agent_id, float, float, float)) {
	agent_model_color = f;
}

void goetia_api_AddCallbackAgentAnimationSet(void (*f)(agent_id, atom, int)) {
	agent_model_animation = f;
}

void goetia_api_AddCallbackAgentRemoved(void (*f)(agent_id)) {
	agent_removed = f;
}

void goetia_api_AddCallbackAgentMoved(void (*f)(agent_id, Vector3)) {
	agent_moved = f;
}

void goetia_api_AddCallbackAgentRotated(void (*f)(agent_id, float)) {
	agent_rotated = f;
}


void goetia_api_FireCallbackAgentModelSet(agent_id a, atom b) {
	if (agent_model_set_callback) agent_model_set_callback(a, b);
}

void goetia_api_FireCallbackAgentScaleSet(agent_id a, float b) {
	if (agent_model_scale) agent_model_scale(a, b);
}

void goetia_api_FireCallbackAgentColorSet(agent_id a, float b, float c, float d) {
	if (agent_model_color) agent_model_color(a, b, c, d);
}

void goetia_api_FireCallbackAgentAnimationSet(agent_id a, atom b, int c) {
	if (agent_model_animation) agent_model_animation(a, b, c);
}

void goetia_api_FireCallbackAgentRemoved(agent_id a) {
	if (agent_removed) agent_removed(a);
}

void goetia_api_FireCallbackAgentMoved(agent_id a, Vector3 b) {
	if (agent_moved) agent_moved(a, b);
}

void goetia_api_FireCallbackAgentRotated(agent_id a, float b) {
	if (agent_rotated) agent_rotated(a, b);
}
