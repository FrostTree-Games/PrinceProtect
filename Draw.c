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
SDL_Surface* pubLogo;
SDL_Surface* devLogo;

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
		fprintf(stderr, "Error opening font. Check assets.\n");
		return 1;
	}

	if ((tileSheet = IMG_Load("gfx/sheet.png")) == NULL)
	{
		fprintf(stderr, "Error loading tilesheet. Check assets.\n");
		return 1;
	}
	
	if ((devLogo = IMG_Load("gfx/devLogo.png")) == NULL)
	{
		fprintf(stderr, "Error loading developer logo. Check assets.\n");
		return 1;
	}
	
	if ((pubLogo = IMG_Load("gfx/pubLogo.png")) == NULL)
	{
		fprintf(stderr, "Error loading publisher logo. Check assets.\n");
		return 1;
	}

	return 0;
}

void clearAssets()
{
	TTF_CloseFont(pushNotificationFont);

	SDL_FreeSurface(tileSheet);
	
	SDL_FreeSurface(devLogo);
	SDL_FreeSurface(pubLogo);
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
	
	char p2HealthText[50];
	sprintf(p2HealthText, "P2 HP: %.2d/%.2d", getPlayerHealth(2), getPlayerMaxHealth(2));

	SDL_Surface* health_surface;
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	
	SDL_Rect p1HpPos = {10, 5, 0, 0};

	SDL_BlitSurface(health_surface, NULL, buffer, &p1HpPos);
	SDL_FreeSurface(health_surface);
	
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p2HealthText, cl);
	p1HpPos.x += 200;
	SDL_BlitSurface(health_surface, NULL, buffer, &p1HpPos);
	SDL_FreeSurface(health_surface);

	sprintf(p1HealthText, "ICECREAM: %.2d", getIceCreamCount());
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect iceCreamNumbers = {SCREEN_WIDTH/2 - health_surface->w/2, 25, 0, 0};
	SDL_BlitSurface(health_surface, NULL, buffer, &iceCreamNumbers);
	SDL_FreeSurface(health_surface);
	
	sprintf(p1HealthText, "SCORE: %.6d", getScore());
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect scoreNumbers = {SCREEN_WIDTH/2 - health_surface->w/2, 40, 0, 0};
	SDL_BlitSurface(health_surface, NULL, buffer, &scoreNumbers);
	SDL_FreeSurface(health_surface);
}

void drawDevScreen(SDL_Surface* buffer, int devScreenNumber)
{
	SDL_Rect logoPosition = {0, 0, 0, 0};
	SDL_Surface* image = NULL;

	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 0, 0));

	switch (devScreenNumber)
	{
		case 0:
		image = pubLogo;
		break;
		case 1:
		image = devLogo;
		break;
		default:
		return;
	}
	
	logoPosition.x = SCREEN_WIDTH/2 - (image->w)/2;
	logoPosition.y = SCREEN_HEIGHT/2 - (image->h)/2;

	SDL_BlitSurface(image, NULL, buffer, &logoPosition);
}

void drawTitleScreen(SDL_Surface* buffer, int mSelected)
{
	int i;

	SDL_Rect testMenuBox = {SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2, 80, 102};
	SDL_Rect testMenuSelect = {SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 + 1, 80, 20};

	SDL_Rect textPos = {0, 0, 0 ,0};
	SDL_Surface* menuItemTextSurface = NULL;
	SDL_Color textCol = {0, 0, 0, 0};

	testMenuSelect.y = SCREEN_HEIGHT/2 + 1 + mSelected*20;

	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 58, 197, 190));
	SDL_FillRect(buffer, &testMenuBox, SDL_MapRGB(buffer->format, 58, 58, 255));
	SDL_FillRect(buffer, &testMenuSelect, SDL_MapRGB(buffer->format, 255, 255, 20));
	
	for (i = 0; i < 5; i++)
	{
		switch (i)
		{
			case 0:
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "P1 START", textCol);
			break;
			case 1:
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "P2 START", textCol);
			break;
			case 2:
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "KEY CONFIG", textCol);
			break;
			case 3:
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "ETC CONFIG", textCol);
			break;
			case 4:
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "EXIT", textCol);
			break;
			default:
			break;
		}

		textPos.x = SCREEN_WIDTH/2 - menuItemTextSurface->w/2;
		textPos.y = SCREEN_HEIGHT/2 + 1 + i*20;
		
		if (menuItemTextSurface != NULL)
		{
			SDL_BlitSurface(menuItemTextSurface, NULL, buffer, &textPos);
			SDL_FreeSurface(menuItemTextSurface);
		}
	}
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
		SDL_Rect shadowRect= {(gb->x * 16), (gb->y * 16), 16, 16};
		SDL_Rect shadowTileRect = {208, 0, 16, 16};
		
		if (gb->height < 33)
		{
			shadowTileRect.x += 16;
		}
		
		if (gb->height < 66)
		{
			shadowTileRect.x += 16;
		}
		
		SDL_BlitSurface(tileSheet, &shadowTileRect, buffer, &shadowRect);
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
	if (ib->health == 0)
	{
		return;
	}

	SDL_Rect tileRect = {160, 48, 16, 16};
	SDL_Rect entRect = {(ib->x * 16) + (ib->offsetX - 8), (ib->y * 16) + (ib->offsetY - 8), 16, 16};

	tileRect.x += (Sint16)( (3 - ib->health) * 16 );

	if (ib->health == 1 && getTimeSingleton() - ib->startTime > 8 * 1000)
	{
		if ((getTimeSingleton()/100) % 2 == 0)
		{
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
		}
	}
	else
	{
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
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
		
		if (pl->holdingSuperHammer == 1)
		{
			tileRect.y += 64;
		}
		
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

void drawPlayer2(SDL_Surface* buffer, Player* pl)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {64, 0, 16, 16};

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

void drawBoxerGreg(SDL_Surface* buffer, Enemy* en)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {0, 0, 16, 16};
	
	tileRect.x = 32;
	tileRect.y = 192;
	
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
			case 4:
			if (en->cream == NULL)
			{
				tileRect.x += 64;
			}
			switch (en->AISlot2)
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
	
	if (en->AISlot1 == 1)
	{
		tileRect.x = 128;
		tileRect.y = 192;
		
		if (en->frame == 1)
		{
			tileRect.x += 16;
		}

		switch (en->AISlot2)
		{
			case 0:
			tileRect.y += 16;
			entRect.y -= 16;
			break;
			case 1:
			tileRect.y += 32;
			entRect.x += 16;
			break;
			case 2:
			tileRect.y += 0;
			entRect.y += 16;
			break;
			case 3:
			tileRect.y += 48;
			entRect.x -= 16;
			break;
			default:
			break;
		}

		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}

	if (en->cream != NULL)
	{
		entRect.y -= 16;
		
		tileRect.x = 208;
		tileRect.y = 16;

		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
}

void drawShooter(SDL_Surface* buffer, Enemy* en)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {0, 0, 16, 16};

	tileRect.x = 96;
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

void drawLaser(SDL_Surface* buffer, Laser* ls)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {160, 64, 16, 16};

	entRect.x = (ls->x * 16) + ls->offsetX - 8;
	entRect.y = (ls->y * 16) + ls->offsetY - 8;

	if (ls->allegiance == 0)
	{
		tileRect.y += 16;
	}
	
	switch(ls->direction)
	{
		case 0:
		tileRect.x += 48;
		break;
		case 1:
		break;
		case 2:
		tileRect.x += 16;
		break;
		case 3:
		tileRect.x += 32;
		break;
		default:
		break;
	}

	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
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
			drawPlayer2(buffer, (Player*)entList[i]);
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
			drawLaser(buffer, (Laser*)entList[i]);
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
			tileRect.x = 224;
			tileRect.y = 16;
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
			break;
			case GLUE:
			entRect.x = entList[i]->iceCream.x * 16;
			entRect.y = entList[i]->iceCream.y * 16;
			tileRect.x = 240;
			tileRect.y = 16;
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
			break;
			case ENEMY_CRAWLER:
			drawCrawler(buffer, (Enemy*)entList[i]);
			break;
			case ENEMY_SHOOTER:
			drawShooter(buffer, (Enemy*)entList[i]);
			break;
			case ENEMY_BOXERGREG:
			drawBoxerGreg(buffer, (Enemy*)entList[i]);
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

