// MESSAGE.H - libgoetia
// Message definition.

#ifndef MESSAGE_H
#define MESSAGE_H

#include <list.h>

typedef struct Message {
	int sender;
	int receiver;
	List* message;
} Stimulus;

void goetia_message_Emit(Message message);
void goetia_message_Process();

#endif // MESSAGE_H