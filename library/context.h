// CONTEXT.C - libgoetia
// Script execution context definition, functions.

#ifndef CONTEXT_H
#define CONTEXT_H

#include <list.h>

typedef struct KeyValue {
	atom name;
	Value value;
} KeyValue;

enum {
	LAYER_CLASS,
	LAYER_SCALAR,
	LAYER_VECTOR,
	LAYER_INPUT,
	LAYER_OUTPUT,
	LAYER_HIDDEN
};

typedef struct LayerConnection {
	int index_from;
	int index_to;
	
	atom layer_to;
	
	float weight;
} LayerConnection;

typedef struct Layer {
	atom name;
	
	float* parameters;
	int parameter_type;
	int parameter_size;
	
	int layer_type;
	
	atom* layer_keys;
	
	LayerConnection* connections;
	int connection_size;
} Layer;

typedef struct Context {
	KeyValue values[100];	// TODO: switch to dynamic array
	int value_count;
	
	Layer* layers;
	int layer_size;
	int layer_allocated;
} Context;




Context* goetia_script_MakeContext();

void goetia_script_BindValue(Context* c, atom atom, Value value);
void goetia_script_UnbindValue(Context* c, atom atom);
Value goetia_script_GetValue(Context* c, atom atom);

void goetia_script_NewLayer(Context* c, atom name, int layer_type, int value_type, int layer_size, atom* layer_keys);
void goetia_script_SetLayerWeight(Context* context, atom from_name, int from_index, atom to_name, int to_index, float weight);

void goetia_script_SetLayerInput(Context* c, atom name, int index, float value);
float goetia_script_GetLayerOutput(Context* c, atom name, int index);

void goetia_script_FeedforwardLayer(Context* c, atom name);
void goetia_script_ResetLayer(Context* c, atom name);

#endif // CONTEXT_H