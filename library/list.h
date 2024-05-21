// LIST.H - libgoetia
// Linked-list definition, functions, s-expression parsing.

#ifndef LIST_H
#define LIST_H

#include <value.h>

enum {
	LIST_GARBAGE_COLLECT = 1,
};

typedef int list_flags;

typedef struct List {
	Value data;
	struct List* next;
	list_flags flags;
	short file;
	short line;
} List;

List* goetia_ParseListFromFile(const char* filename);
List* goetia_ParseList(const char* string);
void goetia_WriteListToFile(const char* filename, List* list);
void goetia_PrintList(List* list);
void goetia_PrintListShallow(List* list);
List* goetia_NewListElement();
const char* goetia_GetListFilename(short index);

#endif // LIST_H