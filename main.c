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

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 448

SDL_Surface* screen;
SDL_Surface* buffer;

Entity* testEn;

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
	
	//remove this code later
	testEn = malloc(sizeof(Entity));
	testEn->playerEntity.type = PLAYER1;
	testEn->playerEntity.x = 4;
	testEn->playerEntity.y = 3;
	testEn->playerEntity.width = 1;
	testEn->playerEntity.height = 1;

	return 0;
}

int deinit()
{
	free(testEn);

	SDL_FreeSurface(buffer);

	SDL_Quit();
	
	return 0;
}

int draw()
{
	int i;

	SDL_Rect r1 = {0, 0, 1, SCREEN_HEIGHT};
	SDL_Rect r2 = {0, 0, SCREEN_WIDTH, 1};
	SDL_Rect pR = {testEn->playerEntity.x * 16, testEn->playerEntity.y * 16, testEn->playerEntity.width * 16, testEn->playerEntity.height * 16};
	
	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 255, 0));
	
	SDL_FillRect(buffer, &pR, SDL_MapRGB(buffer->format, 255, 0, 0));

	for (i = 0; i < SCREEN_WIDTH / 16; i++)
	{
		SDL_FillRect(buffer, &r1, SDL_MapRGB(buffer->format, 255, 255, 255));
		r1.x += 16;
	}
	
	for (i = 0; i < SCREEN_HEIGHT / 16; i++)
	{
		SDL_FillRect(buffer, &r2, SDL_MapRGB(buffer->format, 255, 255, 255));
		r2.y += 16;
	}

	return 0;
}

int testLoop()
{
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

		draw();
		
		SDL_BlitSurface(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(20);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	init();
	
	testLoop();
	
	deinit();

	return 0;
}

