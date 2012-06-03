/* Whimsy Block Go
 * ---------------
 * A game for Windows written by Daniel Savage.
 *
 * For noncommercial use only.
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>

#include "Entity.h"
#include "Keyboard.h"
#include "Draw.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 448

SDL_Surface* screen;
SDL_Surface* buffer;

int init()
{
	// strangely prints stdout to stdout
	freopen( "CON", "w", stdout );

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		perror("Error initalizing SDL");
		return 1;
	}

	if ((screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE)) == NULL)
	{
		perror("Error initalizing screen");
		return 1;
	}
	
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	SDL_WM_SetCaption( "Whimsy Block Go", NULL );

	return 0;
}

int deinit()
{
	SDL_FreeSurface(buffer);

	SDL_Quit();
	
	return 0;
}

// perform game logic on all the connecting game blocks
// NOTE: this code uses a nested function, which is a feature of GCC;
//       if you find yourself cursed with porting such code using another
//       compiler (eg: ICC), you may need to remove the nest to a seperate 
//       function.
void whimsyBlocks()
{
	int i,j;
	Entity* gameBlockGrid[32][16];
	int flagMatrix[32][16];
	int connectedBlockCounter = 0;
	Entity** entList = getEntityList();
	
	void checkConnectedBlocks(int x, int y, BlockType b)
	{
		if (gameBlockGrid[x][y] == NULL)
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
		if (gameBlockGrid[x][y] == NULL)
		{
			return;
		}

		if (gameBlockGrid[x][y]->gBlock.bType != b)
		{
			return;
		}
		
		gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
		gameBlockGrid[x][y] = NULL;

		clearConnectedBlocks(x + 1, y, b);
		clearConnectedBlocks(x, y + 1, b);
		clearConnectedBlocks(x - 1, y, b);
		clearConnectedBlocks(x, y - 1, b);
	}

	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 16; j++)
		{
			gameBlockGrid[i][j] = NULL;
			flagMatrix[i][j] = 0;
		}
	}

	for (i = 0; i < getEntityListSize(); i++)
	{
		if (entList[i]->type == GAMEBLOCK)
		{
			if (entList[i]->base.x >= 0 && entList[i]->base.y >= 8 && entList[i]->base.x < 32 && entList[i]->base.y < 24)
			{
				gameBlockGrid[entList[i]->base.x][entList[i]->base.y] = entList[i];
				flagMatrix[entList[i]->base.x][entList[i]->base.y] = 1;
			}
		}
	}
	
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 16; j++)
		{
			connectedBlockCounter = 0;
			
			if (gameBlockGrid[i][j] != NULL)
			{
				checkConnectedBlocks(i, j, gameBlockGrid[i][j]->gBlock.bType);
				
				if (connectedBlockCounter > 3)
				{
					clearConnectedBlocks(i, j, gameBlockGrid[i][j]->gBlock.bType);
				}
			}
		}
	}
}

int testLoop()
{
	int i;   //loop iterators

	int quit = 0;
	SDL_Event ev;

	while (!quit)
	{
		while(SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_QUIT)
			{
				quit = 1;
			}
		}
		
		//update call
		pollKeyboard();

		Entity** entList = getEntityList();
		Uint32 currTime = SDL_GetTicks();
		for (i = 0; i < getEntityListSize(); i++)
		{
			if (entList[i]->type == DELETE_ME_PLEASE)
			{
				popEntity(entList[i]);
				i--;
				continue;
			}
			
			whimsyBlocks();

			update_entity(entList[i], currTime);
		}

		testDraw(buffer);
		
		SDL_BlitSurface(buffer, NULL, screen, NULL);
		SDL_Flip(screen);
		SDL_Delay(20);
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int i;
	
	srand(time(NULL));

	init();

	initEntityList();

	pushEntity(PLAYER1, 3, 19);

	for (i = 0; i < 32; i++)
	{
		pushEntity(PERMABLOCK, i, 8);
		pushEntity(PERMABLOCK, i, 24);
	}

	for (i = 0; i < 15; i++)
	{
		pushEntity(PERMABLOCK, -1, 9 + i);
		pushEntity(PERMABLOCK, 32, 9 + i);
	}
	
	pushEntity(ICEBLOCK, 10, 10);
	
	/*
	Entity* newGameBlock = pushEntity(GAMEBLOCK, 10, 10);
	newGameBlock->gBlock.bType = RED_BLOCK;

	newGameBlock = pushEntity(GAMEBLOCK, 12, 10);
	newGameBlock->gBlock.bType = BLUE_BLOCK;

	newGameBlock = pushEntity(GAMEBLOCK, 13, 10);
	newGameBlock->gBlock.bType = GREEN_BLOCK;  */
	
	for (i = 0; i < 14; i++)
	{
		Entity* newGameBlock = pushEntity(GAMEBLOCK, 1 + 2*i, 10);
                newGameBlock->gBlock.bType = RED_BLOCK;
                
                newGameBlock = pushEntity(GAMEBLOCK, 1 + 2*i, 15);
                newGameBlock->gBlock.bType = GREEN_BLOCK;
	}
	
	pushEntity(ENEMY_CRAWLER, 5, 12);

	testLoop();
	
	freeEntityList();
	
	deinit();

	return 0;
}

