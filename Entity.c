#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Entity.h"
#include "Keyboard.h"

// Master list of all entities in play
Entity** entityList = NULL; // set to NULL when uninitalized
const int entityListMaxSize = 532; // the area is a 32 by 16 tile zone (plus 20 extra overhead locations)
int entityListCurrentSize = 0;

Entity* create_entity(EntityType type, int newX, int newY)
{
	Entity* newEntity = malloc(sizeof(Entity));
	newEntity->type = type;
	
	switch (type)
	{
		case PLAYER1:
		newEntity->player.x = newX;
		newEntity->player.y = newY;
                newEntity->player.upKeyDown = 0;
                newEntity->player.downKeyDown = 0;
                newEntity->player.leftKeyDown = 0;
                newEntity->player.rightKeyDown = 0;
                newEntity->player.lastUpdate = 0;
		break;
		case PERMABLOCK:
		newEntity->permaBlock.x = newX;
		newEntity->permaBlock.y = newY;
		break;
		case GAMEBLOCK:
		newEntity->gBlock.x = newX;
		newEntity->gBlock.y = newY;
		newEntity->gBlock.bType = GREEN_BLOCK;
		break;
		default:
		free(newEntity);
		return NULL;
	}
	
	return newEntity;
}

void destroy_entity(Entity* entity)
{
	free(entity);
}

int initEntityList()
{
	if (entityList != NULL)
	{
		return 2;
	}
	
	entityList = malloc(sizeof(Entity*) * entityListMaxSize);
	
	if (entityList == NULL)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int freeEntityList()
{
	int i;

	if (entityList == NULL)
	{
		return 2;
	}
	
	for (i = 0; i < entityListCurrentSize; i++)
	{
		destroy_entity(entityList[i]);
	}

	free(entityList);
	
	entityList = NULL;
	
	return 1;
}

int getEntityListSize()
{
	return entityListCurrentSize;
}

int getEntityListMaxSize()
{
	return entityListMaxSize;
}

Entity** getEntityList()
{
	return entityList;
}

Entity* pushEntity(EntityType type, int newX, int newY)
{
	if (entityList == NULL)
	{
		return NULL;
	}

	if (entityListCurrentSize >= entityListMaxSize)
	{
		return NULL;
	}

	Entity* newEnt = create_entity(type, newX, newY);
	
	if (newEnt == NULL)
	{
		return NULL;
	}
	else
	{
		entityList[entityListCurrentSize] = newEnt;
		entityListCurrentSize++;
		return newEnt;
	}
}

int popEntity(Entity* entity)
{
	int i;

	if (entityList == NULL)
	{
		return 0;
	}
	if (entityListCurrentSize < 1)
	{
		return 0;
	}

	for (i = 0; i < entityListCurrentSize; i++)
	{
		if (entity == entityList[i])
		{
			free(entity);
			entityList[i] = entityList[entityListCurrentSize - 1];
			entityListCurrentSize--;
			return 1;
		}
	}

	return 0;
}

int occupyingOnHere(int x, int y, Entity** list, int listMaxSize, int* returnedSize)
{
	int i;
	*returnedSize = 0;
	
	if (list == NULL)
	{
		return 0;
	}
	
	Entity* en;
	for (i = 0; i < entityListCurrentSize; i++)
	{
		en = entityList[i];
		
		if (en->base.x == x && en->base.y == y)
		{
			if ((*returnedSize) < listMaxSize)
			{
				list[(*returnedSize)] = en;
				(*returnedSize)++;
			}
			else
			{
				return 1;
			}
		}
	}
	
	if ((*returnedSize) == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* entity update functions */

void update_player(Player* pl, Uint32 currTime)
{
	int i;
	int xInc = 0;
	int yInc = 0;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	
	occupyingOnHere(pl->x, pl->y - 1, northList, 5, &northResultSize);
	occupyingOnHere(pl->x, pl->y + 1, southList, 5, &southResultSize);
	occupyingOnHere(pl->x + 1, pl->y, eastList, 5, &eastResultSize);
	occupyingOnHere(pl->x - 1, pl->y, westList, 5, &westResultSize);

	if (getKey(P1_UP) && pl->upKeyDown == 0)
	{
		yInc--;
		pl->direction = 0;

		if (northResultSize > 0)
		{
			for (i = 0; i < northResultSize; i++)
			{
				if (northList[i]->type == PERMABLOCK || northList[i]->type == GAMEBLOCK)
				{
					yInc++;
					break;
				}
			}
		}
		
		pl->upKeyDown = 1;
	}
	else if (!getKey(P1_UP) && pl->upKeyDown == 1)
	{
		pl->upKeyDown = 0;
	}

	if (getKey(P1_DOWN) && pl->downKeyDown == 0)
	{
		yInc++;
		pl->direction = 2;

		if (southResultSize > 0)
		{
			for (i = 0; i < southResultSize; i++)
			{
				if (southList[i]->type == PERMABLOCK  || southList[i]->type == GAMEBLOCK)
				{
					yInc--;
					break;
				}
			}
		}
		
		pl->downKeyDown = 1;
	}
	else if (!getKey(P1_DOWN) && pl->downKeyDown == 1)
	{
		pl->downKeyDown = 0;
	}

	if (getKey(P1_LEFT) && pl->leftKeyDown == 0)
	{
		xInc--;
		pl->direction = 3;

		if (westResultSize > 0)
		{
			for (i = 0; i < westResultSize; i++)
			{
				if (westList[i]->type == PERMABLOCK || westList[i]->type == GAMEBLOCK)
				{
					xInc++;
					break;
				}
			}
		}
		
		pl->leftKeyDown = 1;
	}
	else if (!getKey(P1_LEFT) && pl->leftKeyDown == 1)
	{
		pl->leftKeyDown = 0;
	}

	if (getKey(P1_RIGHT) && pl->rightKeyDown == 0)
	{
		xInc++;
		pl->direction = 1;

		if (eastResultSize > 0)
		{
			for (i = 0; i < eastResultSize; i++)
			{
				if (eastList[i]->type == PERMABLOCK || eastList[i]->type == GAMEBLOCK)
				{
					xInc--;
					break;
				}
			}
		}
		
		pl->rightKeyDown = 1;
	}
	else if (!getKey(P1_RIGHT) && pl->rightKeyDown == 1)
	{
		pl->rightKeyDown = 0;
	}

	pl->x += xInc;
	pl->y += yInc;

	return;
}

void update_entity(Entity* entity, Uint32 currTime)
{
	// just in terrible, terrible, case
	if (entity == NULL || currTime < 10)
	{
		return;
	}

	switch (entity->type)
	{
		case PLAYER1:
		case PLAYER2:
		update_player((Player*)entity, currTime);
		break;
		case PERMABLOCK:
		break;
		case GAMEBLOCK:
		break;
		default:
		printf("unregognized entity type updated\n");
		break;
	}
	
	return;
}
