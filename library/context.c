// CONTEXT.C - libgoetia
// Script execution context definition, function implementation.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <eval.h>
#include <context.h>

/// Creates a new script context.
/// @return Pointer to the new script context.
Context* goetia_script_MakeContext() {
	Context* context = malloc(sizeof(Context));
	
	context->value_count = 0;
	
	context->layers = NULL;
	context->layer_size = 0;
	context->layer_allocated = 0;
	
	return context;
}

/// Binds a value to a variable.
void goetia_script_BindValue(Context* c, atom atom, Value value) {
	for (int i = 0; i < c->value_count; i++) {
		if (atom == c->values[i].name) {
			c->values[i].value = value;
			return;
		}
	}
	
	c->values[c->value_count].name = atom;
	c->values[c->value_count].value = value;
	c->value_count++;
}

/// Unbinds a value from a variable.
void goetia_script_UnbindValue(Context* c, atom atom) {
	int unbind_index = -1;
	
	for (int i = 0; i < c->value_count; i++) {
		if (atom == c->values[i].name) {
			unbind_index = i;
			break;
		}
	}
	
	if (unbind_index == -1) return;
	
	c->value_count--;
	
	for (int i = unbind_index; i < c->value_count; i++) {
		c->values[i] = c->values[i + 1];
	}
}

/// Retrieves a value to a variable.
Value goetia_script_GetValue(Context* c, atom atom) {
	// TODO: replace this with a hash map or idk something
	for (int i = 0; i < c->value_count; i++) {
		if (c->values[i].name == atom) {
			return c->values[i].value;
		}
	}
	
	return goetia_script_error("Value atom unbound!!");
}



// NEURAL NET STUFF

/// Creates a new neural net layer.
/// @param layer_type Either LAYER_INPUT, LAYER_OUTPUT or LAYER_HIDDEN.
/// @param value_type Either LAYER_CLASS, LAYER_SCALAR or LAYER_VECTOR. For
///                   LAYER_HIDDEN this will be ignored and it will be treated
///                   as a LAYER_VECTOR.
/// @param layer_size Ignored for LAYER_SCALAR, number of vector dimensions for 
///                   LAYER_VECTOR or number of atoms for for LAYER_CLASS.
/// @param layer_keys Class names for LAYER_CLASS, otherwise ignored.
void goetia_script_NewLayer(Context* context, atom name, int layer_type, int value_type, int layer_size, atom* layer_keys) {
	if (!context->layers) {
		context->layers = malloc(sizeof(Layer) * 10);
		context->layer_size = 0;
		context->layer_allocated = 10;
	}
	
	int layer_parameters = value_type == LAYER_SCALAR ? 1 : layer_size;
	
	Layer* new_layer = &context->layers[context->layer_size++];
	
	new_layer->name = name;
	new_layer->parameters = malloc(sizeof(float) * layer_parameters);
	new_layer->parameter_type = value_type;
	new_layer->parameter_size = layer_parameters;
	
	memset(new_layer->parameters, 0, sizeof(float) * new_layer->parameter_size);
	
	if (value_type == LAYER_CLASS) {
		new_layer->layer_keys = malloc(layer_size * sizeof(atom));
		for (int i = 0; i < layer_size; i++) new_layer->layer_keys[i] = layer_keys[i];
	} else {
		new_layer->layer_keys = NULL;
	}
	
	new_layer->connection_size = 0;
	new_layer->connections = malloc(sizeof(LayerConnection) * 100);
}

/// Adds a connection from a layer to another layer.
void goetia_script_SetLayerWeight(Context* context, atom from_name, int from_index, atom to_name, int to_index, float weight) {
	Layer* from = NULL;
	Layer* to = NULL;
	
	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == from_name) {
			from = &context->layers[i];
			break;
		}
	}
	
	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == to_name) {
			to = &context->layers[i];
			break;
		}
	}
	
	if (!from) {
		printf("Layer 'from' %i not found!\n", from_name);
		return;
	}
	
	if (!to) {
		printf("Layer 'to' %i not found!\n", to_name);
		return;
	}
	
	from->connections[from->connection_size].index_from = from_index;
	from->connections[from->connection_size].index_to = to_index;
	from->connections[from->connection_size].layer_to = to_name;

	from->connections[from->connection_size].weight = weight;
	
	from->connection_size++;
}

/// Sets the input value for an input layer.
void goetia_script_SetLayerInput(Context* context, atom name, int index, float value) {
	Layer* layer = NULL;

	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == name) {
			layer = &context->layers[i];
			break;
		}
	}
	
	if (!layer) {
		printf("Layer 'layer' %i not found!\n", name);
		return;
	}
	
	layer->parameters[index] = value;
}

/// Gets the output value for an output layer.
float goetia_script_GetLayerOutput(Context* context, atom name, int index) {
	Layer* layer = NULL;

	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == name) {
			layer = &context->layers[i];
			break;
		}
	}
	
	if (!layer) {
		printf("Layer 'layer' %i not found!\n", name);
		return 0.0f;
	}
	
	return layer->parameters[index];
}

/// Pushes a layer's weights to the layer's that it's connected to.
void goetia_script_FeedforwardLayer(Context* context, atom name) {
	Layer* layer = NULL;

	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == name) {
			layer = &context->layers[i];
			break;
		}
	}
	
	if (!layer) {
		printf("Layer 'layer' %i not found!\n", name);
		return;
	}
	
	for (int i = 0; i < layer->connection_size; i++) {
		if (layer->parameters[layer->connections[i].index_from] < 1.0f) continue;
		
		
		for (int j = 0; j < layer->connection_size; j++) {
			Layer* layer_to_to = NULL;

			for (int k = 0; k < context->layer_size; k++) {
				if (context->layers[k].name == layer->connections[i].layer_to) {
					layer_to_to = &context->layers[k];
					break;
				}
			}
			
			if (!layer) {
				printf("Layer 'layer to to' %i not found!\n", name);
				return;
			}
			
			layer_to_to->parameters[layer->connections[i].index_to] += layer->connections[i].weight;
		}
	}
}

/// Resets the internal values for a layer.
void goetia_script_ResetLayer(Context* context, atom name) {
	Layer* layer = NULL;

	for (int i = 0; i < context->layer_size; i++) {
		if (context->layers[i].name == name) {
			layer = &context->layers[i];
			break;
		}
	}
	
	if (!layer) {
		printf("Layer 'layer' %i not found!\n", name);
		return;
	}
	
	for (int i = 0; i < layer->parameter_size; i++) {
		layer->parameters[i] = 0.0f;
	}
}
