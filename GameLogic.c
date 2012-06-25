#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "GameLogic.h"
#include "Entity.h"

#define MAX_ONSCREEN_GAMEBLOCKS 300
#define MAX_ONSCREEN_ENEMIES 300
#define MAX_ONSCREEN_ICECREAM 30

#define XRAND_MAX (RAND_MAX*(RAND_MAX + 2))

int gameState = 0;

int player1Health = 0;
int player2Health = 0;
int player1MaxHealth = 10;
int player2MaxHealth = 10;

int gameScore = 0;
int waveNumber = 0;
int betweenWaves = 0; //0 = not between waves; 1 = between waves

Uint32 lastUpdateTime = 0;

unsigned int xrand(void)
{
	return rand () * (RAND_MAX + 1) + rand ();
}

void updateGameLogic()
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
		if (enemyCount < MAX_ONSCREEN_ENEMIES && (entList[i]->type == ENEMY_SHOOTER || entList[i]->type == ENEMY_CRAWLER))
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
		
		//
	}

	if (lastUpdateTime == 0)
	{
		lastUpdateTime = getTimeSingleton();
		return;
	}
	
	if (lastUpdateTime - getTimeSingleton() > 1000)
	{
		unsigned int val = xrand() % 10000;

		if (enemyCount < 9)
		{
			if (val / 10 == 2)
			{
				Enemy* en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, xrand() % 6 + 6);
				en->direction = 1;
			}
			else if (val / 10 == 1)
			{
				Enemy* en = (Enemy*)pushEntity(ENEMY_SHOOTER, 0, xrand() % 6 + 6);
				en->direction = 1;
			}
		}
		
		if (gameBlockCount < 20)
		{
			if(val / 20 < 7)
			{
				int i;
				int xSpot = -1;
				int ySpot = -1;
				
				for (i = 0; i < 5; i++)
				{
					xSpot = (xrand() % 28) + 2;
					ySpot = (xrand() % 6) + 6;
					Entity* checkList[5];
					int checkResultSize = 0;
					occupyingOnHere(xSpot, ySpot, checkList, 5, &checkResultSize);
					
					if (checkResultSize == 0)
					{
						Entity* newBlock;
						switch (xrand() % 3)
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
							default:
							break;
						}
						break;
					}
				}
			}
		}
	}
}

int clearResetGame()
{
	gameState = 0;

	player1Health = 10;
	player2Health = 10;
	
	gameScore = 0;
	
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


