/* Whimsy Block Go
 * ---------------
 * A game for Windows written by Daniel Savage.
 *
 * Copyright 2012
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "Entity.h"
#include "Keyboard.h"
#include "Draw.h"
#include "Pushdown.h"
#include "GameLogic.h"
#include "GameScreens.h"
#include "Particle.h"
#include "Audio.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 272

typedef enum
{
	INIT = 0,
	SPLASH,
	TITLE,
	KEY_CONFIG,
	ETC_CONFIG,
	INGAME,
	INGAME2,
	GAME_OVER,
	ENCOURAGEMENT,
	TRANSITION_TO_TITLE,
	TRANSITION_TO_BLACK
} screenState;

// 1 of the (X) is clicked, game should then entirely exit
int hardCoreQuit = 0;

SDL_Surface* screen;
SDL_Surface* buffer;

screenState currentState;

int init()
{
	// strangely prints stdout to stdout
	freopen( "CON", "w", stdout );

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		perror("Error initalizing SDL");
		return 1;
	}

	if ((screen = SDL_SetVideoMode(SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2, 32, SDL_SWSURFACE)) == NULL)
	{
		perror("Error initalizing screen");
		return 1;
	}
	
	if (TTF_Init() != 0)
	{
		perror("Error initalizing SDL_ttf.");
		return 1;
	}
	
	if (setupAssets() != 0)
	{
		perror("Error setting up file assets");
		return -1;
	}
	
	if (setupAudio() != 0)
	{
		perror("Error setting up audio");
		return -1;
	}
	
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	SDL_WM_SetCaption( "Whimsy Block Go", NULL );

	return 0;
}

int deinit()
{
	SDL_FreeSurface(buffer);
	
	clearAssets();
	clearAudio();

	TTF_Quit();
	SDL_Quit();
	
	return 0;
}

// sorts all of the entities according to y and type
// so they're drawn mostly correctly.
void sortEntitiesForDrawing()
{
	int compareEntities(const void * elem1, const void * elem2)
	{
		Entity* en1 = *(Entity**)elem1;
		Entity* en2 = *(Entity**)elem2;
		
		// poofs are always on top
		if (en1->type == POOF)
		{
			return 1;
		}
		else if (en2->type == POOF)
		{
			return -1;
		}
		
		//glues are always on bottom
		if (en1->type == GLUE)
		{
			return -1;
		}
		else if (en2->type == GLUE)
		{
			return 1;
		}

		if (en1->base.y < en2->base.y)
		{
			return -1;
		}
		else if (en2->base.y < en1->base.y)
		{
			return 1;
		}
		else
		{
			if (en1->type == PLAYER1)
			{
				return 1;
			}
			else if (en2->type == PLAYER1)
			{
				return -1;
			}

			if (en1->type == EXPLOSION)
			{
				return -1;
			}
			else if (en2->type == EXPLOSION)
			{
				return 1;
			}
		}
		
		return 0;
	}
	
	qsort(getEntityList(), getEntityListSize(), sizeof(Entity*), compareEntities); //not optimal, but whatever
}

// perform game logic on all the connecting game blocks
// NOTE: this code uses a nested function, which is a feature of GCC;
//       if you find yourself cursed with porting such code using another
//       compiler (eg: ICC), you may need to remove the nest to a seperate
//       function. My deepest apologies. I love you.
void whimsyBlocks()
{
	int i,j;
	Entity* gameBlockGrid[32][28];
	int flagMatrix[32][28];
	int connectedBlockCounter = 0;
	Entity** entList = getEntityList();

	void checkConnectedBlocks(int x, int y, BlockType b)
	{
		if (x > BOARD_WIDTH || y > BOARD_HEIGHT || x < 0 || y < 0)
		{
			return;
		}

		if (gameBlockGrid[x][y] == NULL)
		{
			return;
		}
		
		if (gameBlockGrid[x][y]->type != GAMEBLOCK)
		{
			return;
		}

		if (gameBlockGrid[x][y]->gBlock.bType != b || flagMatrix[x][y] != 1)
		{
			return;
		}
		
		connectedBlockCounter++;
		
		flagMatrix[x][y] = 0;
		checkConnectedBlocks(x + 1, y, b);
		checkConnectedBlocks(x, y + 1, b);
		checkConnectedBlocks(x - 1, y, b);
		checkConnectedBlocks(x, y - 1, b);
	}
	
	void clearConnectedBlocks(int x, int y, BlockType b)
	{
		Entity* en;

		if (x < 0 || y < 0 || x > BOARD_WIDTH || y > BOARD_HEIGHT)
		{
			return;
		}

		if (gameBlockGrid[x][y] == NULL)
		{
			return;
		}
		
		if (gameBlockGrid[x][y]->type != GAMEBLOCK)
		{
			return;
		}

		if (gameBlockGrid[x][y]->gBlock.bType != b)
		{
			return;
		}

		//turn the block into it's designated type
		/*switch (b)
		{
			case RED_BLOCK:
			gameBlockGrid[x][y]->type = SUPERHAMMER;
			break;
			case GREEN_BLOCK:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			pushEntity(TELEBLOCK, x, y);
			break;
			case BLUE_BLOCK:
			gameBlockGrid[x][y]->type = ICEBLOCK;
			gameBlockGrid[x][y]->iBlock.moving = 0;
			gameBlockGrid[x][y]->iBlock.direction = 0;
			gameBlockGrid[x][y]->iBlock.offsetX = 8;
			gameBlockGrid[x][y]->iBlock.offsetY = 8;
			gameBlockGrid[x][y]->iBlock.health = 3;
			gameBlockGrid[x][y]->iBlock.startTime = getTimeSingleton() - ((rand() % 7) * 100);
			break;
			case YELLOW_BLOCK:
			gameBlockGrid[x][y]->type = GLUE;
			break;
			default:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			printf("Unrecognized block pattern destroyed: %d\n", b);
			break;
		}   */
		switch (b)
		{
			case RED_BLOCK:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			en = pushEntity(POOF, x, y);
			en->poof.birthed = 1;
			en->poof.offsetX = 8;
			en->poof.offsetY = 8;
			en->poof.colour = 1;
			break;
			case GREEN_BLOCK:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			en = pushEntity(POOF, x, y);
			en->poof.birthed = 1;
			en->poof.offsetX = 8;
			en->poof.offsetY = 8;
			en->poof.colour = 3;
			break;
			case BLUE_BLOCK:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			en = pushEntity(POOF, x, y);
			en->poof.birthed = 1;
			en->poof.offsetX = 8;
			en->poof.offsetY = 8;
			en->poof.colour = 2;
			break;
			case YELLOW_BLOCK:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			en = pushEntity(POOF, x, y);
			en->poof.birthed = 1;
			en->poof.offsetX = 8;
			en->poof.offsetY = 8;
			en->poof.colour = 4;
			break;
			default:
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
			printf("Unrecognized block pattern destroyed: %d\n", b);
			break;
		}

		gameBlockGrid[x][y] = NULL;

		clearConnectedBlocks(x + 1, y, b);
		clearConnectedBlocks(x, y + 1, b);
		clearConnectedBlocks(x - 1, y, b);
		clearConnectedBlocks(x, y - 1, b);
	}

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 28; j++)
		{
			gameBlockGrid[i][j] = NULL;
			flagMatrix[i][j] = 0;
		}
	}

	for (i = 0; i < getEntityListSize(); i++)
	{
		if (entList[i]->type == GAMEBLOCK && entList[i]->gBlock.height == 0)
		{
			if (entList[i]->base.x >= 0 && entList[i]->base.y >= 0 && entList[i]->base.x < 32 && entList[i]->base.y < 26)
			{
				gameBlockGrid[entList[i]->base.x][entList[i]->base.y] = entList[i];
				flagMatrix[entList[i]->base.x][entList[i]->base.y] = 1;
			}
		}
	}

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 28; j++)
		{
			connectedBlockCounter = 0;
			
			if (gameBlockGrid[i][j] != NULL)
			{
				checkConnectedBlocks(i, j, gameBlockGrid[i][j]->gBlock.bType);
				
				if (connectedBlockCounter > 3)
				{
					clearConnectedBlocks(i, j, gameBlockGrid[i][j]->gBlock.bType);
					
					addScore(75);
				}
			}
		}
	}
}

int gameOverLoop()
{
	int quit = 0;
	SDL_Event ev;
	
	setTimeSingleton(SDL_GetTicks());
	
	Uint32 gameOverHang = getTimeSingleton();
	
	while (!quit && !hardCoreQuit)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				hardCoreQuit = 1;
			}
		}
		
		setTimeSingleton(SDL_GetTicks());
		
		if (getTimeSingleton() - gameOverHang > 6 * 1000)
		{
			quit = 1;
		}
		
		drawGameOverScreen(buffer);

		SDL_SoftStretch(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(17); //crude 60fps
	}
	
	return 0;
}

int testLoop(int twoPlayerGame)
{
	int i;   //loop iterators

	int quit = 0;
	SDL_Event ev;
	
	initalizeParticleList();
	
	if (twoPlayerGame == 0)
	{
		clearResetGame(1);
	}
	else
	{
		clearResetGame(2);
	}

	beginGame();

	while (!quit && !hardCoreQuit)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				hardCoreQuit = 1;
			}
		}
		
		if (getGameState() != 1)
		{
			quit = 1;
		}
		
		//update call
		pollKeyboard();
		
		updatePushMessages();
		updateGameLogic();

		Entity** entList = getEntityList();
		setTimeSingleton(SDL_GetTicks());

		for (i = 0; i < getEntityListSize(); i++)
		{
			if (entList[i]->type == DELETE_ME_PLEASE)
			{
				popEntity(entList[i]);
				i--;
				continue;
			}

			whimsyBlocks();

			update_entity(entList[i], getTimeSingleton());
		}

		sortEntitiesForDrawing();
		
		updateParticles();

		testDraw(buffer);

		SDL_SoftStretch(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(17); //crude 60fps
	}
	
	freeEntityList();
	
	freeParticleList();

	return 0;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	currentState = TITLE; //should start at NONE

	if (init() != 0)
	{
		deinit();
		return 1;
	}	
	
	//init error handling will go here

	while (hardCoreQuit == 0)
	{
		if (currentState == INIT)
		{
			currentState = SPLASH;
		}
		else if (currentState == SPLASH)
		{
			switch (preambleSplashScreen(screen))
			{
				case 0:
				currentState = TRANSITION_TO_TITLE;
				break;
				case 1:
				hardCoreQuit = 1;
				break;
				default:
				fprintf(stderr, "Bad splash screen return!\n");
				currentState = TRANSITION_TO_TITLE;
				break;
			}
		}
		else if (currentState == TITLE)
		{
			switch (titleScreen(screen))
			{
				case 0:
				currentState = TRANSITION_TO_BLACK;
				break;
				case 1:
				currentState = INGAME;
				break;
				case 2:
				currentState = INGAME2;
				break;
				case 3:
				currentState = KEY_CONFIG;
				break;
				case 4:
				break;
				case -1:
				hardCoreQuit = 1;
				break;
				default:
				fprintf(stderr, "Title screen returned a strange menu code\n");
				break;
			}
		}
		else if (currentState == INGAME)
		{
			testLoop(0);
			
			currentState = GAME_OVER;
		}
		else if (currentState == INGAME2)
		{
			testLoop(1);
			
			currentState = GAME_OVER;
		}
		else if (currentState == KEY_CONFIG)
		{
			if (keyConfigScreen(screen) == 1)
			{
				hardCoreQuit = 1;
			}	

			currentState = TITLE;
		}
		else if (currentState == GAME_OVER)
		{
			gameOverLoop();
			currentState = TITLE;
		}
		else if (currentState == TRANSITION_TO_TITLE)
		{
			if (titleTransitionScreen(screen) == 1)
			{
				hardCoreQuit = 1;
			}

			currentState = TITLE;
		}
		else if (currentState == TRANSITION_TO_BLACK)
		{
			if (exitTransitionScreen(screen) == 1)
			{
				hardCoreQuit = 1;
			}

			hardCoreQuit = 1;
		}
	}

	deinit();

	return 0;
}

