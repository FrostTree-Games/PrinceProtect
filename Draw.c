#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Draw.h"
#include "Entity.h"

void testDraw(SDL_Surface* buffer)
{
	int i;

	SDL_Rect r1 = {0, 0, 1, SCREEN_HEIGHT};
	SDL_Rect r2 = {0, 0, SCREEN_WIDTH, 1};
	
	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 255, 0));

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
	
	Entity** entList = getEntityList();

	//used for default drawing
	SDL_Rect entRect = {0, 0, 16, 16};

	for (i = 0; i < getEntityListSize(); i++)
	{

		switch (entList[i]->type)
		{
			case PLAYER1:
			entRect.x = entList[i]->player.x * 16;
			entRect.y = entList[i]->player.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 0, 0, 255));
			break;
			case PLAYER2:
			break;
			case PERMABLOCK:
			entRect.x = entList[i]->permaBlock.x * 16;
			entRect.y = entList[i]->permaBlock.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 0, 0));
			break;
			default:
			break;
		}
	}
}

