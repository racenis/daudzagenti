// CHEMICAL.H - libgoetia
// Chemical, chemical container,definitions, functions.

#ifndef CHEMICAL_H
#define CHEMICAL_H

#include <stdbool.h>
#include <value.h>
#include <list.h>

/// Default chemical list.
enum {
	CHEM_NONE,
	CHEM_ANIMA,
	CHEM_LIGHT,
	CHEM_WATER,
	CHEM_OXYGEN,
	CHEM_NITROGEN,
	CHEM_CARBON_MONOXIDE,
	CHEM_CARBON_DIOXIDE,
	CHEM_ATP,
	CHEM_ADP,
	CHEM_ETHANOL,
	CHEM_VINEGAR,
	CHEM_GLUCOSE,
	CHEM_PYRUVATE,
	CHEM_PYRUVATE_OXIDE,
	CHEM_GLYCOGEN,
	CHEM_FAT,
	CHEM_STARCH,
	CHEM_CELLULOSE,
	CHEM_FATTY_ACID,
	CHEM_LACTATE,
	CHEM_PHOSPHATE,
	CHEM_NITRATE,
	CHEM_AMINO_ACID,
	CHEM_AMMONIA,
	CHEM_UREA,
	CHEM_LAST
};

/// Properties of chemicals.
enum {
	CHEM_SOLUBLE	= 1,
	CHEM_VOLATILE	= 2,
};

/// Properties of chemical containers.
enum {
	CHEM_ALLOW_SOLUBLE		= 1,
	CHEM_ALLOW_VOLATILE		= 2,
	CHEM_ALLOW_DIFFUSION	= 4,
};

/// Index of a chemical in the chemical list.
typedef unsigned char chem;

/// Definition of an allowed reaction.
typedef struct Reaction {
	chem reactant1;
	chem reactant2;
	chem catalyst;
	chem product;
	chem byproduct;
	int reactant1_ammount;
	int reactant2_ammount;
	int product_ammount;
	int byproduct_ammount;
	int rate;
} Reaction;

/// Container of chemicals.
typedef struct ChemContainer {
	int properties;
	int chems[CHEM_LAST];
} ChemContainer;

ChemContainer* goetia_chemical_MakeContainer();
ChemContainer* goetia_chemical_ListToContainer(List* list);
void goetia_chemical_UpdateContainer(ChemContainer* container);
int goetia_chemical_React(ChemContainer* container, chem chem1, chem chem2, chem prod);
int goetia_chemical_Add(ChemContainer* container, chem chemical, int ammount);
int goetia_chemical_Remove(ChemContainer* container, chem chemical, int ammount);
int goetia_chemical_Property(chem chemical);
void goetia_chemical_Diffuse(ChemContainer* c1, ChemContainer* c2);
void goetia_chemical_PrintContainer(ChemContainer* container);

void goetia_chemical_RegisterDefaultChemicals();
void goetia_chemical_RegisterDefaultReactions();
chem goetia_chemical_Register(atom chemical_name, int properties);
void goetia_chemical_RegisterReaction(Reaction reaction, bool auto_reaction);

atom goetia_chemical_ChemicalToAtom(chem chemical);
chem goetia_chemical_AtomToChemical(atom atom);
#endif // CHEMICAL_H