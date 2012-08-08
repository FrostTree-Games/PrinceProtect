#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "Draw.h"
#include "Entity.h"
#include "Pushdown.h"
#include "GameLogic.h"
#include "Particle.h"
#include "Keyboard.h"

int pushNotificationFontSize = 7; //this font should be 7x7 pixels
TTF_Font* pushNotificationFont = NULL;

SDL_Surface* tileSheet;
SDL_Surface* pubLogo;
SDL_Surface* devLogo;
SDL_Surface* gameLogo; //used on the title screen
SDL_Surface* portrait1; //title screen portraits
SDL_Surface* portrait2;

// PIXEL MANIPULATION FUNCTIONS STOLEN FROM
// http://lazyfoo.net/SDL_tutorials/lesson31/index.php
// USED WITHOUT PERMISSION:: ALL RIGHTS TO LAZY FOO PRODUCTIONS
Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;
	
	//Get the requested pixel
	return pixels[ ( y * surface->w ) + x ];
}
void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;
	
	//Set the pixel
	pixels[ ( y * surface->w ) + x ] = pixel;
}

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
	
	if ((gameLogo = IMG_Load("gfx/gameLogo.png")) == NULL)
	{
		fprintf(stderr, "Error loading game logo. Check assets.\n");
		return 1;
	}
	
	if ((portrait1 = IMG_Load("gfx/port1.png")) == NULL)
	{
		fprintf(stderr, "Error loading first portrait. Check assets.\n");
		return 1;
	}
	
	if ((portrait2 = IMG_Load("gfx/port2.png")) == NULL)
	{
		fprintf(stderr, "Error loading second portrait. Check assets.\n");
		return 1;
	}

	return 0;
}

void clearAssets()
{
	TTF_CloseFont(pushNotificationFont);

	SDL_FreeSurface(tileSheet);
	
	SDL_FreeSurface(gameLogo);
	
	SDL_FreeSurface(devLogo);
	SDL_FreeSurface(pubLogo);
	
	SDL_FreeSurface(portrait1);
	SDL_FreeSurface(portrait2);
}

// draws an easy GUI box where needed
void fillGUIBox(SDL_Surface* buffer, int x, int y, int w, int h, int selected)
{
	int i, j;

	if (w < 2 || h < 2)
	{
		return;
	}

	SDL_Rect tileRect = {0, 0, 16, 16};
	SDL_Rect bufferRect = {0, 0, 0, 0};

	for (i = x; i < x + w; i++)
	{
		for (j = y; j < y + h; j++)
		{
			if (i == x)
			{
				if (j == y)
				{
					tileRect.x = 208;
					tileRect.y = 208;
				}
				else if (j == y + h - 1)
				{
					tileRect.x = 208;
					tileRect.y = 240;
				}
				else
				{
					tileRect.x = 208;
					tileRect.y = 224;
				}
			}
			else if (i == x + w - 1)
			{
				if (j == y)
				{
					tileRect.x = 240;
					tileRect.y = 208;
				}
				else if (j == y + h - 1)
				{
					tileRect.x = 240;
					tileRect.y = 240;
				}
				else
				{
					tileRect.x = 240;
					tileRect.y = 224;
				}
			}
			else
			{
				if (j == y)
				{
					tileRect.x = 224;
					tileRect.y = 208;
				}
				else if (j == y + h - 1)
				{
					tileRect.x = 224;
					tileRect.y = 240;
				}
				else
				{
					tileRect.x = 224;
					tileRect.y = 224;
				}
			}
			
			bufferRect.x = (i) * 16;
			bufferRect.y = (j) * 16;
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &bufferRect);
			
			if (selected)
			{
				tileRect.y -= 48;
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &bufferRect);
			}
		}
	}
}

void drawLatestPushDown(SDL_Surface* buffer)
{
	PushMessage* latestMessage = getCurrentMessage();
	if (latestMessage == NULL)
	{
		return;
	}

	SDL_Surface* text_surface;
	SDL_Color cl = {255, 0, 0, 0};
	if ((getTimeSingleton() / 5) % 2 == 0)
	{
		cl.r = 255;
		cl.g = 255;
		cl.b = 0;
	}

	text_surface = TTF_RenderText_Solid(pushNotificationFont, latestMessage->message, cl);
	
	SDL_Rect msgPos = {(buffer->w - text_surface->w)/2, 50, 0, 0};

	if (getTimeSingleton() - latestMessage->startTime < 500)
	{
		msgPos.y = (Sint16)(50 * (getTimeSingleton() - latestMessage->startTime)/500.0);
	}

	SDL_BlitSurface(text_surface, NULL, buffer, &msgPos);

	SDL_FreeSurface(text_surface);
	

	if (cl.g < 255)
	{
		cl.g = 255;
	}
	else
	{
		cl.g = 0;
	}
	text_surface = TTF_RenderText_Solid(pushNotificationFont, latestMessage->message, cl);
	msgPos.x += 1;
	msgPos.y += 1;
	if (getTimeSingleton() - latestMessage->startTime < 500)
	{
		msgPos.y = (Sint16)(50 * (getTimeSingleton() - latestMessage->startTime)/500.0);
	}
	SDL_BlitSurface(text_surface, NULL, buffer, &msgPos);
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
	p1HpPos.x += 220;
	SDL_BlitSurface(health_surface, NULL, buffer, &p1HpPos);
	SDL_FreeSurface(health_surface);

	sprintf(p1HealthText, "PRINCES: %.2d", getIceCreamCount());
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect iceCreamNumbers = {SCREEN_WIDTH/2 - health_surface->w/2, 5, 0, 0};
	SDL_BlitSurface(health_surface, NULL, buffer, &iceCreamNumbers);
	SDL_FreeSurface(health_surface);
	
	sprintf(p1HealthText, "SCORE: %.6d", getScore());
	health_surface = TTF_RenderText_Solid(pushNotificationFont, p1HealthText, cl);
	SDL_Rect scoreNumbers = {SCREEN_WIDTH/2 - health_surface->w/2, 15, 0, 0};
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

void drawKeyConfigScreen(SDL_Surface* buffer, int menuPosition, int keyCheck)
{
	int i;
	SDL_Color textCol = {255, 255, 240, 0};
	SDL_Rect buttonTextPosition = {40, 188, 0, 0};
	SDL_Surface* buttonText;

	SDL_Rect controlsPosition = {0, 0, 0, 0};
	SDL_Surface* controlsText;
	
	fillGUIBox(buffer, 0, 0, 20, 17, 0);

	fillGUIBox(buffer, 0, 4, 10, 7, 0);
	fillGUIBox(buffer, 10, 4, 10, 7, 0);
	
	fillGUIBox(buffer, 7, 1, 6, 2, 0);

	controlsText = TTF_RenderText_Solid(pushNotificationFont, "key config", textCol);
	SDL_Rect titleTextPosition = {SCREEN_WIDTH/2 - controlsText->w/2, 26, 0, 0};
	SDL_BlitSurface(controlsText, NULL, buffer, &titleTextPosition);
	SDL_FreeSurface(controlsText);

	controlsPosition.x = 108;
	controlsPosition.y = 80;
	for (i = 0; i < 6; i++)
	{
		SDL_Rect labelPosition = {10, 76 + i*16, 0, 0};
		
		switch (i)
		{
			case 0:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Up", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 1:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Down", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 2:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Left", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 3:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Right", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 4:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Grab Block", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 5:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Strike", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			default:
			break;
		}

		switch (i)
		{
			case 0:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_UP)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 1:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_DOWN)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 2:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_LEFT)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 3:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_RIGHT)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 4:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_A)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 5:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P1_B)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			default:
			break;
		}
		
		controlsPosition.y += 16;
	}
	
	controlsPosition.x = 272;
	controlsPosition.y = 80;
	for (i = 0; i < 6; i++)
	{
		SDL_Rect labelPosition = {170, 76 + i*16, 0, 0};

		switch (i)
		{
			case 0:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Up", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 1:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Down", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 2:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Left", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 3:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Move Right", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 4:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Grab Block", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 5:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, "Strike", textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &labelPosition);
			SDL_FreeSurface(controlsText);
			break;
			default:
			break;
		}

		switch (i)
		{
			case 0:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_UP)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 1:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_DOWN)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 2:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_LEFT)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 3:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_RIGHT)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 4:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_A)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			case 5:
			controlsText = TTF_RenderText_Solid(pushNotificationFont, (const char*)SDL_GetKeyName(getSDLKeyValue(P2_B)), textCol);
			SDL_BlitSurface(controlsText, NULL, buffer, &controlsPosition);
			SDL_FreeSurface(controlsText);
			break;
			default:
			break;
		}
		
		controlsPosition.y += 16;
	}
	
	if (keyCheck != -1)
	{
		SDL_Rect queryTextPosition = {110, 110, 0, 0};
		SDL_Surface* queryTextSurface = NULL;
		char queryText[100];
		char* keyText;

		switch (keyCheck)
		{
			case 0:
			keyText = (char*)"P1 MOVE UP";
			break;
			case 1:
			keyText = (char*)"P1 MOVE DOWN";
			break;
			case 2:
			keyText = (char*)"P1 MOVE LEFT";
			break;
			case 3:
			keyText = (char*)"P1 MOVE RIGHT";
			break;
			case 4:
			keyText = (char*)"P1 GRAB BLOCK";
			break;
			case 5:
			keyText = (char*)"P1 STRIKE";
			break;
			case 10:
			keyText = (char*)"P2 MOVE UP";
			break;
			case 11:
			keyText = (char*)"P2 MOVE DOWN";
			break;
			case 12:
			keyText = (char*)"P2 MOVE LEFT";
			break;
			case 13:
			keyText = (char*)"P2 MOVE RIGHT";
			break;
			case 14:
			keyText = (char*)"P2 GRAB BLOCK";
			break;
			case 15:
			keyText = (char*)"P2 STRIKE";
			break;
			default:
			keyText = (char*)"???";
			break;

		}

		sprintf(queryText, "Press key for %s", keyText);
		fillGUIBox(buffer, 4, 7, 12, 3, 0);
		queryTextSurface = TTF_RenderText_Solid(pushNotificationFont, (const char*)queryText, textCol);
		queryTextPosition.x = SCREEN_WIDTH/2 - queryTextSurface->w/2;
		queryTextPosition.y = SCREEN_HEIGHT/2 - queryTextSurface->h/2;
		SDL_BlitSurface(queryTextSurface, NULL, buffer, &queryTextPosition);
		SDL_FreeSurface(queryTextSurface);
	}

	for (i = 0; i < 3; i++)
	{

		if (i == menuPosition)
		{
			fillGUIBox(buffer, 2 + 2*i + i*3, 11, 4, 4, 1);
		}
		else
		{
			fillGUIBox(buffer, 2 + 2*i + i*3, 11, 4, 4, 0);
		}

		switch (i)
		{
			case 0:
			buttonText = TTF_RenderText_Solid(pushNotificationFont, "P1 KEY", textCol);
			break;
			case 1:
			buttonText = TTF_RenderText_Solid(pushNotificationFont, "P2 KEY", textCol);
			break;
			case 2:
			buttonText = TTF_RenderText_Solid(pushNotificationFont, "BACK", textCol);
			break;
			default:
			buttonText = TTF_RenderText_Solid(pushNotificationFont, "LOL WUT?", textCol);
			break;
		}
		
		SDL_BlitSurface(buttonText, NULL, buffer, &buttonTextPosition);
		SDL_FreeSurface(buttonText);
		
		if (i < 2)
		{
			SDL_Rect configPosition = buttonTextPosition;
			configPosition.y += 15;

			buttonText = TTF_RenderText_Solid(pushNotificationFont, "CONFIG", textCol);
			SDL_BlitSurface(buttonText, NULL, buffer, &configPosition);
			SDL_FreeSurface(buttonText);
		}

		buttonTextPosition.x += 80;
	}
}

void drawToTitleWipe(SDL_Surface* buffer, float curtainLength)
{
	drawTitleScreen(buffer, 0, 0);
	
	SDL_Rect leftCurtain = {0, 0, (int)(curtainLength*(SCREEN_WIDTH/2)), SCREEN_HEIGHT};
	SDL_Rect rightCurtain = {SCREEN_WIDTH - (int)(curtainLength*(SCREEN_WIDTH/2)), 0, SCREEN_WIDTH/2, SCREEN_HEIGHT};
	
	SDL_FillRect(buffer, &leftCurtain, SDL_MapRGB(buffer->format, 0, 0, 0));
	SDL_FillRect(buffer, &rightCurtain, SDL_MapRGB(buffer->format, 0, 0, 0));
}

void drawToExitWipe(SDL_Surface* buffer, float curtainLength)
{
	curtainLength = 1.0f - curtainLength;
	
	drawTitleScreen(buffer, 4, 5000);

	SDL_Rect leftCurtain = {0, 0, (int)(curtainLength*(SCREEN_WIDTH/2)), SCREEN_HEIGHT};
	SDL_Rect rightCurtain = {SCREEN_WIDTH - (int)(curtainLength*(SCREEN_WIDTH/2)), 0, SCREEN_WIDTH/2, SCREEN_HEIGHT};

	SDL_FillRect(buffer, &leftCurtain, SDL_MapRGB(buffer->format, 0, 0, 0));
	SDL_FillRect(buffer, &rightCurtain, SDL_MapRGB(buffer->format, 0, 0, 0));
}

void drawGetReadyScreen(SDL_Surface* buffer, int playerCount, int seed)
{
	SDL_Color textCol = {255, 255, 240, 0};
	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 0, 0, 0));
	SDL_Surface* encourageText = TTF_RenderText_Solid(pushNotificationFont, "GET READY!", textCol);
	SDL_Rect text1Position = {SCREEN_WIDTH/2 - encourageText->w/2, SCREEN_HEIGHT/4, 0, 0};
	SDL_Rect playerPosition = {SCREEN_WIDTH/2 - 8, SCREEN_HEIGHT/2 - 8, 0, 0};
	
	if (encourageText != NULL)
	{
		SDL_BlitSurface(encourageText, NULL, buffer, &text1Position);
		SDL_FreeSurface(encourageText);
	}

	if (playerCount == 1)
	{
		SDL_Rect spritePosition = {0, 0, 16, 16};
		SDL_BlitSurface(tileSheet, &spritePosition, buffer, &playerPosition);
	}
	else if (playerCount == 2)
	{
		SDL_Rect spritePosition = {0, 0, 16, 16};
		playerPosition.x -= 16;
		SDL_BlitSurface(tileSheet, &spritePosition, buffer, &playerPosition);
		playerPosition.x += 32;
		spritePosition.x += 64;
		SDL_BlitSurface(tileSheet, &spritePosition, buffer, &playerPosition);
	}
	
	encourageText = TTF_RenderText_Solid(pushNotificationFont, "ENCOURAGEMENT:", textCol);

	if (encourageText != NULL)
	{
		text1Position.x = SCREEN_WIDTH/2 - encourageText->w/2;
		text1Position.y = 3*(SCREEN_HEIGHT/4);

		SDL_BlitSurface(encourageText, NULL, buffer, &text1Position);
		SDL_FreeSurface(encourageText);
	}

	switch (seed + (10*(playerCount - 1)))
	{
		//1 player encouragement
		case 0:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Please win and stuff", textCol);
		break;
		case 1:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Whoa now, don't be too awesome", textCol);
		break;
		case 2:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "You have a nice haircut", textCol);
		break;
		case 3:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Remember your sensei's words", textCol);
		break;
		case 4:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "I like your key config", textCol);
		break;
		case 5:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "If you were a meal you would be expensive", textCol);
		break;
		case 6:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Follow your heart's gut", textCol);
		break;
		case 7:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "For our proud nation", textCol);
		break;
		case 8:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "You are very attractive", textCol);
		break;
		case 9:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "You're bucket", textCol);
		break;

		//2 player encouragement
		case 10:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "It's great seeing one friend sacrifice for another", textCol);
		break;
		case 11:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Where did you find each other? The awesome store?", textCol);
		break;
		case 12:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Sorry, the game only supports one super player", textCol);
		break;
		case 13:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "I believe in most of you", textCol);
		break;
		case 14:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Good class! A+!", textCol);
		break;
		case 15:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Now with 100% more misogyny!", textCol);
		break;
		case 16:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "Straight out of Compton", textCol);
		break;
		case 17:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "For our proud nation", textCol);
		break;
		case 18:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "You are both very attractive", textCol);
		break;
		default:
		encourageText = TTF_RenderText_Solid(pushNotificationFont, "I've never seen a better team", textCol);
		break;
	}
	
	if (encourageText != NULL)
	{
		text1Position.x = SCREEN_WIDTH/2 - encourageText->w/2;
		text1Position.y = 3*(SCREEN_HEIGHT/4) + 15;

		SDL_BlitSurface(encourageText, NULL, buffer, &text1Position);
		SDL_FreeSurface(encourageText);
	}
}

void drawTitleScreen(SDL_Surface* buffer, int mSelected, Uint32 delta)
{
	int i;

	SDL_Rect textPos = {0, 0, 0 ,0};
	SDL_Surface* menuItemTextSurface = NULL;
	SDL_Color textCol = {255, 255, 240, 0};

	int titleDistance = ((int)( (delta/500.0) * (gameLogo->h + 10))) - gameLogo->h;
	if (titleDistance > 10)
	{
		titleDistance = 10;
	}

	SDL_Rect titlePos = {49, titleDistance, 0, 0};

	SDL_FillRect(buffer, NULL, SDL_MapRGB(buffer->format, 58, 197, 190));

	for (i = 0; i < 4; i++)
	{
		if (i == mSelected)
		{
			fillGUIBox(buffer, 6, 6 + 2*i, 8, 2, 1);
		}
		else
		{
			fillGUIBox(buffer, 6, 6 + 2*i, 8, 2, 0);
		}
	}
	
	SDL_BlitSurface(gameLogo, NULL, buffer, &titlePos);

	if (delta > 500)
	{
		int portraitDistance = 320 - ((int)( ((delta - 500)/500.0) * 180));
		if (portraitDistance < 140)
		{
			portraitDistance = 140;
		}
		
		SDL_Rect portrait1Pos = {SCREEN_WIDTH/8 - portrait1->w/2, portraitDistance, 0, 0};
		SDL_Rect portrait2Pos = {7*(SCREEN_WIDTH/8) - portrait1->w/2, portraitDistance, 0, 0};
		SDL_BlitSurface(portrait1, NULL, buffer, &portrait1Pos);
		SDL_BlitSurface(portrait2, NULL, buffer, &portrait2Pos);
	}

	for (i = 0; i < 4; i++)
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
			menuItemTextSurface = TTF_RenderText_Solid(pushNotificationFont, "EXIT", textCol);
			break;
			default:
			break;
		}

		textPos.x = SCREEN_WIDTH/2 - menuItemTextSurface->w/2;
		textPos.y = SCREEN_HEIGHT/2 - 16 + i*32;
		
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

	if (pl->dead > 0)
	{
		entRect.x = (pl->x * 16) + pl->offsetX - 8;
		entRect.y = (pl->y * 16) + pl->offsetY - 8;
		
		if (getTimeSingleton() - pl->swordTimer > 1900)
		{
			tileRect.x = 48;
			tileRect.y = 64;
		}
		else
		{
			switch (((getTimeSingleton() - pl->swordTimer) / 50) % 4)
			{
				case 0:
				tileRect.x = 32;
				tileRect.y = 64 + 48;
				break;
				case 1:
				tileRect.x = 32;
				tileRect.y = 64 + 16;
				break;
				case 2:
				tileRect.x = 32;
				tileRect.y = 64 + 32;
				break;
				case 3:
				tileRect.x = 32;
				tileRect.y = 64;
				break;
				default:
				tileRect.x = 48;
				tileRect.y = 64;
				break;
			}
		}

		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
	else if (pl->knockBackDirection != 255)
	{
		tileRect.x = 32;
		tileRect.y = 64;
		
		switch (pl->knockBackDirection)
		{
			case 0:
			tileRect.y += 0;
			break;
			case 1:
			tileRect.y += 48;
			break;
			case 2:
			tileRect.y += 16;
			break;
			case 3:
			tileRect.y += 32;
			break;
			default:
			break;
		}
		
		entRect.x = (pl->x * 16) + pl->offsetX - 8;
		entRect.y = (pl->y * 16) + pl->offsetY - 8;
		
		SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	}
	else if (pl->isThrusting  == 0)
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

void drawIceCream (SDL_Surface* buffer, IceCream* cream, int x, int y, int carry)
{
	SDL_Rect tileRect = {160, 192, 16, 16};
	SDL_Rect entRect = {x, y, 0, 0};

	if (cream->frame == 1)
	{
		tileRect.x += 16;
	}
	
	if (carry == 1)
	{
		tileRect.y += 16;
	}	

	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
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

		drawIceCream(buffer, en->cream, entRect.x, entRect.y, 1);
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

		drawIceCream(buffer, en->cream, entRect.x, entRect.y, 1);
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
		
		drawIceCream(buffer, en->cream, entRect.x, entRect.y, 1);
	}
}

void drawLaser(SDL_Surface* buffer, Laser* ls)
{
	SDL_Rect entRect = {0, 0, 16, 16};
	SDL_Rect tileRect = {160, 64, 16, 16};

	entRect.x = (ls->x * 16) + ls->offsetX - 8;
	entRect.y = (ls->y * 16) + ls->offsetY - 8;

	if (ls->frame == 0)
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

void drawPoof(SDL_Surface* buffer, Poof* pf)
{
	SDL_Rect tileRect = {128, 32, 16, 16};
	SDL_Rect entRect = {pf->x * 16 + pf->offsetX - 8, pf->y * 16 + pf->offsetY - 8, 16, 16};
	
	switch(pf->colour)
	{
		case 4:
		tileRect.y += 16;
		case 3:
		tileRect.y += 16;
		case 2:
		tileRect.y += 16;
		case 1:
		tileRect.y += 16;
		case 0:
		break;
		default:
		fprintf(stderr, "Strange poof colour: %d\n", pf->colour);
		break;
	}
	
	if (getTimeSingleton() - pf->startTime > 125)
	{
		tileRect.x += 16;
	}
	
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
}

void drawParticles(SDL_Surface* buffer)
{
	int i;

	Uint32 px = 0;
	Uint8* pxOffset = (Uint8*)&px;
	
	Uint32 px2 = 0;
	Uint8* px2Offset = (Uint8*)&px2;

	Particle* pList = getParticleList();
	
	if (pList == NULL)
	{
		return;
	}
	
	for (i = 0; i < PARTICLE_LIST_MAX_SIZE; i++)
	{
		if (pList[i].type == NONE)
		{
			continue;
		}

		switch (pList[i].type)
		{
			case ICE:
			pxOffset[0] = 255;
			pxOffset[1] = 240;
			pxOffset[2] = 0;
			px2Offset[0] = 255;
			px2Offset[1] = 255;
			px2Offset[2] = 200;
			break;
			case TELESPARK:
			pxOffset[0] = 47;
			pxOffset[1] = 250;
			pxOffset[2] = 54;
			px2Offset[0] = 128;
			px2Offset[1] = 255;
			px2Offset[2] = 232;
			break;
			case MUD:
			pxOffset[0] = 0;
			pxOffset[1] = 200;
			pxOffset[2] = 200;
			break;
			case BLOOD:
			pxOffset[0] = 4;
			pxOffset[1] = 4;
			pxOffset[2] = 196;
			break;
			case SWEAT:
			pxOffset[0] = 255;
			pxOffset[1] = 255;
			pxOffset[2] = 255;
			break;
			case FIRE:
			pxOffset[0] = 5;
			pxOffset[1] = 60;
			pxOffset[2] = 255;
			px2Offset[0] = 240;
			px2Offset[1] = 120;
			px2Offset[2] = 0;
			break;
			default:
			break;
		}
		
		switch (pList[i].type)
		{
			case ICE:
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px2);
			put_pixel32(buffer, (int)pList[i].x + 1, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y + 1, px);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y - 1, px);
			break;
			case TELESPARK:
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x + 1, (int)pList[i].y, px2);
			put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y, px2);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y + 1, px2);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y - 1, px2);
			break;
			case BLOOD:
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x + 1, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y, px);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y + 1, px);
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y - 1, px);
			break;
			case SWEAT:
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px);
			if (pList[i].xVelo > 0)
			{
				put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y, px);
			}
			else
			{
				put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y, px);
			}
			if (pList[i].yVelo > 0)
			{
				put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y + 1, px);
			}
			else
			{
				put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y - 1, px);
			}
			break;
			case MUD:
                        put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px);
                        put_pixel32(buffer, (int)pList[i].x + 1, (int)pList[i].y + 1, px);
			case FIRE:
			put_pixel32(buffer, (int)pList[i].x, (int)pList[i].y, px);
			if (pList[i].xVelo < 0)
			{
				put_pixel32(buffer, (int)pList[i].x + 1, (int)pList[i].y + 1, px);
			}
			else
			{
				put_pixel32(buffer, (int)pList[i].x - 1, (int)pList[i].y + 1, px);
			}
			break;
			default:
			break;
		}
	}
}

void testDraw(SDL_Surface* buffer)
{
	int i,j;

	Entity** entList = getEntityList();

	//used for default drawing
	SDL_Rect entRect = {0, 0, 16, 16};
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
			else if (j == BOARD_TOP_WALL)
			{
				if (i == 0)
				{
					tileRect.x = 192;
					tileRect.y = 112;
				}
				else if (i == BOARD_WIDTH - 1)
				{
					tileRect.x = 176;
					tileRect.y = 112;
				}
				else
				{
					tileRect.x = 160;
					tileRect.y = 112;
				}
			}
			else if (j < BOARD_TOP_WALL && j > BOARD_TOP_WALL - 3)
			{
				if (j == BOARD_TOP_WALL - 1 && (i == 5 || i == 10 || i == 15))
				{
					tileRect.x = 176;
					tileRect.y = 96;
				}
				else
				{
					tileRect.x = 160;
					tileRect.y = 96;
				}
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
	tileRect.x = 192;
	tileRect.y = 128;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	entRect.x = 0;
	entRect.y = (BOARD_BOTTOM_WALL - 1) * 16;
	tileRect.x = 176;
	tileRect.y = 16;
	SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
	entRect.x = (BOARD_WIDTH*16) - 16;
	entRect.y = (BOARD_TOP_WALL + 1) * 16;
	tileRect.x = 176;
	tileRect.y = 128;
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
			drawIceCream(buffer, (IceCream*)entList[i], entRect.x = entList[i]->iceCream.x * 16, entRect.y = entList[i]->iceCream.y * 16, 0);
			break;
			case SUPERHAMMER:
			entRect.x = entList[i]->hammer.x * 16;
			entRect.y = entList[i]->hammer.y * 16;
			tileRect.x = 224;
			tileRect.y = 16;
			SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
			break;
			case GLUE:
			entRect.x = entList[i]->glue.x * 16;
			entRect.y = entList[i]->glue.y * 16;
			tileRect.x = 240;
			tileRect.y = 16;
			// flickering
			if (getTimeSingleton() - entList[i]->glue.startTime > 26 * 1000)
			{
				if ( ((getTimeSingleton() - entList[i]->glue.startTime - (9 * 1000)) / 100) % 2 == 0)
				{
					SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
				}
			}
			else
			{
				SDL_BlitSurface(tileSheet, &tileRect, buffer, &entRect);
			}
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
			case POOF:
			drawPoof(buffer, (Poof*)entList[i]);
			break;
			default:
			break;
		}
	}
	
	fillGUIBox(buffer, 0, 0, 20, 2, 0);

	drawParticles(buffer);

	drawHealthScores(buffer);

	drawLatestPushDown(buffer);
}

void drawGameOverScreen(SDL_Surface* buffer, int menuOption)
{
	int i;

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

	for (i = 0; i < 2; i++)
	{
		if (i == menuOption)
		{
			fillGUIBox(buffer, 1, 8 + 2*i, 5, 2, 1);
		}
		else
		{
			fillGUIBox(buffer, 1, 8 + 2*i, 5, 2, 0);
		}
		
		SDL_Rect menuTextBoxPos = {23, 138 + 32*i, 0, 0};
		switch(i)
		{
			case 0:
			gameOverText = TTF_RenderText_Solid(pushNotificationFont, "PLAY AGAIN", cl);
			break;
			case 1:
			gameOverText = TTF_RenderText_Solid(pushNotificationFont, "TO TITLE", cl);
			break;
			default:
			gameOverText = TTF_RenderText_Solid(pushNotificationFont, "MENU PROBLEM", cl);
			break;
		}
		
		if (gameOverText != NULL)
		{
			SDL_BlitSurface(gameOverText, NULL, buffer, &menuTextBoxPos);
			SDL_FreeSurface(gameOverText);
		}
	}
}

