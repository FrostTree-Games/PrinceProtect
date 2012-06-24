#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "Draw.h"
#include "Entity.h"
#include "Pushdown.h"
#include "GameLogic.h"

int pushNotificationFontSize = 14;
TTF_Font* pushNotificationFont = NULL;

int setupFonts()
{
	if (TTF_WasInit() != 1)
	{
		fprintf(stderr, "Check that SDL_ttf was initalized\n");
		return 1;
	}

	if ((pushNotificationFont = TTF_OpenFont("ttf/push.ttf", pushNotificationFontSize)) == NULL)
	{
		return 1;
	}

	return 0;
}

void clearFonts()
{
	TTF_CloseFont(pushNotificationFont);
}

void drawLatestPushDown(SDL_Surface* buffer)
{
	PushMessage* latestMessage = getCurrentMessage();
	if (latestMessage == NULL)
	{
		return;
	}

	SDL_Surface* text_surface;
	SDL_Color cl = {255, 255, 0, 0};
	text_surface = TTF_RenderText_Solid(pushNotificationFont, latestMessage->message, cl);
	
	SDL_Rect msgPos = {(buffer->w - text_surface->w)/2, 30, 0, 0};
	
	if (getTimeSingleton() - latestMessage->startTime < 2000)
	{
		msgPos.y = (Sint16)(30 * (getTimeSingleton() - latestMessage->startTime)/2000.0);
	}

	SDL_BlitSurface(text_surface, NULL, buffer, &msgPos);
	SDL_FreeSurface(text_surface);
}

void drawHealthScores(SDL_Surface* buffer)
{
	char p1HealthText[50];
	sprintf(p1HealthText, "P1 HP: %.2d/%.2d", getPlayerHealth(1), getPlayerMaxHealth(1));

	SDL_Surface* health_surface;
	SDL_Color cl = {255, 200, 25, 0};
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	
	SDL_Rect p1HpPos = {10, 10, 0, 0};
	
	SDL_BlitSurface(health_surface, NULL, buffer, &p1HpPos);
	SDL_FreeSurface(health_surface);
}

void drawGameBlock(SDL_Surface* buffer, GameBlock* gb)
{
	SDL_Rect entRect = {gb->x * 16, gb->y * 16 - (int)(((float)gb->height/100) * (gb->y * 16)), 16, 16};
	
	// this creates flickering for the last second of the Gameblock's life
	if (getTimeSingleton() - gb->startTime > 9 * 1000)
	{
		if ( ((getTimeSingleton() - gb->startTime - (9 * 1000)) / 100) % 2 == 0)
		{
			return;
		}
	}

	if (gb->height > 0)
	{
		SDL_Rect shadowRect= {(gb->x * 16) + 4, (gb->y * 16) + 4, 8, 8};
		SDL_FillRect(buffer, &shadowRect, SDL_MapRGB(buffer->format, 5, 5, 5));
	}

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
	SDL_Rect altRect = {0, 0, 4, 4};

	for (i = 0; i < getEntityListSize(); i++)
	{
		entRect.w = 16;
		entRect.h = 16;

		switch (entList[i]->type)
		{
			case PLAYER1:
			entRect.x = entList[i]->player.x * 16;
			entRect.y = entList[i]->player.y * 16;
			altRect.w = 8;
			altRect.h = 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 0, 255));

			//draw sword if necessary
			if (entList[i]->player.isThrusting == 1)
			{
				switch (entList[i]->player.direction)
				{
					case 0:
					altRect.x = (entList[i]->player.x * 16) + 4;
                                        altRect.y = (entList[i]->player.y * 16) - 8;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 200, 200, 210));
					break;
                                        case 1:
                                        altRect.x = (entList[i]->player.x * 16) + 16;
                                        altRect.y = (entList[i]->player.y * 16) + 4;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 200, 200, 210));
					break;
					case 2:
					altRect.x = (entList[i]->player.x * 16) + 4;
                                        altRect.y = (entList[i]->player.y * 16) + 16;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 200, 200, 210));
					break;
					case 3:
					altRect.x = (entList[i]->player.x * 16) - 8;
                                        altRect.y = (entList[i]->player.y * 16) + 4;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 200, 200, 210));
					break;
					default:
					break;
				}
			}
			
			entRect.x = (entList[i]->player.x * 16);
			entRect.y = (entList[i]->player.y * 16) - 8;
			
			if (entList[i]->player.holding != NULL)
			{
				switch (entList[i]->player.holding->bType)
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
			case LASER:
			entRect.x = 2 + (entList[i]->laser.x * 16) + entList[i]->laser.offsetX - 8;
			entRect.y = 6 + (entList[i]->laser.y * 16) + entList[i]->laser.offsetY - 8;
			entRect.w = 12;
			entRect.h = 12;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 0, 255, 50));
			break;
			case TELEBLOCK:
			entRect.w = 4;
			entRect.x = 6 + entList[i]->tBlock.x * 16;
			entRect.y = entList[i]->tBlock.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 100, 255, 0));
			break;
			case ICECREAM:
			entRect.x = entList[i]->iceCream.x * 16;
			entRect.y = entList[i]->iceCream.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 240));
			break;
			case ENEMY_CRAWLER:
			entRect.x = (entList[i]->enemy.x * 16) + entList[i]->enemy.offsetX - 8;
			entRect.y = (entList[i]->enemy.y * 16) + entList[i]->enemy.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 100, 5));
			if (entList[i]->enemy.cream != NULL)
			{
				entRect.y -= 16;
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 240));
			}
			break;
			case ENEMY_SHOOTER:
			entRect.x = (entList[i]->enemy.x * 16) + entList[i]->enemy.offsetX - 8;
			entRect.y = (entList[i]->enemy.y * 16) + entList[i]->enemy.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 100, 255, 75));
			if (entList[i]->enemy.cream != NULL)
			{
				entRect.y -= 16;
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 240));
			}
			break;
			default:
			break;
		}
	}
	
	drawHealthScores(buffer);
	
	drawLatestPushDown(buffer);
}

