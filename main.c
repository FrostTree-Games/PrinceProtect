/* Whimsy Block Go
 * ---------------
 * A game for Windows written by Daniel Savage.
 *
 * For noncommercial use only.
 */


#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Entity.h"
#include "Keyboard.h"
#include "Draw.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 448

SDL_Surface* screen;
SDL_Surface* buffer;

int init()
{
	// strangely prints stdout to stdout
	freopen( "CON", "w", stdout );

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		perror("Error initalizing SDL");
		return 1;
	}

	if ((screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE)) == NULL)
	{
		perror("Error initalizing screen");
		return 1;
	}
	
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	SDL_WM_SetCaption( "Whimsy Block Go", NULL );

	return 0;
}

int deinit()
{
	SDL_FreeSurface(buffer);

	SDL_Quit();
	
	return 0;
}

int testLoop()
{
	int i;

	int quit = 0;
	SDL_Event ev;

	while (!quit)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				quit = 1;
			}
		}
		
		//update call
		pollKeyboard();

		//test code for checking if entities are on a square

		Entity** entList = getEntityList();
		Uint32 currTime = SDL_GetTicks();
		for (i = 0; i < getEntityListSize(); i++)
		{
			update_entity(entList[i], currTime);
		}

		testDraw(buffer);
		
		SDL_BlitSurface(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(20);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int i;

	init();

	initEntityList();

	pushEntity(PLAYER1, 3, 19);

	for (i = 0; i < 32; i++)
	{
		pushEntity(PERMABLOCK, i, 8);
		pushEntity(PERMABLOCK, i, 24);
	}
	
	for (i = 0; i < 15; i++)
	{
		pushEntity(PERMABLOCK, -1, 9 + i);
		pushEntity(PERMABLOCK, 32, 9 + i);
	}

	testLoop();
	
	freeEntityList();
	
	deinit();

	return 0;
}

