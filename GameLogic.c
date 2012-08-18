#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "GameLogic.h"
#include "Pushdown.h"
#include "Entity.h"
#include "Audio.h"
#include "HighScore.h"
#include "Keyboard.h"

#define MAX_ONSCREEN_GAMEBLOCKS 300
#define MAX_ONSCREEN_ENEMIES 300
#define MAX_ONSCREEN_ICECREAM 30

#define INITAL_SECONDS_BETWEEN_ROBOTS 3000
#define REST_PERIOD_LENGTH 5500
#define INITAL_ROBOT_LIMIT_AMOUNT 5
#define ROBOTS_PER_WAVE 10

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

// if one player is dead but not both, then revive the dead player during the rest period
int reloadingHealth = 0; //0 -> not reloading, 1 -> reloading p1, 2 -> reloading p2
Uint32 reloadingDelayEffectTime = 0; //used to stagger life-giving qualities

// Each wave randomly picks two random block types and two random enemy types
BlockType initalBlockList[] = {RED_BLOCK, GREEN_BLOCK, BLUE_BLOCK, YELLOW_BLOCK};
BlockType waveTypeBlock1 = RED_BLOCK;
BlockType waveTypeBlock2 = GREEN_BLOCK;

EntityType initalEnemyList[] = {ENEMY_CRAWLER, ENEMY_SHOOTER, ENEMY_BOXERGREG, ENEMY_FAST};
EntityType waveTypeEnemy1 = ENEMY_CRAWLER;
EntityType waveTypeEnemy2 = ENEMY_SHOOTER;

int gameEnding = 0; //0 false, 1 true
Uint32 endGameDelta;

Uint32 lastUpdateTime = 0;

BGMType currentBGM = BGM_NONE;

unsigned int xrand(void)
{
	return rand () * (RAND_MAX + 1) + rand ();
}

//randomizes blocks and enemies per wave
void randomizeWaveTypes()
{
	int val = xrand() % 4;
	waveTypeBlock1 = initalBlockList[val];
	BlockType holdType = initalBlockList[3];
	initalBlockList[3] = waveTypeBlock1;
	initalBlockList[val] = holdType;
	val = xrand() % 3;
	waveTypeBlock2 = initalBlockList[val];
	printf("randomized block types: %d, %d\n", waveTypeBlock1, waveTypeBlock2);

	val = xrand() % 4;
	waveTypeEnemy1 = initalEnemyList[val];
	EntityType holdType2 = initalEnemyList[3];
	initalEnemyList[3] = waveTypeEnemy1;
	initalEnemyList[val] = holdType2;
	val = xrand() % 3;
	waveTypeEnemy2 = initalEnemyList[val];
	printf("randomized block types: %d, %d\n", waveTypeEnemy1, waveTypeEnemy2);
}

// picks new background music
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
	int i;

	if (restPeriodStart == 0)
	{
		restPeriodStart = getTimeSingleton();
		return;
	}
	
	if ((player1Health == 0 || player2Health == 0) && gameEnding == 0)
	{

		Entity** entList = getEntityList();
		
		if (entList != NULL)
		{
			for (i = 0; i < getEntityListSize(); i++)
			{
				if (entList[i]->base.type == PLAYER1)
				{
					if (entList[i]->player.dead == 2)
					{
						entList[i]->player.dead = 0;
						reloadingHealth = 1;
						reloadingDelayEffectTime = getTimeSingleton();
						
						player1Health++;
						playSFX(SFX_LIFEUP);
					}
				}

				if (entList[i]->base.type == PLAYER2)
				{
					if (entList[i]->player.dead == 2)
					{
						entList[i]->player.dead = 0;
						reloadingHealth = 2;
						reloadingDelayEffectTime = getTimeSingleton();
						
						player2Health++;
						playSFX(SFX_LIFEUP);
					}
				}
			}
		}
	}
	else if (reloadingHealth == 1)
	{
		if (getTimeSingleton() - reloadingDelayEffectTime > 500)
		{
			player1Health++;
			playSFX(SFX_LIFEUP);
			
			reloadingDelayEffectTime = getTimeSingleton();
			
			if (player1Health == 3)
			{
				reloadingHealth = 0;
			}
		}
	}
	else if (reloadingHealth == 2)
	{
		if (getTimeSingleton() - reloadingDelayEffectTime > 500)
		{
			player2Health++;
			playSFX(SFX_LIFEUP);
			
			reloadingDelayEffectTime = getTimeSingleton();
			
			if (player2Health == 3)
			{
				reloadingHealth = 0;
			}
		}
	}

	if ((getTimeSingleton() - restPeriodStart > REST_PERIOD_LENGTH && waveNumber > 0) || (getTimeSingleton() - restPeriodStart > 2 * REST_PERIOD_LENGTH && waveNumber == 0))
	{
		waveNumber++;
		restPeriod = 0;
		enemiesLeftToPush = ROBOTS_PER_WAVE;
		if (waveNumber >= 3)
		{
			maxOnScreenRobots += 3;
		}
		char msg[49];
		sprintf(msg, "WAVE %.2d", waveNumber);
		pushNewMessage(msg);
		
		randomizeWaveTypes();
		
		if (waveNumber > 1)
		{
			currentBGM = randomizeBGM(currentBGM);
			playBGM(currentBGM);
		}
		
		printf("max onscreen robots: %d\n", maxOnScreenRobots);
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
		if (enemyCount < MAX_ONSCREEN_ENEMIES && (entList[i]->type == ENEMY_FAST || entList[i]->type == ENEMY_SHOOTER || entList[i]->type == ENEMY_CRAWLER || entList[i]->type == ENEMY_BOXERGREG))
		{
			onScreenEnemies[enemyCount] = (Enemy*)(entList[i]);
			enemyCount++;
		}
		if (iceCreamCount < MAX_ONSCREEN_ICECREAM && entList[i]->type == ICECREAM && entList[i]->iceCream.decoy == 0)
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

				switch (xrand() % 2)
				{

					case 1:
					en = (Enemy*)pushEntity(waveTypeEnemy1, -1, (xrand() % 8) + 6);
					break;
					case 0:
					default:
					en = (Enemy*)pushEntity(waveTypeEnemy2, -1, (xrand() % 8) + 6);
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

						switch (xrand() % 2)
						{
							case 0:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = waveTypeBlock1;
					                break;
							case 1:
							newBlock = pushEntity(GAMEBLOCK, xSpot, ySpot);
					                newBlock->gBlock.bType = waveTypeBlock2;
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
			pushNewMessage("HARDCORE WAVE COMPLETE!");
			break;
			case 6:
			pushNewMessage("YOU JUST COMPLETED THAT WAVE!");
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
	
	if ( (player1Health < 1 && is2PlayerGame == 0) || (player1Health < 1 && player2Health < 1 && is2PlayerGame == 1) )
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
	char playerControlsMessage[50];

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
	
	sprintf(playerControlsMessage, "P1 Keys: %s=Strike, %s=grab block", SDL_GetKeyName(getSDLKeyValue(P1_B)), SDL_GetKeyName(getSDLKeyValue(P1_A)));
	pushNewMessage(playerControlsMessage);
	if (is2PlayerGame)
	{
		sprintf(playerControlsMessage, "P2 Keys: %s=Strike, %s=grab block", SDL_GetKeyName(getSDLKeyValue(P1_B)), SDL_GetKeyName(getSDLKeyValue(P1_A)));
		pushNewMessage(playerControlsMessage);
	}

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


