#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "GameLogic.h"
#include "Pushdown.h"
#include "Entity.h"

#define MAX_ONSCREEN_GAMEBLOCKS 300
#define MAX_ONSCREEN_ENEMIES 300
#define MAX_ONSCREEN_ICECREAM 30

#define BETWEEN_WAVE_DELAY 3000
#define INITAL_SECONDS_BETWEEN_ROBOTS 3000
#define REST_PERIOD_LENGTH 5000

#define XRAND_MAX (RAND_MAX*(RAND_MAX + 2))

//game states:
// 0 -> no game running
// 1 -> regular game play
// 2 -> player health depleted
// 3 -> ice cream depleted
int gameState = 0;

int player1Health = 0;
int player2Health = 0;
int player1MaxHealth = 10;
int player2MaxHealth = 10;

int numberOfIceCreams = 0;

int gameScore = 0;
int waveNumber = 0;
int restPeriod = 0; //0 = not between waves; 1 = between waves
Uint32 restPeriodStart = 0;
Uint32 enemyPushInterval = INITAL_SECONDS_BETWEEN_ROBOTS;
Uint32 sinceLastEnemyOutput = 0;
int enemiesLeftToPush = 0;

int gameEnding = 0; //0 false, 1 true
Uint32 endGameDelta;

Uint32 lastUpdateTime = 0;

unsigned int xrand(void)
{
	return rand () * (RAND_MAX + 1) + rand ();
}

void updateRestPeriod()
{
	if (restPeriodStart == 0)
	{
		restPeriodStart = getTimeSingleton();
		return;
	}
	
	if (getTimeSingleton() - restPeriodStart > REST_PERIOD_LENGTH)
	{
		waveNumber++;
		restPeriod = 0;
		enemiesLeftToPush = 10;
		char msg[49];
		sprintf(msg, "WAVE %.2d", waveNumber);
		pushNewMessage(msg);
	}
}

void updateWave()
{
	int i;

	GameBlock* onScreenGameBlocks[MAX_ONSCREEN_GAMEBLOCKS];
	Enemy* onScreenEnemies[MAX_ONSCREEN_ENEMIES];
	IceCream* onScreenIceCream[MAX_ONSCREEN_ICECREAM];
	int gameBlockCount = 0;
	int enemyCount = 0;
	int iceCreamCount = 0;
	
	Entity** entList = getEntityList();
	
	for (i = 0; i < getEntityListSize(); i++)
	{
		if (enemyCount < MAX_ONSCREEN_ENEMIES && (entList[i]->type == ENEMY_SHOOTER || entList[i]->type == ENEMY_CRAWLER || entList[i]->type == ENEMY_BOXERGREG))
		{
			onScreenEnemies[enemyCount] = (Enemy*)(entList[i]);
			enemyCount++;
		}
		if (iceCreamCount < MAX_ONSCREEN_ICECREAM && entList[i]->type == ICECREAM)
		{
			onScreenIceCream[iceCreamCount] = (IceCream*)(entList[i]);
			iceCreamCount++;
		}
		if (gameBlockCount < MAX_ONSCREEN_GAMEBLOCKS && entList[i]->type == GAMEBLOCK)
		{
			onScreenGameBlocks[gameBlockCount] = (GameBlock*)(entList[i]);
			gameBlockCount++;
		}
	}

	if (lastUpdateTime - getTimeSingleton() > 1000)
	{
		unsigned int val = xrand() % 10000;

		if (enemyCount < 5 && enemiesLeftToPush > 0)
		{
			if (getTimeSingleton() - sinceLastEnemyOutput > enemyPushInterval)
			{
				Enemy* en;
				int enemyMod = 0;
				
				if (waveNumber > 2)
				{
					unsigned int highDifVal = val % 100;
					if (highDifVal < 10)
					{
						enemyMod = 2;
					}
					else if (highDifVal < 40)
					{
						enemyMod = 1;
					}
				}
				else if (waveNumber > 1)
				{
					unsigned int medDifVal = val % 100;
					
					if (medDifVal < 30)
					{
						enemyMod = 1;
					}
				}

				switch (enemyMod)
				{
					case 2:
					en = (Enemy*)pushEntity(ENEMY_SHOOTER, -1, (xrand() % 8) + 6);
					break;
					case 1:
					en = (Enemy*)pushEntity(ENEMY_BOXERGREG, -1, (xrand() % 8) + 6);
					break;
					case 0:
					default:
					en = (Enemy*)pushEntity(ENEMY_BOXERGREG, -1, (xrand() % 8) + 6);
					break;
				}
				en->direction = 1;
				
				if (xrand() % 2 == 0)
				{
					en->x += BOARD_WIDTH + 1;
					en->direction = 3;
				}

				sinceLastEnemyOutput = getTimeSingleton();
				enemiesLeftToPush--;
			}
		}

		if (gameBlockCount < 20)
		{
			if(val / 20 < 4)
			{
				int i;
				int xSpot = -1;
				int ySpot = -1;

				for (i = 0; i < 5; i++)
				{
					xSpot = (xrand() % (BOARD_WIDTH - 4)) + 2;
					ySpot = (xrand() % 8) + 6;
					Entity* checkList[5];
					int checkResultSize = 0;
					occupyingOnHere(xSpot, ySpot, checkList, 5, &checkResultSize);
					
					if (checkResultSize == 0)
					{
						Entity* newBlock;
						switch (xrand() % 4)
						{
							case 0:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = RED_BLOCK;
					                break;
							case 1:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = BLUE_BLOCK;
					                break;
							case 2:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = GREEN_BLOCK;
					                break;
							case 3:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = YELLOW_BLOCK;
					                break;
							default:
							break;
						}
						break;
					}
				}
			}
		}
		
		lastUpdateTime = getTimeSingleton();
	}
	
	if (enemiesLeftToPush == 0 && enemyCount == 0)
	{
		restPeriod = 1;
		restPeriodStart = getTimeSingleton();
	}

	if (iceCreamCount < numberOfIceCreams && iceCreamCount > 0)
	{
		pushNewMessage("ICE CREAM LOST!");
	}
	numberOfIceCreams = iceCreamCount;
}

void updateGameLogic()
{
	if (gameState == 0)
	{
		return;
	}

	if (lastUpdateTime == 0)
	{
		lastUpdateTime = getTimeSingleton();
		return;
	}
	
	if (player1Health < 1)
	{
		if (gameEnding == 0)
		{
			gameEnding = 1;
			endGameDelta = getTimeSingleton();
		}
		else if (gameEnding == 1)
		{
			if (getTimeSingleton() - endGameDelta > 5 * 1000)
			{
				gameState = 2;
			}
		}
	}
	
	if (numberOfIceCreams < 1)
	{
		if (gameEnding == 0)
		{
			pushNewMessage("Ice Cream Depleted!");
			gameEnding = 1;
			endGameDelta = getTimeSingleton();
		}
		else if (gameEnding == 1)
		{
			if (getTimeSingleton() - endGameDelta > 7 * 1000)
			{
				gameState = 3;
			}
		}
	}

	if (restPeriod == 0)
	{
		updateWave();
	}
	else if (restPeriod == 1)
	{
		updateRestPeriod();
	}
}

int clearResetGame()
{
	int i;
	
	gameState = 0;

	player1Health = 10;
	player2Health = 10;
	
	gameScore = 0;
	gameEnding = 0;
	
	waveNumber = 0;
	
	if (getEntityList() != NULL)
	{
		freeEntityList();
	}
	
	initEntityList();
	
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
	
	return 0;
}

int beginGame()
{
	if (gameState != 0)
	{
		return 1;
	}

	gameState = 1;
	
	restPeriod = 1;
	
	numberOfIceCreams = 8; //quick fix

	return 0;
}

int getPlayerHealth(int playerNo)
{
	switch (playerNo)
	{
		case 1:
		return player1Health;
		case 2:
		return player2Health;
		default:
		return -1;
	}
}

int getPlayerMaxHealth(int playerNo)
{
	switch (playerNo)
	{
		case 1:
		return player1MaxHealth;
		case 2:
		return player2MaxHealth;
		default:
		return -1;
	}
}

void modPlayerHealth(int playerNo, int delta)
{
	switch (playerNo)
	{
		case 1:
		player1Health += delta;
		break;
		case 2:
		player2Health += delta;
		break;
		default:
		break;
	}
	
	if (player1Health > player1MaxHealth)
	{
		player1Health = player1MaxHealth;
	}	
	else if (player1Health < 0)
	{
		player1Health = 0;
	}
	if (player2Health > player2MaxHealth)
	{
		player2Health = player2MaxHealth;
	}
	else if (player2Health < 0)
	{
		player2Health = 0;
	}
}

void modPlayermaxHealth(int playerNo, int delta)
{
	switch (playerNo)
	{
		case 1:
		player1MaxHealth += delta;
		break;
		case 2:
		player2MaxHealth += delta;
		break;
		default:
		break;
	}

	if (player1MaxHealth > 99)
	{
		player1MaxHealth = 99;
	}	
	else if (player1MaxHealth < 0)
	{
		player1MaxHealth = 0;
	}
	if (player2MaxHealth > 99)
	{
		player2MaxHealth = 99;
	}
	else if (player2MaxHealth < 0)
	{
		player2MaxHealth = 0;
	}
}

int getIceCreamCount()
{
	return numberOfIceCreams;
}

int getGameState()
{
	return gameState;
}


