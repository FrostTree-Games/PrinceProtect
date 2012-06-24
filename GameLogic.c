#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "GameLogic.h"
#include "Entity.h"

#define XRAND_MAX (RAND_MAX*(RAND_MAX + 2))

int gameState = 0;

int player1Health = 0;
int player2Health = 0;
int player1MaxHealth = 10;
int player2MaxHealth = 10;

int gameScore = 0;

Uint32 lastUpdateTime = 0;

unsigned int xrand(void)
{
	return rand () * (RAND_MAX + 1) + rand ();
}

void updateGameLogic()
{
	if (lastUpdateTime == 0)
	{
		lastUpdateTime = getTimeSingleton();
		return;
	}
	
	if (lastUpdateTime - getTimeSingleton() > 1000)
	{
		unsigned int val = xrand() % 10000;
		
		if (getEntityListSize() < 180)
		{
			if (val / 10 == 2)
			{
				Enemy* en = (Enemy*)pushEntity(ENEMY_CRAWLER, -1, xrand() % 11 + 9);
				en->direction = 1;
			}
			else if (val / 10 == 1)
			{
				Enemy* en = (Enemy*)pushEntity(ENEMY_SHOOTER, 0, xrand() % 9 + 11);
				en->direction = 1;
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


