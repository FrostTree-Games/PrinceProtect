/* PRINCE PROTECT
 * ---------------
 * A game for Windows written by Daniel Savage.
 *
 * Copyright 2012
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "HighScore.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

typedef enum
{
	INIT = 0,
	SPLASH,
	TITLE,
	KEY_CONFIG,
	ETC_CONFIG,
	GETREADY,
	GETREADY2,
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

	SDL_WM_SetCaption( "Whimsy Block Go", NULL );

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
					playSFX(SFX_MUD);

					clearConnectedBlocks(i, j, gameBlockGrid[i][j]->gBlock.bType);
					
					addScore(75);
				}
			}
		}
	}
}

int gameOverLoop(SDL_Surface* screen)
{
	int quit = 0;
	int hardCoreQuit = 0;
	SDL_Event ev;
	
	int newHighScore = 0;
	int menuItem = 0;
	int upKeyDown = 0;
	int downKeyDown = 0;
	int aKeyDown = 0;
 
	if(isHighScore(getScore()))
	{
		char playerNameInput[3];
		playerNameInput[0] = 'A';
		playerNameInput[1] = 'A';
		playerNameInput[2] = 'A';
		int currentlySelected = 0;  //between 0 and 2

		while (!quit && !hardCoreQuit)
		{
			SDL_Event ev2;
			
			while (SDL_PollEvent(&ev2))
			{
				if (ev2.type == SDL_QUIT)
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
				upKeyDown = 0;
				
				playerNameInput[currentlySelected] -= 1;
				
				if (playerNameInput[currentlySelected] < 'A')
				{
					playerNameInput[currentlySelected] = '_';
				}

				playSFX(SFX_MENU);
			}

			if (getKey(P1_DOWN) && downKeyDown == 0)
			{
				downKeyDown = 1;
			}
			else if (!getKey(P1_DOWN) && downKeyDown == 1)
			{
				downKeyDown = 0;

				playerNameInput[currentlySelected] += 1;

				if (playerNameInput[currentlySelected] > '_')
				{
					playerNameInput[currentlySelected] = 'A';
				}


				playSFX(SFX_MENU);
			}
			
			if (getKey(P1_A) && aKeyDown == 0)
			{
				aKeyDown = 1;
			}
			else if (!getKey(P1_A) && aKeyDown == 1)
			{
				aKeyDown = 0;
				
				currentlySelected++;
				if (currentlySelected > 2)
				{
					quit = 1;
				}
	
				playSFX(SFX_MENU);
			}
			
			drawGameOverScreen(buffer, menuItem, currentlySelected, playerNameInput);

			SDL_SoftStretch(buffer, NULL, screen, NULL);
			SDL_Flip(screen);
			SDL_Delay(17); //crude 60fps
		}

		HighScore h;
		h.playerCount = 1;
		h.name[0] = playerNameInput[0]; h.name[1] = playerNameInput[1]; h.name[2] = playerNameInput[2];
		h.score = getScore();
		pushHighScore(h);

		upKeyDown = 0;
		downKeyDown = 0;
		aKeyDown = 0;
		quit = 0;
	}
	
	saveHighScores();

	while (!quit && !hardCoreQuit)
	{
		while (SDL_PollEvent(&ev))
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
			upKeyDown = 0;
			
			menuItem = (menuItem + 1) % 2;

			playSFX(SFX_MENU);
		}

		if (getKey(P1_DOWN) && downKeyDown == 0)
		{
			downKeyDown = 1;
		}
		else if (!getKey(P1_DOWN) && downKeyDown == 1)
		{
			downKeyDown = 0;
			
			menuItem = (menuItem + 1) % 2;
			
			playSFX(SFX_MENU);
		}

		if (getKey(P1_A) && aKeyDown == 0)
		{
			aKeyDown = 1;
		}
		else if (!getKey(P1_A) && aKeyDown == 1)
		{
			aKeyDown = 0;
			
			quit = 1;

			playSFX(SFX_MENU);
		}

		drawGameOverScreen(buffer, menuItem, -1, NULL);

		SDL_SoftStretch(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(17); //crude 60fps
	}
	
	stopAllSFX();
	
	if (hardCoreQuit == 1)
	{
		return -1;
	}

	switch (menuItem)
	{
		case 0:
		return 0;
		case 1:
		default:
		return 1;
	}
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
	
	stopBGM();

	return 0;
}

int main(int argc, char* argv[])
{
	int i;

	srand(time(NULL));
	currentState = TITLE; //should start at NONE

	if (init() != 0)
	{
		deinit();
		return 1;
	}

	//init error handling will go here
	
	for (i = 0; i < argc; i++)
	{
		if (argv[i] == NULL)
		{
			continue;
		}
		
		if (strcmp(argv[i], "nobgm") == 0)
		{
			toggleBGM(0);
		}
		if (strcmp(argv[i], "nosfx") == 0)
		{
			toggleSFX(0);
		}
	}
	
	loadHighScores();

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
				currentState = GETREADY;
				break;
				case 2:
				currentState = GETREADY2;
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
		else if (currentState == GETREADY)
		{
			if (getReadyScreen(screen, 1) == 1)
			{
				hardCoreQuit = 1;
			}

			currentState = INGAME;
		}
		else if (currentState == GETREADY2)
		{
			if (getReadyScreen(screen, 2) == 1)
			{
				hardCoreQuit = 1;
			}

			currentState = INGAME2;
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
			switch(gameOverLoop(screen))
			{
				case -1:
				hardCoreQuit = 1;
				break;
				case 0:
				switch (getPlayerCount())
				{
					case 1:
					currentState = INGAME;
					break;
					case 2:
					currentState = INGAME2;
					break;
					default:
					currentState = TITLE;
					break;
				}
				break;
				case 1:
				currentState = TITLE;
				break;
				default:
				currentState = TITLE;
				break;
			}
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

