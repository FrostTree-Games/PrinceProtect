#include <SDL/SDL.h>

#include "Draw.h"
#include "Keyboard.h"
#include "GameScreens.h"

int keyConfigScreen(SDL_Surface* screen)
{
	int hardCoreQuit = 0;
	
	int menuPosition = 2; //highlights selected menu: 0 -> configP1, 1 -> configP2, 2 -> back button
	int quit = 0;
	
	int leftKeyDown = 0;
	int rightKeyDown = 0;
	int aKeyDown = 0;

	SDL_Event ev;
	SDL_Surface* buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	Uint32 timeStamp = SDL_GetTicks();

	while(quit == 0 && hardCoreQuit == 0)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				hardCoreQuit = 1;
			}
		}
		
		pollKeyboard();

		if (getKey(P1_LEFT) && leftKeyDown == 0)
		{
			leftKeyDown = 1;
		}
		else if (!getKey(P1_LEFT) && leftKeyDown == 1)
		{
			if (menuPosition > 0)
			{
				menuPosition -= 1;
			}

			leftKeyDown = 0;
		}

		if (getKey(P1_RIGHT) && rightKeyDown == 0)
		{
			rightKeyDown = 1;
		}
		else if (!getKey(P1_RIGHT) && rightKeyDown == 1)
		{
			if (menuPosition < 2)
			{
				menuPosition += 1;
			}

			rightKeyDown = 0;
		}

		if (getKey(P1_A) && aKeyDown == 0)
		{
			aKeyDown = 1;
		}
		else if (!getKey(P1_A) && aKeyDown == 1)
		{
			if (menuPosition == 2)
			{
				quit = 1;
			}
			else if (menuPosition == 0)
			{
				int i = 0;
				SDL_Event check;
				
				//this is a disgusting loop-inside a loop // no regrets
				while (i < 6)
				{
					while (SDL_PollEvent(&check))
					{
						if (check.type == SDL_QUIT)
						{
							hardCoreQuit = 1;
							i = 6;
						}
						else if (check.type == SDL_KEYUP)
						{
							if (check.key.keysym.sym == SDLK_ESCAPE)
							{
								i = 6;
								continue;
							}
							else
							{
								switch (i)
								{
									case 0:
									setKey(check.key.keysym.sym, P1_UP);
									break;
									case 1:
									setKey(check.key.keysym.sym, P1_DOWN);
									break;
									case 2:
									setKey(check.key.keysym.sym, P1_LEFT);
									break;
									case 3:
									setKey(check.key.keysym.sym, P1_RIGHT);
									break;
									case 4:
									setKey(check.key.keysym.sym, P1_A);
									break;
									case 5:
									setKey(check.key.keysym.sym, P1_B);
									break;
									default:
									break;
								}
								
								i++;
							}
						}
					}

					drawKeyConfigScreen(buffer, menuPosition, i);

					SDL_SoftStretch(buffer, NULL, screen, NULL);
					SDL_Flip(screen);
					SDL_Delay(17);
				}
			}
			else if (menuPosition == 1)
			{
				int i = 0;
				SDL_Event check;
				
				//this is a disgusting loop-inside a loop // no regrets
				while (i < 6)
				{
					while (SDL_PollEvent(&check))
					{
						if (check.type == SDL_QUIT)
						{
							hardCoreQuit = 1;
							i = 6;
						}
						else if (check.type == SDL_KEYUP)
						{
							if (check.key.keysym.sym == SDLK_ESCAPE)
							{
								i = 6;
								continue;
							}
							else
							{
								switch (i)
								{
									case 0:
									setKey(check.key.keysym.sym, P2_UP);
									break;
									case 1:
									setKey(check.key.keysym.sym, P2_DOWN);
									break;
									case 2:
									setKey(check.key.keysym.sym, P2_LEFT);
									break;
									case 3:
									setKey(check.key.keysym.sym, P2_RIGHT);
									break;
									case 4:
									setKey(check.key.keysym.sym, P2_A);
									break;
									case 5:
									setKey(check.key.keysym.sym, P2_B);
									break;
									default:
									break;
								}
								
								i++;
							}
						}
					}

					drawKeyConfigScreen(buffer, menuPosition, i + 10);

					SDL_SoftStretch(buffer, NULL, screen, NULL);
					SDL_Flip(screen);
					SDL_Delay(17);
				}
			}

			aKeyDown = 0;
		}

		drawKeyConfigScreen(buffer, menuPosition, -1);

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

