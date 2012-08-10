#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "GameLogic.h"
#include "Pushdown.h"
#include "Entity.h"
#include "Audio.h"
#include "HighScore.h"

#define MAX_ONSCREEN_GAMEBLOCKS 300
#define MAX_ONSCREEN_ENEMIES 300
#define MAX_ONSCREEN_ICECREAM 30

#define INITAL_SECONDS_BETWEEN_ROBOTS 3000
#define REST_PERIOD_LENGTH 5500
#define INITAL_ROBOT_LIMIT_AMOUNT 5

#define XRAND_MAX (RAND_MAX*(RAND_MAX + 2))

//game states:
// 0 -> no game running
// 1 -> regular game play
// 2 -> player health depleted
// 3 -> ice cream depleted
int gameState = 0;
int is2PlayerGame = 0; //0 -> false, 1 -> true

int player1Health = 0;
int player2Health = 0;
int player1MaxHealth = 10;
int player2MaxHealth = 10;

int numberOfIceCreams = 0;

int gameScore = 0;
int waveNumber = 0;
int restPeriod = 0; //0 = not between waves; 1 = between waves
Uint32 restPeriodStart = 0;
Uint32 sinceLastEnemyOutput = 0;
int enemiesLeftToPush = 0;
int maxOnScreenRobots = INITAL_ROBOT_LIMIT_AMOUNT;
Uint32 enemyPushInterval = INITAL_SECONDS_BETWEEN_ROBOTS;

int gameEnding = 0; //0 false, 1 true
Uint32 endGameDelta;

Uint32 lastUpdateTime = 0;

BGMType currentBGM = BGM_NONE;

unsigned int xrand(void)
{
	return rand () * (RAND_MAX + 1) + rand ();
}

BGMType randomizeBGM(BGMType bg)
{
	if (bg != BGM_1 && bg != BGM_2 && bg != BGM_3)
	{
		return BGM_1;
	}
	
	if (bg == BGM_1)
	{
		switch (xrand() % 2)
		{
			case 0:
			return BGM_2;
			case 1:
			return BGM_3;
			default:
			return BGM_1;
		}
	}
	else if (bg == BGM_2)
	{
		switch (xrand() % 2)
		{
			case 0:
			return BGM_1;
			case 1:
			return BGM_3;
			default:
			return BGM_2;
		}
	}
	else if (bg == BGM_3)
	{
		switch (xrand() % 2)
		{
			case 0:
			return BGM_1;
			case 1:
			return BGM_2;
			default:
			return BGM_3;
		}
	}
	
	return BGM_1;
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
		enemiesLeftToPush = 20;
		if (waveNumber == 4)
		{
			enemiesLeftToPush = 40;
			maxOnScreenRobots += 3;
		}
		char msg[49];
		sprintf(msg, "WAVE %.2d", waveNumber);
		pushNewMessage(msg);
		
		if (waveNumber > 1)
		{
			currentBGM = randomizeBGM(currentBGM);
			playBGM(currentBGM);
		}
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

		if (enemyCount < maxOnScreenRobots && enemiesLeftToPush > 0)
		{
			if (getTimeSingleton() - sinceLastEnemyOutput > enemyPushInterval)
			{
				Enemy* en;

				if (waveNumber >= 5)
				{
					int enemyMod = 0;
					
					int val = xrand() % 100;
					
					if (val < 50)
					{
						enemyMod = 0;
					}
					else if (val < 75)
					{
						enemyMod = 1;
					}
					else
					{
						enemyMod = 2;
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
						en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, (xrand() % 8) + 6);
						break;
					}
				}
				else
				{
					switch (waveNumber)
					{
						case 1:
						en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, (xrand() % 8) + 6);
						break;
						case 2:
						if (xrand() % 5 == 0)
						{
							en = (Enemy*)pushEntity(ENEMY_SHOOTER, -1, (xrand() % 8) + 6);
						}
						else
						{
							en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, (xrand() % 8) + 6);
						}
						break;
						case 3:
						if (xrand() % 3 == 0)
						{
							en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, (xrand() % 8) + 6);
						}
						else
						{
							en = (Enemy*)pushEntity(ENEMY_BOXERGREG, -1, (xrand() % 8) + 6);
						}
						break;
						case 4:
						en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, (xrand() % 8) + 6);
						break;
						default:
						break;
					}
				}
				
				en->direction = 1;
				
				if (xrand() % 2 == 0)
				{
					en->x += BOARD_WIDTH + 1;
					en->direction = 3;
				}

				sinceLastEnemyOutput = getTimeSingleton();
				if (waveNumber == 4)
				{
					sinceLastEnemyOutput -= 1500;
				}

				enemiesLeftToPush--;
			}
		}

		if (gameBlockCount < 20 && waveNumber > 1)
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
						
						if (waveNumber >= 5)
						{
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
						}
						else
						{
							switch (waveNumber)
							{
								case 2:
								newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
						                newBlock->gBlock.bType = GREEN_BLOCK;
								break;
								case 3:
								newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
						                newBlock->gBlock.bType = RED_BLOCK;
								break;
								case 4:
								newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
								if (xrand() % 2 == 0)
								{
									newBlock->gBlock.bType = YELLOW_BLOCK;
								}
								else
								{
									newBlock->gBlock.bType = BLUE_BLOCK;
								}
								break;
							}
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
		
		switch (rand() % 10)
		{
			case 0:
			pushNewMessage("NICE ONE! WAVE COMPLETE!");
			break;
			case 1:
			pushNewMessage("WAVE COMPLETE! PARTY ROCK!");
			break;
			case 2:
			pushNewMessage("SO MUCH WAVE COMPLETE RIGHT NOW!");
			break;
			case 3:
			pushNewMessage("BOYFRIENDS SAFE! WAVE COMPLETE!");
			break;
			case 4:
			pushNewMessage("WAVE COMPLETE! MOMS LOVE YOU!");
			break;
                        case 5:
			pushNewMessage("WAVE IS COMPLETE AND PRINCES STILL EXIST!");
			break;
			case 6:
			pushNewMessage("YOU JUST COMPLETED THAT WAVE! NO WAY!");
			break;
			case 7:
			pushNewMessage("WAVE OF BADDIES BUSTED!");
			break;
			case 8:
			pushNewMessage("THIS WAVE IS SO OVER YOU GUYS!");
			break;
			case 9:
			pushNewMessage("WAVE CONQUERED AND STUFF!");
			break;
			default:
			pushNewMessage("WAVE COMPLETE!");
			break;
		}
		
		fadeBGM();
	}

	if (iceCreamCount < numberOfIceCreams && iceCreamCount > 0)
	{
		pushNewMessage("PRINCE BOYFRIEND LOST!");
		
		playSFX(SFX_LOSE_PRINCESS);
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
			pushNewMessage("no more princes!");
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

int clearResetGame(int playerCount)
{
	int i;

	gameState = 0;

	player1Health = 0;
	player2Health = 10;

	gameScore = 0;
	gameEnding = 0;
	
	waveNumber = 0;
	
	currentBGM = BGM_NONE;
	
	maxOnScreenRobots = INITAL_ROBOT_LIMIT_AMOUNT;
	enemyPushInterval = INITAL_SECONDS_BETWEEN_ROBOTS;

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
	
	switch (playerCount)
	{
		case 1:
		pushEntity(PLAYER1, BOARD_WIDTH/2, 9);
		is2PlayerGame = 0;
		break;
		case 2:
		pushEntity(PLAYER1, BOARD_WIDTH/2, 9);
                pushEntity(PLAYER2, BOARD_WIDTH/2 + 1, 9);
                is2PlayerGame = 1;
                break;
                default:
                printf("ODD PLAYER COUNT; FIX THIS AND MAKE IT ATOMIC");
                pushEntity(PLAYER1, BOARD_WIDTH/2, 9);
		is2PlayerGame = 0;
		break;
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
	
	setTimeSingleton(SDL_GetTicks());
	pushNewMessage("NINJAS WANT YOUR BOYFRIENDS!");
	pushNewMessage("YOU MUST PROTECT THEM!");
	
	currentBGM = BGM_1;
	playBGM(currentBGM);

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
	
	if (delta < 0)
	{
		playSFX(SFX_PLAYER_HURT);
	}

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

int getPlayerCount()
{
	if (gameState == 0)
	{
		return 0;
	}
	
	if (is2PlayerGame)
	{
		return 2;
	}
	else
	{
		return 1;
	}	
}

int getScore()
{
	return gameScore;
}

void addScore(int delta)
{
	gameScore += delta;
}


