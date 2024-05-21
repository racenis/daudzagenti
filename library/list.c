// LIST.C - libgoetia
// Linked-list definition, functions, s-expression parsing implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <list.h>

static char* file_names[100];
static short file_count = 1;

const char* goetia_GetListFilename(short index) {
	return file_names[index];
}

static List* parse_list(const char* string, short file_index);

/// Parses a list from file.
/// This will read in the file specified in the filename, then use
/// goetia_ParseList() to parse the list.
/// @return Pointer to list or a NULL if file can't be read.
List* goetia_ParseListFromFile(const char* filename) {
	FILE* file = fopen(filename, "rb");
	
	if (!file) {
		printf("Can't open file: %s\n", filename);
		return NULL;
	}
	
	// save filename into global file list
	short file_index = file_count++;
	file_names[file_index] = malloc(strlen(filename)+1);
	strcpy(file_names[file_index], filename);
	
	fseek(file, 0, SEEK_END);
	int file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* contents = malloc(file_size + 1);
	fread(contents, file_size, 1, file);
	fclose(file);
	
	contents[file_size] = '\0';
	
	List* list = parse_list(contents, file_index);
	
	free(contents);
	
	return list;
}

/// Creates a new list element.
List* goetia_NewListElement() {
	List* list = malloc(sizeof(List));
	
	list->data.data = NULL;
	list->next = NULL;
	list->flags = 0;
	
	return list;
}

static List* parse_sexpr(const char** sexpr, short file_index, short* line) {
	// let's start with abbreviating the parameter
	const char* s = *sexpr;
	
	// then we can create pointers for the list
	List* list = NULL;
	List* list_first = NULL;
		
	// now we can start parsing in the atoms and values until we get to the end
	// of the expression
	while (1) {
		// skip over any whitespace
		while (isspace(*s)) *s++ == '\n' && (*line)++;
		
		// skip over comment
		if (*s == ';') {
			while(*s != '\n' && *s != '\0') s++;
			continue;
		}
		
		// if we're at the end of the s-expression, bail
		if (*s == ')' || *s == '\0') {
			break;
		}
		
		// now we can create a new list element
		List* new_list = goetia_NewListElement();

		new_list->file = file_index;
		new_list->line = *line;
		
		if (list) {
			list->next = new_list;
		}
		
		if (!list_first) {
			list_first = new_list;
		}

		list = new_list;

		// check if number
		if (isdigit(*s) || ((*s == '+' || *s == '-') && isdigit(*(s+1)))) {
			list->data = goetia_value_MakeNumeric(strtof(s, NULL));
			while (!isspace(*s) && *s != ')') s++;
		} else switch (*s) {
			// check if we have another s-expression
			case '(': {
				s++;
				list->data.list = parse_sexpr(&s, file_index, line);
				list->data.type = VALUE_LIST;
			} break;
			
			// check if we have a string
			case '"': {
				char buffer[100];
				char* ptr = buffer;
				
				// skip over first quote
				s++;
				
				// copy the string into buffer
				while (*s != '"') {
					// TODO: fix linebreak in string messing up line count
					*(ptr++) = *(s++);
				}
				
				// skip over last quote and append a null to the buffer
				*ptr = '\0';
				s++;
				
				// now we know how long the string is and we can allocate
				char* value = malloc(ptr - buffer + 1);
				strcpy(value, buffer);

				list->data.str = value;			
				list->data.type = VALUE_STRING;		
			} break;
			
			// otherwise we will assume that we have an atom
			default: {
				char buffer[100];
				char* ptr = buffer;
				
				while (!isspace(*s) && *s != ')') {
					*(ptr++) = *(s++);
				}
				
				*ptr = '\0';
				
				// now we know how long the string is and we can allocate
				list->data.atom = goetia_atom_FromString(buffer);
				list->data.type = VALUE_ATOM;
			}
		}
	}
	
	// skip over closing bracket, return parameter and return
	s++;
	
	*sexpr = s;
	
	return list_first;
}

static List* parse_list(const char* string, short file_index) {
	short line = 1;
	if (*string == '(') string++;
	return parse_sexpr(&string, file_index, &line);
}

/// Parses a list from a string.
/// This will parse the list from the string.
/// @return Pointer to list or a NULL if there was an error.
List* goetia_ParseList(const char* string) {
	return parse_list(string, 0);
}

void goetia_WriteListToFile(const char* filename, List* list);

/// Prints a list.
/// This will print a list to standard output.
void goetia_PrintList(List* list) {
	printf("(");
	do {
		if (list->data.type == VALUE_LIST) {
			goetia_PrintList(list->data.list);
		} else {
			goetia_value_Print(list->data);
		}
		list = list->next;
	} while (list && printf(" "));
	printf(")");
}

/// Prints a list, but without sublists.
/// This will print a list to standard output.
void goetia_PrintListShallow(List* list) {
	printf("(");
	do {
		goetia_value_Print(list->data);
		list = list->next;
	} while (list && printf(" "));
	printf(")");
}

//                                           miu mau                            
//                        \                   /                                 
//                         \          /\___/\                                   
//                          \_______<(O  .  0)>                                 
//                          /*              /                                   
//                         (________________)                                   
//                           |  |      |  |                                     
