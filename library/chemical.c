// CHEMICAL.C - libgoetia
// Chemical, chemical container function implementations.

#include <chemical.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_REACTION_COUNT 100
#define MAX_CHEMICAL_COUNT 100

static int auto_reaction_count = 0;
static int enzyme_reaction_count = 0;
static int chemical_count = 0;

static Reaction auto_reactions[MAX_REACTION_COUNT];
static Reaction enzyme_reactions[MAX_REACTION_COUNT];
static int chemical_properties[MAX_CHEMICAL_COUNT];

static atom chemical_to_atom[MAX_CHEMICAL_COUNT];
static chem atom_to_chemical[1000]; // uhh ... switch to ?? hash map?

/// Registers a new chemical.
chem goetia_chemical_Register(atom chemical_name, int properties) {
	chem new_chem_id = chemical_count++;
	
	chemical_to_atom[new_chem_id] = chemical_name;
	atom_to_chemical[chemical_name] = new_chem_id;
	
	chemical_properties[new_chem_id] = properties;
	
	return new_chem_id;
}

/// Registers a new reaction.
void goetia_chemical_RegisterReaction(Reaction reaction, bool auto_reaction) {
	if (auto_reaction) {
		auto_reactions[auto_reaction_count++] = reaction;
	} else {
		enzyme_reactions[enzyme_reaction_count++] = reaction;
	}
}

/// Registers default chemicals.
void goetia_chemical_RegisterDefaultChemicals() {
#define r(X, Y) goetia_chemical_Register(goetia_atom_FromString(X), Y);
	r("none", 0)
	r("anima", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("light", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("water", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("oxygen", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("nitrogen", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("carbon-monoxide", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("carbon-dioxide", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("atp", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("adp", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("ethanol", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("vinegar", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("glucose", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("pyruvate", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("pyruvate-oxide", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("glycogen", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("fat", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("starch", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("cellulose", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("fatty-acid", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("lactate", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("phosphate", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("nitrate", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("amino-acid", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("ammonia", CHEM_SOLUBLE | CHEM_VOLATILE)
	r("urea", CHEM_SOLUBLE | CHEM_VOLATILE)
#undef r
	assert(chemical_count == CHEM_LAST);
}

/// Registers default reactions.
void goetia_chemical_RegisterDefaultReactions() {
	#define r(T, X0, X1, X2, X3, X4, X5, X6, X7, X8, X9, A) \
		Reaction T = {X0, X1, X2, X3, X4, X5, X6, X7, X8, X9}; \
		goetia_chemical_RegisterReaction(T, A);
	
	
	
	
	// glucose <-> glycogen
	r(r01, CHEM_GLUCOSE, CHEM_ATP, CHEM_NONE, CHEM_GLYCOGEN, CHEM_ADP, 1, 1, 1, 1, 1, false)
	r(r02, CHEM_GLYCOGEN, CHEM_ATP, CHEM_NONE, CHEM_GLUCOSE, CHEM_ADP, 1, 1, 1, 1, 1, false)
	
	// amino acid and adp synthase
	r(r03, CHEM_NITRATE, CHEM_NONE, CHEM_NONE, CHEM_AMINO_ACID, CHEM_NONE, 1, 0, 1, 1, 1, false)
	r(r04, CHEM_PHOSPHATE, CHEM_NONE, CHEM_NONE, CHEM_ADP, CHEM_NONE, 1, 0, 1, 1, 1, false)
	
	// amino acid breakdown
	r(r05, CHEM_AMINO_ACID, CHEM_NONE, CHEM_NONE, CHEM_PYRUVATE, CHEM_AMMONIA, 1, 0, 1, 1, 1, false)
	r(r06, CHEM_AMMONIA, CHEM_ATP, CHEM_NONE, CHEM_UREA, CHEM_ADP, 1, 1, 1, 1, 1, false)
	
	// fat synthase
	r(r07, CHEM_PYRUVATE, CHEM_NONE, CHEM_NONE, CHEM_FATTY_ACID, CHEM_NONE, 1, 0, 1, 0, 1, false)
	r(r08, CHEM_FATTY_ACID, CHEM_NONE, CHEM_NONE, CHEM_FAT, CHEM_NONE, 3, 0, 1, 0, 1, false)
	
	// plant synthases
	r(r09, CHEM_GLUCOSE, CHEM_ATP, CHEM_NONE, CHEM_STARCH, CHEM_ADP, 1, 1, 1, 1, 1, false)
	r(r10, CHEM_GLUCOSE, CHEM_ATP, CHEM_NONE, CHEM_CELLULOSE, CHEM_ADP, 1, 1, 1, 1, 1, false)
	
	// photsynthesis system 1 and 2
	r(r11, CHEM_WATER, CHEM_ADP, CHEM_LIGHT, CHEM_OXYGEN, CHEM_ATP, 2, 3, 1, 3, 1, false)
	r(r12, CHEM_CARBON_DIOXIDE, CHEM_ATP, CHEM_NONE, CHEM_GLUCOSE, CHEM_ADP, 6, 18, 1, 2, 1, false)

	// yeasting
	r(r14, CHEM_PYRUVATE, CHEM_NONE, CHEM_NONE, CHEM_ETHANOL, CHEM_NONE, 1, 0, 1, 0, 1, false)
	
	// gluconeogenesis
	r(r15, CHEM_ATP, CHEM_PYRUVATE, CHEM_NONE, CHEM_GLUCOSE, CHEM_ADP, 6, 2, 1, 6, 1, false)

	// glycolysis
	r(r16, CHEM_GLUCOSE, CHEM_ADP, CHEM_NONE, CHEM_PYRUVATE, CHEM_ATP, 1, 2, 2, 2, 1, false)
	
	// respiration
	// TODO: change this to make 38 ATPs from 1 glucose
	// also carbon dioxide should be let off in the first reation? one carbon?
	// and in the total there should be 6 carbons
	// and maybe change pyruvate_oxide to citrate
	r(r17, CHEM_PYRUVATE, CHEM_OXYGEN, CHEM_NONE, CHEM_PYRUVATE_OXIDE, CHEM_NONE, 2, 2, 2, 0, 1, false)
	r(r18, CHEM_PYRUVATE_OXIDE, CHEM_ADP, CHEM_NONE, CHEM_CARBON_DIOXIDE, CHEM_ATP, 2, 2, 2, 2, 1, false)
	
	// fermentation
	r(r19, CHEM_GLUCOSE, CHEM_ADP, CHEM_NONE, CHEM_LACTATE, CHEM_ATP, 2, 2, 2, 2, 1, false)
	r(r20, CHEM_GLUCOSE, CHEM_ADP, CHEM_NONE, CHEM_ETHANOL, CHEM_ATP, 2, 2, 2, 2, 1, false)
	r(r21, CHEM_LACTATE, CHEM_OXYGEN, CHEM_NONE, CHEM_PYRUVATE, CHEM_NONE, 2, 2, 2, 0, 1, false)
	
	#undef r
}



/// Updates a chemical container.
/// Performs auto reactions in a container.
void goetia_chemical_UpdateContainer(ChemContainer* c) {
	for (int i = 0; i < auto_reaction_count; i++) {
		chem chem1 = auto_reactions[i].reactant1;
		chem chem2 = auto_reactions[i].reactant2;
		chem ctlyt = auto_reactions[i].catalyst;
		
		// check if enough reactants
		if (auto_reactions[i].reactant1_ammount > c->chems[chem1] ||
			(chem2 && auto_reactions[i].reactant2_ammount > c->chems[chem2]) ||
			(ctlyt && !c->chems[ctlyt])
		) continue;
		
		//printf("REACT: %s %s %s %s \n", 
		//	goetia_atom_ToString(goetia_chemical_ChemicalToAtom(chem1)),
		//	goetia_atom_ToString(goetia_chemical_ChemicalToAtom(chem2)),
		//	goetia_atom_ToString(goetia_chemical_ChemicalToAtom(auto_reactions[i].product)),
		//	goetia_atom_ToString(goetia_chemical_ChemicalToAtom(auto_reactions[i].byproduct))
		//);
		
		c->chems[chem1] -= auto_reactions[i].reactant1_ammount;
		c->chems[chem2] -= auto_reactions[i].reactant2_ammount;
		
		c->chems[auto_reactions[i].product] += auto_reactions[i].product_ammount;
		c->chems[auto_reactions[i].byproduct] += auto_reactions[i].byproduct_ammount;
	}
}

/// Performs a reaction.
int goetia_chemical_React(ChemContainer* container, chem chem1, chem chem2, chem prod) {
	const Reaction* r = NULL;
	for (int i = 0; i < enzyme_reaction_count; i++) {
		if (enzyme_reactions[i].reactant1 == chem1 && 
			enzyme_reactions[i].reactant2 == chem2 &&
			enzyme_reactions[i].product == prod
		) {
			r = &enzyme_reactions[i];
			break;
		}
		
		if (enzyme_reactions[i].reactant1 == chem2 && 
			enzyme_reactions[i].reactant2 == chem1 &&
			enzyme_reactions[i].product == prod
		) {
			r = &enzyme_reactions[i];
			
			chem chem_swap = chem1;
			chem1 = chem2;
			chem2 = chem_swap;
			
			break;
		}
	}
	
	// invalid reaction
	if (!r) return 0;
	
	// no catalyst
	if (r->catalyst && !container->chems[r->catalyst]) return 0;
	
	// not enough reactants
	if (r->reactant1_ammount > container->chems[r->reactant1] ||
		r->reactant2_ammount > container->chems[r->reactant2]
	) return 0;
	
	container->chems[r->reactant1] -= r->reactant1_ammount;
	container->chems[r->reactant2] -= r->reactant2_ammount;
	
	container->chems[r->product] += r->product_ammount;
	container->chems[r->byproduct] += r->byproduct_ammount;
	
	return r->product_ammount;
}

/// Adds a chemical to a container.
/// @return Ammount of the chemical added.
int goetia_chemical_Add(ChemContainer* container, chem chemical, int ammount) {
	container->chems[chemical] += ammount;
	return ammount;
}

/// Removes a chemical from a container.
/// @return Ammount of the chemical removed.
int goetia_chemical_Remove(ChemContainer* container, chem chemical, int ammount) {
	container->chems[chemical] -= ammount;
	if (container->chems[chemical] < 0) {
		ammount += container->chems[chemical];
		container->chems[chemical] = 0;
		return ammount;
	} else {
		return ammount;
	}
}

/// Finds the properties of a chemical.
/// @return Bitmask representing properties of chemical.
int goetia_chemical_Property(chem chemical) {
	return chemical_properties[chemical];
}

/// Creates an empty chemical container.
ChemContainer* goetia_chemical_MakeContainer() {
	ChemContainer* container = malloc(sizeof(ChemContainer));
	memset(container, 0, sizeof(ChemContainer));
	container->properties = CHEM_ALLOW_SOLUBLE | CHEM_ALLOW_VOLATILE | CHEM_ALLOW_DIFFUSION;
	return container;
}

/// Creates a chemical container from a list.
/// The list should consist of tuples in the form of (chem ammount). All
/// undefined chems will be assumed to have an ammount of zero.
ChemContainer* goetia_chemical_ListToContainer(List* list) {
	ChemContainer* container = goetia_chemical_MakeContainer();
	do {
		chem chem = goetia_chemical_AtomToChemical(list->data.list->data.atom);
		int ammount = list->data.list->next->data.int32;
		
		if (chem > 0 && chem < CHEM_LAST) {
			container->chems[chem] = ammount;
		} else {
			printf("Unrecognized chemical %s!\n", goetia_atom_ToString(list->data.list->data.atom));
		}
	} while (list = list->next);
	return container;
}

/// Diffuses chemicals.
/// Diffuses chemicals from c1 to c2, if both of them allow it.
void goetia_chemical_Diffuse(ChemContainer* c1, ChemContainer* c2) {
	if (!(c1->properties & CHEM_ALLOW_DIFFUSION) || 
		!(c2->properties & CHEM_ALLOW_DIFFUSION)) {
		return;
	}
	
	for (chem chem = 1; chem < CHEM_LAST; chem++) {
		if (((goetia_chemical_Property(chem) & CHEM_SOLUBLE) && 
			(c2->properties & CHEM_ALLOW_SOLUBLE)) || ((goetia_chemical_Property(chem) & CHEM_VOLATILE) && 
			(c2->properties & CHEM_ALLOW_VOLATILE))
		) {
			if (c1->chems[chem] > c2->chems[chem]) {
				c1->chems[chem]--;
				c2->chems[chem]++;
			}
		}
	}
}

/// Prints out the contents of the container.
void goetia_chemical_PrintContainer(ChemContainer* container) {
	for (int i = 0; i < CHEM_LAST; i++) {
		if (container->chems[i]) {
			printf("%s %i\n",
				goetia_atom_ToString(goetia_chemical_ChemicalToAtom(i)),
				container->chems[i]
			);
		}
	}
}

/// Converts a chemical ID to an atom.
atom goetia_chemical_ChemicalToAtom(chem chemical) {
	return chemical_to_atom[chemical];

}

/// Converts an atom to chemical ID.
chem goetia_chemical_AtomToChemical(atom atom) {
	return atom_to_chemical[atom];
}