#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "Draw.h"
#include "Entity.h"
#include "Pushdown.h"
#include "GameLogic.h"

int pushNotificationFontSize = 10;
TTF_Font* pushNotificationFont = NULL;

SDL_Surface* tileSheet;

int setupAssets()
{
	if (TTF_WasInit() != 1)
	{
		fprintf(stderr, "Check that SDL_ttf was initalized\n");
		return 1;
	}
	
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		fprintf(stderr, "Check that SDL_image was initalized\n");
		return 1;
	}

	if ((pushNotificationFont = TTF_OpenFont("ttf/slkscr.ttf", pushNotificationFontSize)) == NULL)
	{
		return 1;
	}
	
	if ((tileSheet = IMG_Load("gfx/sheet.png")) == NULL)
	{
		return 1;
	}

	return 0;
}

void clearAssets()
{
	TTF_CloseFont(pushNotificationFont);
	
	SDL_FreeSurface(tileSheet);
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

	if (getTimeSingleton() - latestMessage->startTime < 500)
	{
		msgPos.y = (Sint16)(30 * (getTimeSingleton() - latestMessage->startTime)/500.0);
	}
	
	if (getTimeSingleton() - latestMessage->startTime >= 1000)
	{
		if ( ((getTimeSingleton() - latestMessage->startTime)/250) % 2 == 0)
		{
			SDL_BlitSurface(text_surface, NULL, buffer, &msgPos);
		}
	}
	else
	{
		SDL_BlitSurface(text_surface, NULL, buffer, &msgPos);
	}

	SDL_FreeSurface(text_surface);
}

void drawHealthScores(SDL_Surface* buffer)
{
	SDL_Color cl = {255, 255, 255, 0};

	char p1HealthText[50];
	sprintf(p1HealthText, "P1 HP: %.2d/%.2d", getPlayerHealth(1), getPlayerMaxHealth(1));

	SDL_Surface* health_surface;
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	
	SDL_Rect p1HpPos = {10, 5, 0, 0};

	SDL_BlitSurface(health_surface, NULL, buffer, &p1HpPos);
	SDL_FreeSurface(health_surface);

	sprintf(p1HealthText, "ICECREAM: %d", getIceCreamCount());
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect iceCreamNumbers = {10, 25, 0, 0};
	SDL_BlitSurface(health_surface, NULL, buffer, &iceCreamNumbers);
	SDL_FreeSurface(health_surface);
}

void drawGameBlock(SDL_Surface* buffer, GameBlock* gb)
{
	SDL_Rect tileRect = {0, 0, 16, 16};
	SDL_Rect entRect = {gb->x * 16, gb->y * 16 - (int)(((float)gb->height/100) * (gb->y * 16)), 16, 16};
	
	// this creates flickering for the last few second of the Gameblock's life
	if (getTimeSingleton() - gb->startTime > 26 * 1000)
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
		tileRect.x = 128;
		tileRect.y = 0;
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		break;
		case BLUE_BLOCK:
		tileRect.x = 144;
		tileRect.y = 0;
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		break;
		case GREEN_BLOCK:
		tileRect.x = 144;
		tileRect.y = 16;
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		break;
		case YELLOW_BLOCK:
		tileRect.x = 128;
		tileRect.y = 16;
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		break;
		default:
		printf("No block encountered?\n");
		break;
	}
}

void drawExplosion(SDL_Surface* buffer, Explosion* exp)
{
	SDL_Rect tileRect = {160, 32, 16, 16};
	SDL_Rect entRect = {exp->x * 16, exp->y * 16, 16, 16};
	
	Uint32 delta = getTimeSingleton() - exp->startTime;

	tileRect.x += (Sint16)( (Uint32)((delta/750.0) * 6) * 16 );

	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
}

void drawIceblock(SDL_Surface* buffer, IceBlock* ib)
{
	SDL_Rect tileRect = {160, 48, 16, 16};
	SDL_Rect entRect = {(ib->x * 16) + (ib->offsetX - 8), (ib->y * 16) + (ib->offsetY - 8), 16, 16};

	tileRect.x += (Sint16)( (3 - ib->health) * 16 );

	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
}

void drawTeleBlock(SDL_Surface* buffer, TeleBlock* tb)
{
	SDL_Rect tileRect = {208, 48, 16, 16};
	SDL_Rect entRect = {tb->x * 16, tb->y * 16, 16, 16};

	tileRect.x += (Sint16)( tb->frame * 16 );

	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
}

void drawPlayer1(SDL_Surface* buffer, Player* pl)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {0, 0, 16, 16};

	if (pl->isThrusting  == 0)
	{
		if (pl->holding != NULL)
		{
			SDL_Rect placementRect = {pl->x * 16, pl->y * 16, 0, 0};
	
			switch (pl->direction)
			{
				case 0:
				placementRect.y -= 16;
				break;
				case 1:
				placementRect.x += 16;
				break;
				case 2:
				placementRect.y += 16;
				break;
				case 3:
				placementRect.x -= 16;
				break;
				default:
				break;
			}
	
			placementRect.w = 16;
			placementRect.h = 1;
			SDL_FillRect(buffer, &placementRect, SDL_MapRGB(buffer->format, 0, 0, 0));
			placementRect.y += 16;
			SDL_FillRect(buffer, &placementRect, SDL_MapRGB(buffer->format, 0, 0, 0));
			placementRect.y -= 16;
	
			placementRect.w = 1;
			placementRect.h = 16;
			SDL_FillRect(buffer, &placementRect, SDL_MapRGB(buffer->format, 0, 0, 0));
			placementRect.x += 16;
			SDL_FillRect(buffer, &placementRect, SDL_MapRGB(buffer->format, 0, 0, 0));
		}
	
		entRect.x = (pl->x * 16) + pl->offsetX - 8;
		entRect.y = (pl->y * 16) + pl->offsetY - 8;
		
		if (pl->dead == 1)
		{
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 0));
			return;
		}
	
		switch (pl->direction)
		{
			case 0:
			tileRect.y += 16;
			break;
			case 1:
			tileRect.y += 32;
			break;
			case 2:
			tileRect.y += 0;
			break;
			case 3:
			tileRect.y += 48;
			break;
			default:
			break;
		}
	
		
		if (pl->frame == 1)
		{
			tileRect.x += 16;
		}
		
		if (pl->holding != NULL)
		{
			tileRect.x += 32;
		}
		
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		
		entRect.x = (pl->x * 16) + pl->offsetX - 8;
		entRect.y = (pl->y * 16) - 14 + pl->offsetY - 8;
		
		tileRect.x = 128;
		tileRect.y = 0;
		
		if (pl->holding != NULL)
		{
			switch (pl->holding->bType)
			{
				case RED_BLOCK:
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
				break;
				case BLUE_BLOCK:
				tileRect.x += 16;
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
				break;
				case GREEN_BLOCK:
				tileRect.x += 16;
				tileRect.y += 16;
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
				break;
				case YELLOW_BLOCK:
				tileRect.y += 16;
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
				break;
				default:
				printf("No block encountered?\n");
				break;
			}
		}
	}
	else
	{
		tileRect.x = 0;
		tileRect.y = 64;
		
		switch (pl->direction)
		{
			case 0:
			tileRect.y += 16;
			break;
			case 1:
			tileRect.y += 32;
			break;
			case 2:
			tileRect.y += 0;
			break;
			case 3:
			tileRect.y += 48;
			break;
			default:
			break;
		}
		
		if (getTimeSingleton() - pl->swordTimer > 75)
		{
			tileRect.x += 16;
		}
		
		entRect.x = (pl->x * 16) + pl->offsetX - 8;
		entRect.y = (pl->y * 16) + pl->offsetY - 8;
		
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		
		tileRect.y += 64;
		
		//draw sword/hammer
		switch (pl->direction)
		{
			case 0:
			entRect.y -= 16;
			break;
			case 1:
			entRect.x += 16;
			break;
			case 2:
			entRect.y += 16;
			break;
			case 3:
			entRect.x -= 16;
			break;
			default:
			break;
		}
		
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
}

void drawCrawler(SDL_Surface* buffer, Enemy* en)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {0, 0, 16, 16};
	
	tileRect.x = 32;
	tileRect.y = 128;
	
	entRect.x = (en->x * 16) + en->offsetX - 8;
	entRect.y = (en->y * 16) + en->offsetY - 8;
	
	if (en->knockBackDirection == 255)
	{
		if (en->cream != NULL)
		{
			tileRect.x += 32;
		}
		
		if (en->frame == 1)
		{
			tileRect.x += 16;
		}
		
		switch (en->direction)
		{
			case 0:
			tileRect.y += 16;
			break;
			case 1:
			tileRect.y += 32;
			break;
			case 2:
			tileRect.y += 0;
			break;
			case 3:
			tileRect.y += 48;
			break;
			default:
			break;
		}
	}
	else
	{
		tileRect.y += (((getTimeSingleton() - en->timer)/50 ) % 4) * 16;
	}
	
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	
	if (en->cream != NULL)
	{
		entRect.y -= 16;
		
		tileRect.x = 208;
		tileRect.y = 16;

		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
}

void testDraw(SDL_Surface* buffer)
{
	int i,j;

	Entity** entList = getEntityList();

	//used for default drawing
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect altRect = {0, 0, 4, 4};
	SDL_Rect tileRect = {0, 0, 16, 16};
	
	tileRect.x = 160;
	tileRect.y = 0;
	for (i = 0; i < SCREEN_WIDTH/16; i++)
	{
		for (j = 0; j < SCREEN_HEIGHT/16; j++)
		{
			if (j > BOARD_TOP_WALL && j < BOARD_BOTTOM_WALL)
			{
				tileRect.x = 160;
				tileRect.y = 0;
			}
			else
			{
				tileRect.x = 160;
				tileRect.y = 16;
			}

			entRect.x = i*16;
			entRect.y = j*16;
			
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		}
	}
	
	entRect.x = 0;
	entRect.y = (BOARD_TOP_WALL + 1) * 16;
	tileRect.x = 176;
	tileRect.y = 0;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	entRect.x = 0;
	entRect.y = (BOARD_BOTTOM_WALL - 1) * 16;
	tileRect.x = 176;
	tileRect.y = 16;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	entRect.x = (BOARD_WIDTH*16) - 16;
	entRect.y = (BOARD_TOP_WALL + 1) * 16;
	tileRect.x = 192;
	tileRect.y = 0;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	entRect.x = (BOARD_WIDTH*16) - 16;
	entRect.y = (BOARD_BOTTOM_WALL - 1) * 16;
	tileRect.x = 192;
	tileRect.y = 16;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);

	/*SDL_Rect r1 = {0, 0, 1, SCREEN_HEIGHT};
	SDL_Rect r2 = {0, 0, SCREEN_WIDTH, 1};

	for (i = 0; i < SCREEN_WIDTH / 16; i++)
	{
		SDL_FillRect(buffer, &r1, SDL_MapRGB(buffer->format, 0, 0, 0));
		r1.x += 16;
	}

	for (i = 0; i < SCREEN_HEIGHT / 16; i++)
	{
		SDL_FillRect(buffer, &r2, SDL_MapRGB(buffer->format, 0, 0, 0));
		r2.y += 16;
	}  */

	for (i = 0; i < getEntityListSize(); i++)
	{
		entRect.w = 16;
		entRect.h = 16;

		switch (entList[i]->type)
		{
			case PLAYER1:
			drawPlayer1(buffer, (Player*)entList[i]);
			break;
			case PLAYER2:
			break;
			case PERMABLOCK:
			/*entRect.x = entList[i]->permaBlock.x * 16;
			entRect.y = entList[i]->permaBlock.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 100, 100, 100));*/
			break;
			case GAMEBLOCK:
			drawGameBlock(buffer, (GameBlock*)entList[i]);
			break;
			case ICEBLOCK:
			drawIceblock(buffer, (IceBlock*)entList[i]);
			break;
			case EXPLOSION:
			drawExplosion(buffer, (Explosion*)entList[i]);
			break;
			case LASER:
			entRect.x = 2 + (entList[i]->laser.x * 16) + entList[i]->laser.offsetX - 8;
			entRect.y = 2 + (entList[i]->laser.y * 16) + entList[i]->laser.offsetY - 8;
			entRect.w = 12;
			entRect.h = 12;
			if (entList[i]->laser.allegiance == 0)
			{
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 0, 255, 50));
			}
			else if (entList[i]->laser.allegiance == 1)
			{
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 210, 255, 50));
			}
			else
			{
				printf("Illegal laser allegiance: %d\n", entList[i]->laser.allegiance);
			}
			break;
			case TELEBLOCK:
			drawTeleBlock(buffer, (TeleBlock*)entList[i]);
			break;
			case ICECREAM:
			entRect.x = entList[i]->iceCream.x * 16;
			entRect.y = entList[i]->iceCream.y * 16;
			tileRect.x = 208;
			tileRect.y = 16;
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
			break;
			case SUPERHAMMER:
			entRect.x = entList[i]->hammer.x * 16;
			entRect.y = entList[i]->hammer.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 0, 0));
			break;
			case GLUE:
			entRect.x = entList[i]->hammer.x * 16;
			entRect.y = entList[i]->hammer.y * 16;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 150, 75, 0));
			break;
			case ENEMY_CRAWLER:
			drawCrawler(buffer, (Enemy*)entList[i]);
			/*entRect.x = (entList[i]->enemy.x * 16) + entList[i]->enemy.offsetX - 8;
			entRect.y = (entList[i]->enemy.y * 16) + entList[i]->enemy.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 100, 5));
			if (entList[i]->enemy.cream != NULL)
			{
				entRect.y -= 16;
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 240));
			}     */
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
			case ENEMY_BOXERGREG:
			entRect.x = (entList[i]->enemy.x * 16) + entList[i]->enemy.offsetX - 8;
			entRect.y = (entList[i]->enemy.y * 16) + entList[i]->enemy.offsetY - 8;
			SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 170, 170, 190));
			if (entList[i]->enemy.cream != NULL)
			{
				entRect.y -= 16;
				SDL_FillRect(buffer, &entRect, SDL_MapRGB(buffer->format, 255, 255, 240));
			}
			
			//draw sword if necessary
			if (entList[i]->enemy.AISlot1 == 1)
			{
				switch (entList[i]->enemy.AISlot2)
				{
					case 0:
					altRect.x = (entList[i]->enemy.x * 16) + 4;
                                        altRect.y = (entList[i]->enemy.y * 16) - 8;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 255, 0, 50));
					break;
                                        case 1:
                                        altRect.x = (entList[i]->enemy.x * 16) + 16;
                                        altRect.y = (entList[i]->enemy.y * 16) + 4;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 255, 0, 50));
					break;
					case 2:
					altRect.x = (entList[i]->enemy.x * 16) + 4;
                                        altRect.y = (entList[i]->enemy.y * 16) + 16;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 255, 0, 50));
					break;
					case 3:
					altRect.x = (entList[i]->enemy.x * 16) - 8;
                                        altRect.y = (entList[i]->enemy.y * 16) + 4;
                                        SDL_FillRect(buffer, &altRect, SDL_MapRGB(buffer->format, 255, 0, 50));
					break;
					default:
					break;
				}
			}
			break;
			default:
			break;
		}
	}
	
	drawHealthScores(buffer);
	
	drawLatestPushDown(buffer);
}

void drawGameOverScreen(SDL_Surface* buffer)
{
	SDL_Color cl = {255, 255, 255, 0};
	
	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 0, 0));

	char p1HealthText[50];
	sprintf(p1HealthText, "GAME OVER");
	SDL_Surface* gameOverText;

	gameOverText = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect textPos = {buffer->w/2 - gameOverText->w/2, 50, 0, 0};
	SDL_BlitSurface(gameOverText, NULL, buffer, &textPos);
	SDL_FreeSurface(gameOverText);

	switch(getGameState())
	{
		case 2:
		sprintf(p1HealthText, "YOU COULDN'T TAKE IT!");
		break;
		case 3:
		sprintf(p1HealthText, "THE GOODS GOT AWAY! HOW COULD YOU?");
		break;
	}

	textPos.y += 50;
	gameOverText = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_BlitSurface(gameOverText, NULL, buffer, &textPos);
	SDL_FreeSurface(gameOverText);
}

