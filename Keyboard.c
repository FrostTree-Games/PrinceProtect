#include <stdio.h>

#include <SDL/SDL.h>

#include "Keyboard.h"

Uint8* keyList = NULL;

void pollKeyboard()
{
	keyList = SDL_GetKeyState(NULL);
}

int getKey(gameKey key)
{
	//if they keyboard hasn't been polled, don't bother
	if (keyList == NULL)
	{
		return 0;
	}
	
	switch (key)
	{
		case ESCAPE_KEY:
		return keyList[SDLK_ESCAPE];
		break;
		case P1_UP:
		return keyList[SDLK_UP];
		break;
		case P1_DOWN:
		return keyList[SDLK_DOWN];
		break;
		case P1_LEFT:
		return keyList[SDLK_LEFT];
		break;
		case P1_RIGHT:
		return keyList[SDLK_RIGHT];
		break;
		case P1_A:
		return keyList[SDLK_a];
		break;
		case P1_B:
		return keyList[SDLK_s];
		break;
		case P1_START:
		return keyList[SDLK_RETURN];
		break;
		case P1_SELECT:
		return keyList[SDLK_BACKSPACE];
		break;
		case P2_UP:
		case P2_DOWN:
		case P2_LEFT:
		case P2_RIGHT:
		case P2_A:
		case P2_B:
		case P2_START:
		case P2_SELECT:
		printf("No P2 code written yet\n");
		default:
		return 0;
	}
	return 0;
}
