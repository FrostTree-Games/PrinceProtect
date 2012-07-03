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
#include <SDL/SDL_ttf.h>

#include "Entity.h"
#include "Keyboard.h"
#include "Draw.h"
#include "Pushdown.h"
#include "GameLogic.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 272

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
	
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
	
	SDL_WM_SetCaption( "Whimsy Block Go", NULL );

	return 0;
}

int deinit()
{
	SDL_FreeSurface(buffer);
	
	clearAssets();

	TTF_Quit();
	SDL_Quit();
	
	return 0;
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
		int i;

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
		switch (b)
		{
			case RED_BLOCK:
			gameBlockGrid[x][y]->type = EXPLOSION;
			gameBlockGrid[x][y]->exp.startTime = getTimeSingleton();
			if (gameBlockGrid[x-1][y] == NULL)
			{
				gameBlockGrid[x-1][y] = pushEntity(EXPLOSION, x-1, y);
			}
			if (gameBlockGrid[x+1][y] == NULL)
			{
				gameBlockGrid[x+1][y] = pushEntity(EXPLOSION, x+1, y);
			}
			if (gameBlockGrid[x][y-1] == NULL)
			{
				gameBlockGrid[x][y-1] = pushEntity(EXPLOSION, x, y-1);
			}
			if (gameBlockGrid[x][y+1] == NULL)
			{
				gameBlockGrid[x][y+1] = pushEntity(EXPLOSION, x, y+1);
			}
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
			break;
			case YELLOW_BLOCK:
			for (i = 0; i < 4; i++)
			{
				Entity* offShoot = pushEntity(LASER, x, y);
				offShoot->laser.direction = i;
				offShoot->laser.allegiance = 1;
			}
			
			gameBlockGrid[x][y]->type = DELETE_ME_PLEASE;
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
			
			if (entList[i]->base.y < 5 && entList[i]->base.y > -1)
			{
				printf("what? %d\n", entList[i]->type);
			}
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

	pushEntity(PLAYER1, 3, 11);

	for (i = -2; i < BOARD_WIDTH + 3; i++)
	{
		pushEntity(PERMABLOCK, i, BOARD_TOP_WALL);
		pushEntity(PERMABLOCK, i, BOARD_BOTTOM_WALL);
	}

	for (i = 0; i < BOARD_HEIGHT + 2; i++)
	{
		pushEntity(PERMABLOCK, -2, 5 + i);
		pushEntity(PERMABLOCK, BOARD_WIDTH + 1, 5 + i);
	}

	for (i = 0; i < 2; i++)
	{
		pushEntity(ICECREAM, 9 + i, 7);
		pushEntity(ICECREAM, 9 + i, 8);

		pushEntity(ICECREAM, 9 + i, 11);
		pushEntity(ICECREAM, 9 + i, 12);
	}
	
	for (i = 0; i < 4; i++)
	{
		Entity* en = pushEntity(GAMEBLOCK, 2 + i*2, BOARD_TOP_WALL + 3);
		en->gBlock.bType = RED_BLOCK;
	}

	clearResetGame();
	
	beginGame();

	testLoop();
	
	freeEntityList();
	
	deinit();

	return 0;
}

