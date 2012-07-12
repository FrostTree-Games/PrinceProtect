#include <string.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Pushdown.h"
#include "Entity.h" //When time singleton is in a better module, include that instead
#include "Keyboard.h"

PushMessage messageQueue[5];
int queueRear = -1; //-1 means the queue is empty

/* For now, this Pushdown.h implementation uses a stupid non-circuar queue that copies everything forward.
 * I hope to rewrite it later, but if I haven't I'm not too worried. Whimsy Block Go is too small of a game
 * for the performance to matter and it's hidden nicely by a good interface. I get brownie points for the
 * latter, right?
 */

void updatePushMessages()
{
	int i;
	
	if (queueRear < 0)
	{
		return;
	}

	if (getTimeSingleton() - messageQueue[0].startTime > 5000)
	{
		for (i = 0; i < queueRear; i++)
		{
			messageQueue[i] = messageQueue[i+1];
		}
		
		queueRear--;
		
		if (queueRear > -1)
		{
			messageQueue[0].startTime = getTimeSingleton();
		}
	}
}

int pushNewMessage(char* text)
{
	if (text == NULL)
	{
		return 2;
	}
	
	if (queueRear > 3)
	{
		return 1;
	}
	
	queueRear++;
	
	if (strlen(text) > 31)
	{
		strncpy(messageQueue[queueRear].message, text, 31);
		(messageQueue[queueRear].message)[31] = '\0';
	}
	else
	{
		strcpy(messageQueue[queueRear].message, text);
	}
	
	messageQueue[queueRear].startTime = getTimeSingleton();

	return 0;
}

PushMessage* getCurrentMessage()
{
	if (queueRear < 0)
	{
		return NULL;
	}
	else
	{
		return messageQueue;
	}
}
