#include <SDL/SDL.h>

#include "Draw.h"
#include "GameScreens.h"

int preambleSplashScreen(SDL_Surface* screen)
{
	int hardCoreQuit = 0;

	SDL_Event ev;
	SDL_Surface* buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	Uint32 timeStamp = SDL_GetTicks();

	while(SDL_GetTicks() - timeStamp < 9 * 1000 && hardCoreQuit == 0)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				hardCoreQuit = 1;
			}
		}

		if (SDL_GetTicks() - timeStamp < 1000)
		{
			drawDevScreen(buffer, -1);
		}
		else if (SDL_GetTicks() - timeStamp < 3500)
		{
			drawDevScreen(buffer, 0);
		}
		else if (SDL_GetTicks() - timeStamp < 4000)
		{
			drawDevScreen(buffer, -1);
		}
		else if (SDL_GetTicks() - timeStamp < 7000)
		{
			drawDevScreen(buffer, 1);
		}
		else if (SDL_GetTicks() - timeStamp < 7500)
		{
			drawDevScreen(buffer, -1);
		}
		
		SDL_SoftStretch(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(17);
	}
	
	SDL_FreeSurface(buffer);
	
	if (hardCoreQuit == 1)
	{
		return 1;
	}

	return 0;
}

