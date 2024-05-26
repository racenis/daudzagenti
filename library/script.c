// SCRIPT.H - libgoetia
// Additional script functions.
// If eval.c contains the basic interpreter, then this file contains simulation
// and other interfaces for the scripting system.

#include <script.h>

#include <eval.h>
#include <world.h>

#include <api.h>

#include <stdio.h>
#include <stdlib.h>

// idk how many compilers support __VA_OPT__ yet
#if defined(__GNUC__) && __GNUC__ >= 12
	#define error(MSG, ...) goetia_script_error2(p, MSG __VA_OPT__(,) __VA_ARGS__)
#else
	#define error goetia_script_error
#endif

// Finds what agent_id the _self is bound to.
static agent_id find_self(Context* c) {
	for (int i = 0; i < c->value_count; i++) {
		if (c->values[i].name == ATOM_AGENT_SELF) {
			if (c->values[i].value.type != VALUE_INT) return 0;
			return c->values[i].value.int32;
		}
	}
	return 0;
}

static const Value ok = {ATOM_OK, VALUE_ATOM};
//static const Value true = {ATOM_TRUE, VALUE_ATOM};
//static const Value false = {ATOM_FALSE, VALUE_ATOM};

/* ************************************************************************** */
/*                                                                            */
/*                                 SIMULATION                                 */
/*                                                                            */
/* ************************************************************************** */

static Value random_number(Context* c, List* p) {
	Value val;
	
	val.type = VALUE_FLOAT;
	val.float32 = (float)rand() / (float)RAND_MAX;
	
	return val;
}

static Value inject(Context* c, List* p) {
	if (!p || !p->next || !p->next->next) {
		return error("Function 'inject' missing parameters.\n");
	}
	
	Agent* source_agent = goetia_world_FindAgent(find_self(c));
	Agent* target_agent = goetia_world_FindAgent(goetia_script_EvaluateValue(c, p->data).int32);
	if (!source_agent) {
		return error("Function 'inject' _self %i not found.\n", find_self(c));
	}
	if (!target_agent) {
		return error("Function 'inject' target %i not found.\n", p->data.int32);
	}
	
	chem chem = goetia_chemical_AtomToChemical(p->next->data.atom);
	if (!chem) {
		return error("Function 'inject' chemical %s not found.\n", goetia_atom_ToString(p->next->data.atom));
	}
	
	int ammount = p->next->next->data.int32;
	if (ammount < 1) {
		return error("Function 'inject' invalid chem ammount %i.\n", ammount);
	}
	
	ChemContainer* source = source_agent->chemicals;
	ChemContainer* target = target_agent->chemicals;
	
	if (!source) return ok;
	if (!target) {
		target_agent->chemicals = goetia_chemical_MakeContainer();
		target = target_agent->chemicals;
	}
	
	int removed = goetia_chemical_Remove(source, chem, ammount);
	goetia_chemical_Add(target, chem, removed);
	
	printf("Injected %i of %i.\n", removed, chem);
	
	return ok;
}

static Value ingest(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'ingest' missing parameters.\n");
	}
	
	Agent* target_agent = goetia_world_FindAgent(find_self(c));
	if (!target_agent) {
		return error("Function 'ingest' _self %i not found.\n", find_self(c));
	}
	
	chem chem = goetia_chemical_AtomToChemical(p->data.atom);
	if (!chem) {
		return error("Function 'ingest' chemical %s not found.\n", goetia_atom_ToString(p->next->data.atom));
	}
	
	int ammount = p->next->data.int32;
	if (ammount < 1) {
		return error("Function 'ingest' invalid chem ammount %i.\n", ammount);
	}
	
	Sector* sector = goetia_world_FindSectorByPosition(target_agent->position);
	if (!sector) return error("Agent not in sector, can't ingest!");
	ChemContainer* source = sector->chemicals;
	ChemContainer* target = target_agent->chemicals;
	if (!target) {
		target_agent->chemicals = goetia_chemical_MakeContainer();
		target = target_agent->chemicals;
	}
		
	int removed = goetia_chemical_Remove(source, chem, ammount);
	goetia_chemical_Add(target, chem, removed);
	
	return ok;
}

static Value perform(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'perform' missing parameters.\n");
	}
	
	Agent* agent = goetia_world_FindAgent(find_self(c));
	if (!agent) {
		return error("Function 'perform' _self %i not found.\n", find_self(c));
	}
	
	action action = goetia_action_AtomToAction(p->data.atom);
	if (action == -1) {
		return error("Function 'perform' action %s not found.\n", goetia_atom_ToString(p->data.atom));
	}
	
	agent_id target = goetia_script_EvaluateValue(c, p->next->data).int32;
	
	agent->action.type = action;
	agent->action.target = target;
	
	return ok;
}

static Value react(Context* c, List* p) {
	if (!p || !p->next || !p->next->next) {
		return error("Function 'react' missing parameters.\n");
	}
	
	Agent* source_agent = goetia_world_FindAgent(find_self(c));
	if (!source_agent) {
		return error("Function 'inject' _self %i not found.\n", find_self(c));
	}
	
	
	chem chem1 = goetia_chemical_AtomToChemical(p->data.atom);
	chem chem2 = goetia_chemical_AtomToChemical(p->next->data.atom);
	chem chem3 = goetia_chemical_AtomToChemical(p->next->next->data.atom);
	
	if (!chem1 || !chem2 || !chem3) {
		return error("Function 'react' chemical %s or %s or %s not found.\n",
			goetia_atom_ToString(p->data.atom),
			goetia_atom_ToString(p->next->data.atom),
			goetia_atom_ToString(p->next->next->data.atom)
		);
	}
	
	/*int ammount1 = p->next->data.int32;
	int ammount2 = p->next->next->next->data.int32;
	if (ammount1 < 1 || ammount2 < 1) {
		return error("Function 'react' invalid chem ammount %i or %i.\n", ammount1, ammount2);
	}*/
	
	ChemContainer* container = source_agent->chemicals;
	if (!container) {
		source_agent->chemicals = goetia_chemical_MakeContainer();
		container = source_agent->chemicals;
	}
	
	goetia_chemical_React(container, chem1, chem2, chem3);
	
	return ok;
}

static Value eject(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'eject' missing parameters.\n");
	}
	
	Agent* source_agent = goetia_world_FindAgent(find_self(c));
	if (!source_agent) {
		return error("Function 'eject' _self %i not found.\n", find_self(c));
	}
	
	chem chem = goetia_chemical_AtomToChemical(p->data.atom);
	if (!chem) {
		return error("Function 'eject' chemical %s not found.\n", goetia_atom_ToString(p->next->data.atom));
	}
	
	int ammount = p->next->data.int32;
	if (ammount < 1) {
		return error("Function 'eject' invalid chem ammount %i.\n", ammount);
	}
	
	Sector* sector = goetia_world_FindSectorByPosition(source_agent->position);
	if (!sector) return error("Agent not in sector, can't eject!");
	ChemContainer* target = sector->chemicals;
	ChemContainer* source = source_agent->chemicals;
	if (!source) {
		source_agent->chemicals = goetia_chemical_MakeContainer();
		source = source_agent->chemicals;
	}
		
	int removed = goetia_chemical_Remove(source, chem, ammount);
	goetia_chemical_Add(target, chem, removed);
	
	return ok;
}

static Value offer(Context* c, List* p) {
	if (!p) {
		return error("Function 'offer' missing parameters.\n");
	}
	
	Agent* source_agent = goetia_world_FindAgent(find_self(c));
	if (!source_agent) {
		return error("Function 'offer' _self %i not found.\n", find_self(c));
	}
	
	// TODO: actually figure out what is being offered

	return ok;
}

static Value kill(Context* c, List* p) {
	if (!p) {
		return error("Function 'kill' missing parameters.\n");
	}
	
	Agent* agent = goetia_world_FindAgent(goetia_script_Evaluate(c, p).int32);
	
	// we could just call goetia_world_RemoveAgent(), but that might mess
	// something up, so instead we set the agent's dead flag, so that it
	// can be removed during the next goetia_world_Tick().
	
	if (agent) {
		agent->flags |= FLAG_DEAD;
	}
	
	// another thing we could do here is to take all of the chemicals in the
	// agent's chemical bag and eject them into the world
	
	return ok;
}

static Value probe(Context* c, List* p) {
	if (!p) {
		return error("Function 'probe' missing parameters.\n");
	}
	
	Agent* target_agent = goetia_world_FindAgent(find_self(c));
	if (!target_agent) {
		return error("Function 'probe' _self %i not found.\n", find_self(c));
	}
	
	chem chem = goetia_chemical_AtomToChemical(p->data.atom);
	if (!chem) {
		return error("Function 'probe' chemical %s not found.\n", goetia_atom_ToString(p->next->data.atom));
	}
	
	Sector* sector = goetia_world_FindSectorByPosition(target_agent->position);
	if (!sector) {
		return error("Agent not in sector, can't probe!");
	}
	
	Value val;
	val.type = VALUE_INT;
	val.int32 = sector->chemicals->chems[chem];
	
	return val;
}

static Value measure(Context* c, List* p) {
	if (!p) {
		return error("Function 'measure' missing parameters.\n");
	}
	
	Agent* target_agent = goetia_world_FindAgent(find_self(c));
	if (!target_agent) {
		return error("Function 'measure' _self %i not found.\n", find_self(c));
	}
	
	chem chem = goetia_chemical_AtomToChemical(p->data.atom);
	if (!chem) {
		return error("Function 'measure' chemical %s not found.\n", goetia_atom_ToString(p->next->data.atom));
	}
	
	Value val;
	val.type = VALUE_INT;
	val.int32 = target_agent->chemicals->chems[chem];
	
	return val;
}

static Value set_model(Context* c, List* p) {
	if (!p) {
		return error("Function 'set-model' missing parameters.\n");
	}
	
	agent_id id = find_self(c);
	if (!id) {
		return error("Function 'set-model' _self %i not found.\n", find_self(c));
	}
	
	goetia_api_FireCallbackAgentModelSet(id, p->data.atom);
	
	return ok;
}

static Value set_scale(Context* c, List* p) {
	if (!p) {
		return error("Function 'set-scale' missing parameters.\n");
	}
	
	agent_id id = find_self(c);
	if (!id) {
		return error("Function 'set-scale' _self %i not found.\n", find_self(c));
	}
	
	goetia_api_FireCallbackAgentScaleSet(id, goetia_script_EvaluateValue(c, p->data).float32);
	
	return ok;
}

static Value set_color(Context* c, List* p) {
	if (!p || !p->next || !p->next->next) {
		return error("Function 'set-color' missing parameters.\n");
	}
	
	agent_id id = find_self(c);
	if (!id) {
		return error("Function 'set-color' _self %i not found.\n", find_self(c));
	}
	
	
	
	goetia_api_FireCallbackAgentColorSet(
		id,
		goetia_script_EvaluateValue(c, p->data).float32, 
		goetia_script_EvaluateValue(c, p->next->data).float32, 
		goetia_script_EvaluateValue(c, p->next->next->data).float32);
	
	return ok;
}

static Value set_animation(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'set-animation' missing parameters.\n");
	}
	
	agent_id id = find_self(c);
	if (!id) {
		return error("Function 'set-animation' _self %i not found.\n", find_self(c));
	}
	
	goetia_api_FireCallbackAgentAnimationSet(id, p->data.atom, p->next->data.int32);
	
	return ok;
}

static Value lobe(Context* c, List* p) {
	if (!p) {
		return error("Function 'lobe' missing first parameter.\n");
	}
	
	Agent* agent = goetia_world_FindAgent(find_self(c));
	if (!agent) {
		return error("Function 'lobe' _self %i not found.\n", find_self(c));
	}
	
	if (p->data.type != VALUE_ATOM) {
		return error("Function 'lobe' first parameter needs to be atom.\n");
	}
	
	switch (p->data.atom) {
		case ATOM_MAKE:
			if (!p->next || !p->next->next || !p->next->next->next) {
				return error("Function 'lobe make' missing parameters.\n");
			}
			
			const List* layer_type = p->next;
			const List* layer_name = p->next->next;
			
			if (layer_type->data.type != VALUE_ATOM) {
				return error("Function 'lobe make' layer type invalid.\n");
			}
			
			if (layer_name->data.type != VALUE_ATOM) {
				return error("Function 'lobe make' layer name invalid.\n");
			}
			
			int layer_type_enum = 0;
			switch (layer_type->data.atom) {
				case ATOM_INPUT:	layer_type_enum = LAYER_INPUT;	break;
				case ATOM_OUTPUT:	layer_type_enum = LAYER_OUTPUT;	break;
				case ATOM_HIDDEN:	layer_type_enum = LAYER_HIDDEN;	break;
				default: return error("Invalid layer type!\n");
			}

			if (layer_type_enum == LAYER_HIDDEN) {
				const List* layer_size = p->next->next->next;
				if (layer_size->data.type != VALUE_INT) {
					return error("Hidden layer need int size.\n");
				}
				goetia_script_NewLayer(c, layer_name->data.atom, layer_type_enum, LAYER_VECTOR, layer_size->data.int32, NULL);
			} else {
				const List* layer_value = p->next->next->next;
				if (layer_value->data.type != VALUE_ATOM) {
					return error("Function 'lobe make' value type not.\n");
				}
				
				switch (layer_value->data.atom) {
					case ATOM_CLASS:
						const List* layer_classes = p->next->next->next->next;
						if (layer_classes->data.type != VALUE_LIST) {
							return error("Classes need to be list.\n");
						}
						
						layer_classes = layer_classes->data.list;
						
						atom classes[10];
						int class_count = 0;
						for (int i = 0; i < 10; i++) {
							if (!layer_classes) break;
							
							if (layer_classes->data.type != VALUE_ATOM) {
								return error("Classes need to be atom.\n");
							}
							
							classes[i] = layer_classes->data.atom;
							class_count++;
							
							layer_classes = layer_classes->next;
						}
						
						if (!class_count) {
							return error("Classes need some classes in there\n");
						}
						
						goetia_script_NewLayer(c, layer_name->data.atom, layer_type_enum, LAYER_CLASS, class_count, classes);
						break;
					case ATOM_SCALAR:
						goetia_script_NewLayer(c, layer_name->data.atom, layer_type_enum, LAYER_SCALAR, 1, NULL);
						break;
					case ATOM_VECTOR:
						const List* layer_size = p->next->next->next;
						if (layer_size->data.type != VALUE_INT) {
							return error("Hidden layer need int size.\n");
						}
						goetia_script_NewLayer(c, layer_name->data.atom, layer_type_enum, LAYER_VECTOR, layer_size->data.int32, NULL);
						break;
					default: return error("Invalid layer value!\n");
				}
			}
			break;
		
		case ATOM_WEIGHT:
			if (!p->next || !p->next->next || !p->next->next->next || !p->next->next->next->next || !p->next->next->next->next->next) {
				return error("Function 'lobe weight' missing parameters.\n");
			}
			
			List* from_layer = p->next;
			List* from_index = p->next->next;
			List* to_layer = p->next->next->next;
			List* to_index = p->next->next->next->next;
			List* weight = p->next->next->next->next->next;
			
			if (from_layer->data.type != VALUE_ATOM ||
				from_index->data.type != VALUE_INT ||
				to_layer->data.type != VALUE_ATOM ||
				to_index->data.type != VALUE_INT ||
				weight->data.type != VALUE_FLOAT
			) {
				return error("Function 'lobe weight' type error.\n");
			}
			
			goetia_script_SetLayerWeight(c, 
				from_layer->data.atom,
				from_index->data.int32,
				to_layer->data.atom,
				to_index->data.int32,
				weight->data.float32
			);
			break;
			
		case ATOM_SET: {
			if (!p->next || !p->next->next || !p->next->next->next) {
				return error("Function 'lobe set' missing parameters.\n");
			}
			
			List* layer = p->next;
			List* index = p->next->next;
			List* weight = p->next->next->next;
			
			if (layer->data.type != VALUE_ATOM ||
				index->data.type != VALUE_INT ||
				weight->data.type != VALUE_FLOAT
			) {
				return error("Function 'lobe set' type error.\n");
			}
			
			goetia_script_SetLayerInput(c, 
				layer->data.atom,
				index->data.int32,
				weight->data.float32
			);
			} break;
		case ATOM_GET: {
			if (!p->next || !p->next->next) {
				return error("Function 'lobe get' missing parameters.\n");
			}
			
			List* layer = p->next;
			List* index = p->next->next;
			
			if (layer->data.type != VALUE_ATOM ||
				index->data.type != VALUE_INT
			) {
				return error("Function 'lobe get' type error.\n");
			}
			
			float weight = goetia_script_GetLayerOutput(c, 
				layer->data.atom,
				index->data.int32
			);
			
			return goetia_value_MakeFloat(weight);
			}
			
		case ATOM_FORWARD: {
			if (!p->next) {
				return error("Function 'lobe forwdfa' missing parameters.\n");
			}
			
			List* layer = p->next;
			
			if (layer->data.type != VALUE_ATOM) {
				return error("Function 'lobe proapeg' type error.\n");
			}
			
			goetia_script_FeedforwardLayer(c, layer->data.atom);
			
			} break;
	}
	
	
	
	return ok;
}

static Value instantiate(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'instantiate' missing parameters.\n");
	}
	
	Agent* agent = goetia_world_FindAgent(find_self(c));
	if (!agent) {
		return error("Function 'instantiate' _self %i not found.\n", find_self(c));
	}
	
	Agent* prototype = goetia_world_FindPrototype(goetia_script_EvaluateValue(c, p->data).atom);
	
	if (!prototype) {
		return error("Function 'instantiate' prototype %i not found.\n", p->data.int32);
	}
	
	// check if have enough chemicals to instantiate
	for (int i = 0; i < CHEM_LAST; i++) {
		if (prototype->chemicals->chems[i] > agent->chemicals->chems[i]) {
			return goetia_value_MakeInt(0);
		}
	}
	
	if (p->next->data.type != VALUE_ATOM || p->next->data.atom != ATOM_AT) {
		return error("Function 'instantiate' needs 'at'.\n");
	}
	
	Agent instance = goetia_agent_Copy(prototype);
	
	instance.position.x = goetia_script_EvaluateValue(c, p->next->next->data).float32;
	instance.position.y = goetia_script_EvaluateValue(c, p->next->next->next->data).float32;
	instance.position.z = goetia_script_EvaluateValue(c, p->next->next->next->next->data).float32;
	
	for (int i = 0; i < CHEM_LAST; i++) {
		agent->chemicals->chems[i] -= instance.chemicals->chems[i];
	}
	
	return goetia_value_MakeInt(goetia_world_InjectAgent(instance));
}

static Value get(Context* c, List* p) {
	if (!p || !p->next) {
		return error("Function 'get' missing parameters.\n");
	}
	
	Agent* target_agent = goetia_world_FindAgent(goetia_script_EvaluateValue(c, p->data).int32);
	if (!target_agent) {
		return error("Function 'get' target %i not found.\n", p->data.int32);
	}
	
	atom type = p->next->data.atom;
	atom subtype;
	
	switch (type) {
		case ATOM_POSITION:
			subtype = p->next->next->data.atom;
			switch (subtype) {
				case ATOM_X:
					return goetia_value_MakeFloat(target_agent->position.x);
				case ATOM_Y:
					return goetia_value_MakeFloat(target_agent->position.y);
				case ATOM_Z:
					return goetia_value_MakeFloat(target_agent->position.z);
			}
		case ATOM_CATEGORY:
			subtype = p->next->next->data.atom;
			switch (subtype) {
				case ATOM_CATEGORY:
					return goetia_value_MakeAtom(target_agent->category);
				case ATOM_VISUAL:
					return goetia_value_MakeAtom(target_agent->category_visual);
				case ATOM_TOUCH:
					return goetia_value_MakeAtom(target_agent->category_tactile);
			}
		default:
			return error("Function 'get' not good parameter.");
	}
	
	return ok;
}

/// Some additional script functions.
Value goetia_script_SimulationFunction(Context* c, atom atom, List* expr) {
	switch (atom) {
		case ATOM_RANDOM:			return random_number(c, expr);
		case ATOM_INJECT:			return inject(c, expr);
		case ATOM_INGEST: 			return ingest(c, expr);
		case ATOM_PERFORM: 			return perform(c, expr);
		case ATOM_REACT: 			return react(c, expr);
		case ATOM_EJECT: 			return eject(c, expr);
		case ATOM_OFFER: 			return offer(c, expr);
		case ATOM_KILL: 			return kill(c, expr);
		case ATOM_PROBE:			return probe(c, expr);
		case ATOM_MEASURE:			return measure(c, expr);
		case ATOM_SET_MODEL:		return set_model(c, expr);
		case ATOM_SET_SCALE:		return set_scale(c, expr);
		case ATOM_SET_COLOR:		return set_color(c, expr);
		case ATOM_SET_ANIMATION:	return set_animation(c, expr);
		case ATOM_LOBE:				return lobe(c, expr);
		case ATOM_INSTANTIATE:		return instantiate(c, expr);
		case ATOM_GET:				return get(c, expr);
		default:					break;
	}
		
	return goetia_script_error2(expr, "Function %s not found!\n", goetia_atom_ToString(atom));
}