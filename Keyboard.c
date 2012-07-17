#include <stdio.h>

#include <SDL/SDL.h>

#include "Keyboard.h"

Uint8* keyList = NULL;

SDLKey p1_up = SDLK_UP;
SDLKey p1_down = SDLK_DOWN;
SDLKey p1_left = SDLK_LEFT;
SDLKey p1_right = SDLK_RIGHT;
SDLKey p1_a = SDLK_n;
SDLKey p1_b = SDLK_m;
SDLKey p1_select = SDLK_RETURN;
SDLKey p1_start = SDLK_BACKSPACE;

SDLKey p2_up = SDLK_w;
SDLKey p2_down = SDLK_s;
SDLKey p2_left = SDLK_a;
SDLKey p2_right = SDLK_d;
SDLKey p2_a = SDLK_r;
SDLKey p2_b = SDLK_t;
SDLKey p2_select = SDLK_y;
SDLKey p2_start = SDLK_u;

void pollKeyboard()
{
	keyList = SDL_GetKeyState(NULL);
}

void setKey(SDLKey input, gameKey key)
{
	switch (key)
	{
		case P1_UP:
		p1_up = input;
		break;
		case P1_DOWN:
		p1_down = input;
		break;
		case P1_LEFT:
		p1_left = input;
		break;
		case P1_RIGHT:
		p1_right = input;
		break;
		case P1_A:
		p1_a = input;
		break;
		case P1_B:
		p1_b = input;
		break;
		case P1_START:
		p1_start = input;
		break;
		case P1_SELECT:
		p1_select = input;
		break;
		case P2_UP:
		p2_up = input;
		break;
		case P2_DOWN:
		p2_down = input;
		break;
		case P2_LEFT:
		p2_left = input;
		break;
		case P2_RIGHT:
		p2_right = input;
		break;
		case P2_A:
		p2_a = input;
		break;
		case P2_B:
		p2_b = input;
		break;
		case P2_START:
		p2_start = input;
		break;
		case P2_SELECT:
		p2_select = input;
		break;
		default:
		break;
	}
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
		return keyList[p1_up];
		break;
		case P1_DOWN:
		return keyList[p1_down];
		break;
		case P1_LEFT:
		return keyList[p1_left];
		break;
		case P1_RIGHT:
		return keyList[p1_right];
		break;
		case P1_A:
		return keyList[p1_a];
		break;
		case P1_B:
		return keyList[p1_b];
		break;
		case P1_START:
		return keyList[p1_start];
		break;
		case P1_SELECT:
		return keyList[p1_select];
		break;
		case P2_UP:
		return keyList[p2_up];
		break;
		case P2_DOWN:
		return keyList[p2_down];
		break;
		case P2_LEFT:
		return keyList[p2_left];
		break;
		case P2_RIGHT:
		return keyList[p2_right];
		break;
		case P2_A:
		return keyList[p2_a];
		break;
		case P2_B:
		return keyList[p2_b];
		break;
		case P2_START:
		return keyList[p2_start];
		break;
		case P2_SELECT:
		return keyList[p2_select];
		break;
		default:
		printf("STRANGE KEYBOARD CODE FOUND\n");
		return 0;
	}
	return 0;
}

SDLKey getSDLKeyValue(gameKey key)
{
	//if they keyboard hasn't been polled, don't bother
	if (keyList == NULL)
	{
		return 0;
	}
	
	switch (key)
	{
		case ESCAPE_KEY:
		return SDLK_ESCAPE;
		break;
		case P1_UP:
		return p1_up;
		break;
		case P1_DOWN:
		return p1_down;
		break;
		case P1_LEFT:
		return p1_left;
		break;
		case P1_RIGHT:
		return p1_right;
		break;
		case P1_A:
		return p1_a;
		break;
		case P1_B:
		return p1_b;
		break;
		case P1_START:
		return p1_start;
		break;
		case P1_SELECT:
		return p1_select;
		break;
		case P2_UP:
		return p2_up;
		break;
		case P2_DOWN:
		return p2_down;
		break;
		case P2_LEFT:
		return p2_left;
		break;
		case P2_RIGHT:
		return p2_right;
		break;
		case P2_A:
		return p2_a;
		break;
		case P2_B:
		return p2_b;
		break;
		case P2_START:
		return p2_start;
		break;
		case P2_SELECT:
		return p2_select;
		break;
		default:
		printf("STRANGE KEYBOARD CODE FOUND\n");
		return 0;
	}
	return 0;
}