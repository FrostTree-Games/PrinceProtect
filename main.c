/* Whimsy Block Go
 * ---------------
 * A game for Windows written by Daniel Savage.
 *
 * For noncommercial use only.
 */


#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 448

SDL_Surface* screen;

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
	
	return 0;
}

int deinit()
{
	SDL_Quit();
	
	return 0;
}

int testLoop()
{
	return 0;
}

int main(int argc, char* argv[])
{
	init();
	
	testLoop();
	
	deinit();

	return 0;
}

