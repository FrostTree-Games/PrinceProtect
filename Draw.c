#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Draw.h"
#include "Entity.h"

void drawGameBlock(SDL_Surface* buffer, GameBlock* gb)
{
	SDL_Rect entRect = {gb->x * 16, gb->y * 16, 16, 16};

	switch (gb->bType)
	{
		case RED_BLOCK:
		SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 0, 0));
		break;
		case BLUE_BLOCK:
		SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 10, 10, 255));
		break;
		case GREEN_BLOCK:
		SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 10, 255, 10));
		break;
		case YELLOW_BLOCK:
		SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 0));
		break;
		default:
		printf("No block encountered?\n");
		break;
	}
}

void testDraw(SDL_Surface* buffer)
{
	int i;

	SDL_Rect r1 = {0, 0, 1, SCREEN_HEIGHT};
	SDL_Rect r2 = {0, 0, SCREEN_WIDTH, 1};
	
	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 100, 100));

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
		entRect.w = 16;
		entRect.h = 16;

		switch (entList[i]->type)
		{
			case PLAYER1:
			entRect.x = entList[i]->player.x * 16;
			entRect.y = entList[i]->player.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 0, 255));
			break;
			case PLAYER2:
			break;
			case PERMABLOCK:
			entRect.x = entList[i]->permaBlock.x * 16;
			entRect.y = entList[i]->permaBlock.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 100, 100, 100));
			break;
			case GAMEBLOCK:
			drawGameBlock(buffer, (GameBlock*)entList[i]);
			break;
			case ICEBLOCK:
			entRect.x = entList[i]->iBlock.x * 16 + entList[i]->iBlock.offsetX - 8;
			entRect.y = entList[i]->iBlock.y * 16 + entList[i]->iBlock.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 0, 255, 255));
			break;
			case EXPLOSION:
			entRect.x = 2 + (entList[i]->exp.x * 16);
			entRect.y = 2 + (entList[i]->exp.y * 16);
			entRect.w = 12;
			entRect.h = 12;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 40, 50));
			break;
			case TELEBLOCK:
			entRect.w = 4;
			entRect.x = 6 + entList[i]->tBlock.x * 16;
			entRect.y = entList[i]->tBlock.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 100, 255, 0));
			break;
			case ENEMY_CRAWLER:
			entRect.x = (entList[i]->enemy.x * 16) + entList[i]->enemy.offsetX - 8;
			entRect.y = (entList[i]->enemy.y * 16) + entList[i]->enemy.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 100, 5));
			break;
			default:
			break;
		}
	}
}

