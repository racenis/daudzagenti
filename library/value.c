// VALUE.H - libgoetia
// Value function, conversion function implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <list.h>

#include <value.h>

static const int MAX_ATOMS = 400;

static char* atom_names[400] = {
	"null",
	"ok",
	"error",
	"true",
	"false",
	"==",
	">",
	"<",
	"and",
	"or",
	"not",
	"+",
	"-",
	"*",
	"/",
	"print",
	"if",
	"set",
	"quote",
	"list",
	"eval",
	"apply",
	"lambda",
	"context",
	"do",
	"pattern",
	"...",
	"is",
	"reserved_28",
	"reserved_29",
	"reserved_30",
	"reserved_31",
	"reserved_32",
	"reserved_33",
	"reserved_34",
	"reserved_35",
	"reserved_36",
	"reserved_37",
	"reserved_38",
	"reserved_39",
	"_self",
	"_stim",
	"_subtype",
	"_subject",
	"_object",
	"_action",
	"_chem",
	"_ammount",
	"reserved_48",
	"reserved_49",	
	"id",
	"name",
	"energy",
	"entropy",
	"temperature",
	"position",
	"action",
	"chemical",
	"skeleton",
	"sensor",
	"init",
	"uninit",
	"tick",
	"stimulus",
	"category",
	"collision",
	"sphere",
	"cylinder",
	"cube",
	"mass",
	"reserved_70",
	"reserved_71",
	"reserved_72",
	"reserved_73",
	"reserved_74",
	"reserved_75",
	"reserved_76",
	"reserved_77",
	"reserved_78",
	"reserved_79",
	"reserved_80",
	"reserved_81",
	"reserved_82",
	"reserved_83",
	"reserved_84",
	"reserved_85",
	"reserved_86",
	"reserved_87",
	"reserved_88",
	"reserved_89",
	"push",
	"pull",
	"deactivate",
	"eat",
	"pickup",
	"lookat",
	"rest",
	"reserved_97",
	"reserved_98",
	"reserved_99",
	"creature",
	"machine",
	"manmachine",
	"root",
	"fruit",
	"nut",
	"insect",
	"critter",
	"geat",
	"detritus",
	"reserved_110",
	"reserved_111",
	"reserved_112",
	"reserved_113",
	"reserved_114",
	"reserved_115",
	"reserved_116",
	"reserved_117",
	"reserved_118",
	"reserved_119",
	"reserved_120",
	"reserved_121",
	"reserved_122",
	"reserved_123",
	"reserved_124",
	"reserved_125",
	"reserved_126",
	"reserved_127",
	"reserved_128",
	"reserved_129",
	"visual",
	"sound",
	"smell",
	"touch",
	"appear",
	"leave",
	"move",
	"inject",
	"ingest",
	"perform",
	"react",
	"eject",
	"offer",
	"kill",
	"anime",
	"light",
	"water",
	"sugar",
	"starch",
	"fat",
	"fiber",
	"amino-acid",
	"ammonia",
	"oxygen",
	"carbon-dioxide",
	"sugar-oxide",
	"urea",
	"bone-mineral",
	"asbestos",
	"acid",
	"heavy-metal",
	"mercury",
	"lead",
	"cyanide",
	"cholesterol",
	"starch-synthase",
	"starch-breakdownase",
	"fiber-synthase",
	"fiber-breakdownase",
	"amino-acid-synthase",
	"amino-acid-breakdownase",
	"stem-tissue",
	"root-tissue",
	"leaf-tissue",
	"plant-organ-synthase",
	"plant-organ-breakdownase",
	"milk-sugar",
	"milk-sugar-synthase",
	"milk-sugar-breakdownase",
	"lung-tissue",
	"intestine-tissue",
	"eye-tissue",
	"ear-tissue",
	"nose-tissue",
	"kidney-tissue",
	"muscle-tissue",
	"throat-tissue",
	"bone-tissue",
	"heart-tissue",
	"liver-tissue",
	"milk-tissue",
	"sex-tissue",
	"brain-tissue",
	"nerve-tissue",
	"input-organ-synthase",
	"output-organ-synthase",
	"metabolism-organ-synthase",
	"regulatory-organ-synthase",
	"animal-organ-breakdownase",
	"sector",
	"wall",
	"x",
	"y",
	"z",
	"y-max",
	"y-min",
	"set-model",
	"flags",
	"animal",
	"plant",
	"fungus",
	"mechanical",
	"immortal",
	"disallow-pickup",
	"dynamics-asleep",
	"dynamics-disable",
	"get",
	"drop",
	"hit",
	"approach",
	"retreat",
	"left",
	"right",
	"up",
	"down",
	"forward",
	"backward",
	"agent",
	"wallbonk",
	"headbonk",
	"floorbonk",
	"random",
	"probe",
	"measure",
	"speed",
	"rotation",
	"lobe",
	"make",
	"input",
	"output",
	"hidden",
	"class",
	"vector",
	"scalar",
	"connect",
	"weight",
	"reset",
	"head",
	"tail",
	"size",
	"pop",
	"last",
	"set-scale",
	"set-color",
	"set-animation",
	"air",
	"description",
	"all-energy",
	"all-sensor",
	"float-in-air",
	"float-in-water",
	"instantiate",
	"at"
};
static int atom_count = ATOM_LAST_ATOM;

/// Converts a string to an atom.
/// Either stores the string in the string table, or retrieves the stored atom.
/// @return Atom.
atom goetia_atom_FromString(const char* string) {
	// TODO: switch this to a hash map
	for (int i = 0; i < atom_count; i++) {
		if (strcmp(atom_names[i], string) == 0) {
			return i;
		}
	}

	atom_names[atom_count] = malloc(strlen(string) + 1);
	strcpy(atom_names[atom_count], string);
	
	return atom_count++;
}

/// Retrieves an atom's string representation from the string table.
/// @return The string from which the atom was created.
const char* goetia_atom_ToString(atom atom) {
	if (atom < 0 || atom >= atom_count) {
		return NULL;
	}
	
	return atom_names[atom];
}

/// Debug prints a value.
void goetia_value_Print(Value value) {
	switch (value.type) {
		case VALUE_ATOM:
			printf("%s", goetia_atom_ToString(value.atom)); break;
		case VALUE_INT:
			printf("%i", value.int32); break;
		case VALUE_FLOAT:
			printf("%f", value.float32); break;
		case VALUE_STRING:
			printf("\"%s\"", value.data); break;
		case VALUE_LIST:
			goetia_PrintListShallow(value.list); break;
		default:
			printf("(...)");
	}
}

/// Creates either a float, or an int value.
/// If the floating-point number is already rounded, it will return the int
/// representation of it, otherwise it will return floating point value.
/// @return Numeric value. 
Value goetia_value_MakeNumeric(float value) {
	Value val;
	if (floorf(value) == value) {
		val.int32 = value;
		val.type = VALUE_INT;
	} else {
		val.float32 = value;
		val.type = VALUE_FLOAT;
	}
	return val;
}

/// Converts a value type enum to a string.
const char* goetia_value_TypeToString(int type) {
	switch (type) {
		case VALUE_ATOM: 	return "atom";
		case VALUE_INT: 	return "int";
		case VALUE_FLOAT: 	return "float";
		case VALUE_STRING: 	return "string";
		case VALUE_LIST: 	return "list";
	}
}

// TODO: switch this to using bool
/// Checks if values are identical.
int goetia_value_Compare(Value v1, Value v2) {
	if (v1.type != v2.type) return 0;
	switch (v1.type) {
		case VALUE_ATOM: 	return v1.atom == v2.atom;
		case VALUE_INT: 	return v1.int32 == v2.int32;
		case VALUE_FLOAT: 	return v1.float32 == v2.float32;
		case VALUE_STRING: 	return strcmp(v1.str, v2.str) == 0;
		case VALUE_LIST: 	abort();// TODO: implement
	}
}

/// Wraps an atom into a value.
Value goetia_value_MakeAtom(atom atom) {
	Value val;
	val.atom = atom;
	val.type = VALUE_ATOM;
	return val;
}

/// Wraps an int into a value.
Value goetia_value_MakeInt(int int32) {
	Value val;
	val.int32 = int32;
	val.type = VALUE_INT;
	return val;
}

/// Wraps a float into a value.
Value goetia_value_MakeFloat(float float32) {
	Value val;
	val.float32 = float32;
	val.type = VALUE_FLOAT;
	return val;
}

/// Wraps a string into a value.
Value goetia_value_MakeString(const char* string) {
	Value val;
	val.str = string;
	val.type = VALUE_STRING;
	return val;
}