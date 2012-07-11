/*
 * Pushdown.h
 * --------
 * This header file contains definitions for the functionality that controls the pushdown
 * notifications used in-game. Messages can be queued into the system and updated. Rendering 
 * of these messages is handled the draw module though. Each message length is max 31 characters.
 */
 
#ifndef PUSHDOWN_H
#define PUSHDOWN_H
 
#include <SDL/SDL.h>
 
typedef struct
{
	Uint32 startTime;
	char message[32];
} PushMessage;
 
/* updatePushMessages()
 * Purpose: Essentially gets rid of any old messages that need to die; call every loop
 * Returns: N/A
 */
void updatePushMessages();

/* pushNewMessage()
 * Purpose: Attempts to push a new message to the system; if the supplied text is over 50 characters in length it may be cut off
 * Returns: 0 on success, 1 if the queue is full, 2 on some other failure; output MAY be written to stderr
 */
int pushNewMessage(char* text);

/* getCurrentMessage()
 * Purpose: Gets the latest message; usually for the draw module
 * Returns: Pointer to current message in system, NULL if no current message to push
 */
PushMessage* getCurrentMessage();


#endif
