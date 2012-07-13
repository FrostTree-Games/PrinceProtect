#include <SDL/SDL.h>

#include "Draw.h"
#include "Keyboard.h"
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

int titleScreen(SDL_Surface* screen)
{
	int hardCoreQuit = 0;

	SDL_Event ev;
	SDL_Surface* buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	int menuPosition = 0; //0 up to 4, inclusive
	int menuSelected = 255;
	
	int upKeyDown = 0;
	int downKeyDown = 0;
	int aKeyDown = 0;

	while(hardCoreQuit == 0 && menuSelected == 255)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				hardCoreQuit = 1;
			}
		}
		
		pollKeyboard();

		if (getKey(P1_UP) && upKeyDown == 0)
		{
			upKeyDown = 1;
		}
		else if (!getKey(P1_UP) && upKeyDown == 1)
		{
			menuPosition -= 1;
			if (menuPosition < 0)
			{
				menuPosition = 4;
			}
			upKeyDown = 0;
		}

		if (getKey(P1_DOWN) && downKeyDown == 0)
		{
			downKeyDown = 1;
		}
		else if (!getKey(P1_DOWN) && downKeyDown == 1)
		{
			menuPosition = (menuPosition + 1) % 5;
			downKeyDown = 0;
		}

		if (getKey(P1_A) && aKeyDown == 0)
		{
			aKeyDown = 1;
		}
		else if (!getKey(P1_A) && aKeyDown == 1)
		{
			switch (menuPosition)
			{
				case 0:
				menuSelected = 1;
				break;
				case 1:
				menuSelected = 2;
				break;
				case 2:
				menuSelected = 3;
				break;
				case 3:
				menuSelected = 4;
				break;
				case 4:
				menuSelected = 0;
				break;
				default:
				menuPosition = 0;
				break;
			}
			aKeyDown = 0;
		}
		
		drawTitleScreen(buffer, menuPosition);

		SDL_SoftStretch(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(17);
	}

	SDL_FreeSurface(buffer);
	
	if (hardCoreQuit == 1)
	{
		return -1;
	}

	return menuSelected;
}

