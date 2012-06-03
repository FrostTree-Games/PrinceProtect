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
                newEntity->player.aKeyDown = 0;
                newEntity->player.lastUpdate = 0;
                newEntity->player.holding = NULL;
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
		case ICEBLOCK:
		newEntity->iBlock.x = newX;
		newEntity->iBlock.y = newY;
		newEntity->iBlock.moving = 0;
		newEntity->iBlock.direction = 0;
		newEntity->iBlock.offsetX = 8;
		newEntity->iBlock.offsetY = 8;
		break;
		case ENEMY_CRAWLER:
		newEntity->enemy.x = newX;
		newEntity->enemy.y = newY;
		newEntity->enemy.direction = 2;
		newEntity->enemy.offsetX = 8;
		newEntity->enemy.offsetY = 8;
		newEntity->enemy.lastMovementUpdate = 0;
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

int filterOccupyType(int x, int y, Entity** list, int listMaxSize, int* returnedSize, EntityType type)
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
			if ((*returnedSize) < listMaxSize && en->type == type)
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
	
	if (getKey(P1_A) && !(pl->aKeyDown))
	{
		pl->aKeyDown = 1;

		if (pl->holding == NULL)
		{
			if (pl->direction == 0)
			{
				if (northResultSize > 0)
				{
					for (i = 0; i < northResultSize; i++)
					{
						if (northList[i]->type == GAMEBLOCK)
						{
							pl->holding = (GameBlock*)(northList[i]);
							northList[i]->gBlock.x = -1;
							northList[i]->gBlock.y = -1;
							break;
						}
					}
				}
			}
			if (pl->direction == 1)
			{
				if (eastResultSize > 0)
				{
					for (i = 0; i < eastResultSize; i++)
					{
						if (eastList[i]->type == GAMEBLOCK)
						{
							pl->holding = (GameBlock*)(eastList[i]);
							eastList[i]->gBlock.x = -1;
							eastList[i]->gBlock.y = -1;
							break;
						}
					}
				}
			}
			if (pl->direction == 2)
			{
				if (southResultSize > 0)
				{
					for (i = 0; i < southResultSize; i++)
					{
						if (southList[i]->type == GAMEBLOCK)
						{
							pl->holding = (GameBlock*)(southList[i]);
							southList[i]->gBlock.x = -1;
							southList[i]->gBlock.y = -1;
							break;
						}
					}
				}
			}
			if (pl->direction == 3)
			{
				if (westResultSize > 0)
				{
					for (i = 0; i < westResultSize; i++)
					{
						if (westList[i]->type == GAMEBLOCK)
						{
							pl->holding = (GameBlock*)(westList[i]);
							westList[i]->gBlock.x = -1;
							westList[i]->gBlock.y = -1;
							break;
						}
					}
				}
			}
		}
		else
		{
			switch (pl->direction)
			{
				case 0:
				if (northResultSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y - 1;
					pl->holding = NULL;
				}
				break;
				case 1:
				if (eastResultSize == 0)
				{
					(pl->holding)->y = pl->y;
					(pl->holding)->x = pl->x + 1;
					pl->holding = NULL;
				}
				break;
				case 2:
				if (southResultSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y + 1;
					pl->holding = NULL;
				}
				break;
				case 3:
				if (westResultSize == 0)
				{
					(pl->holding)->y = pl->y;
					(pl->holding)->x = pl->x - 1;
					pl->holding = NULL;
				}
				break;
				default:
				fprintf(stderr, "Player direction has become invalid\n");
				break;
			}
		}
	}
	else if (!getKey(P1_A) && pl->aKeyDown)
	{
		pl->aKeyDown = 0;
	}

	if (getKey(P1_UP) && pl->upKeyDown == 0)
	{
		yInc--;
		pl->direction = 0;

		if (northResultSize > 0)
		{
			for (i = 0; i < northResultSize; i++)
			{
				if (northList[i]->type == PERMABLOCK || northList[i]->type == GAMEBLOCK || northList[i]->type == ICEBLOCK)
				{
					if (northList[i]->type == ICEBLOCK)
					{
						northList[i]->iBlock.moving = 1;
						northList[i]->iBlock.direction = pl->direction;
					}
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
				if (southList[i]->type == PERMABLOCK || southList[i]->type == GAMEBLOCK || southList[i]->type == ICEBLOCK)
				{
					if (southList[i]->type == ICEBLOCK)
					{
						southList[i]->iBlock.moving = 1;
						southList[i]->iBlock.direction = pl->direction;
					}
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
				if (westList[i]->type == PERMABLOCK || westList[i]->type == GAMEBLOCK || westList[i]->type == ICEBLOCK)
				{
					if (westList[i]->type == ICEBLOCK)
					{
						westList[i]->iBlock.moving = 1;
						westList[i]->iBlock.direction = pl->direction;
					}
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
				if (eastList[i]->type == PERMABLOCK || eastList[i]->type == GAMEBLOCK || eastList[i]->type == ICEBLOCK)
				{
					if (eastList[i]->type == ICEBLOCK)
					{
						eastList[i]->iBlock.moving = 1;
						eastList[i]->iBlock.direction = pl->direction;
					}
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

void update_enemy(Enemy* enemy, Uint32 currTime)
{
	Uint32 delta = currTime - enemy->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;

	occupyingOnHere(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
	occupyingOnHere(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
	occupyingOnHere(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
	occupyingOnHere(enemy->x - 1, enemy->y, westList, 5, &westResultSize);

	if (delta / 32 > 0)
	{
		switch (enemy->direction)
		{
			case 0:
			enemy->offsetY -= 1;
			break;
			case 1:
			enemy->offsetX += 1;
			break;
			case 2:
			enemy->offsetY += 1;
			break;
			case 3:
			enemy->offsetX -= 1;
			break;
			default:
			break;
		}

		if (enemy->offsetX < 0)
		{
			if (westResultSize == 0)
			{
				enemy->offsetX = 15;
				enemy->x -= 1;
			}
			else
			{
				enemy->offsetX = 8;
				while (enemy->direction == 3)
				{
					enemy->direction = rand() % 4;
				}
			}
		}
		else if (enemy->offsetX > 15)
		{
			if (eastResultSize == 0)
			{
				enemy->offsetX = 0;
				enemy->x += 1;
			}
			else
			{
				enemy->offsetX = 8;
				while (enemy->direction == 1)
				{
					enemy->direction = rand() % 4;
				}
			}
		}
		if (enemy->offsetY < 0)
		{
			if (northResultSize == 0)
			{
				enemy->offsetY = 15;
				enemy->y -= 1;
			}
			else
			{
				enemy->offsetY = 8;
				while (enemy->direction == 0)
				{
					enemy->direction = rand() % 4;
				}
			}
		}
		else if (enemy->offsetY > 15)
		{
			if (southResultSize == 0)
			{
				enemy->offsetY = 0;
				enemy->y += 1;
			}
			else
			{
				enemy->offsetY = 8;
				while (enemy->direction == 2)
				{
					enemy->direction = rand() % 4;
				}
			}
		}
		
		enemy->lastMovementUpdate = currTime;
	}
}

void update_iceBlock(IceBlock* block, Uint32 currTime)
{
	Uint32 delta = currTime - block->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;

	occupyingOnHere(block->x, block->y - 1, northList, 5, &northResultSize);
	occupyingOnHere(block->x, block->y + 1, southList, 5, &southResultSize);
	occupyingOnHere(block->x + 1, block->y, eastList, 5, &eastResultSize);
	occupyingOnHere(block->x - 1, block->y, westList, 5, &westResultSize);

	if (delta / 32 > 0 && block->moving == 1)
	{
		switch (block->direction)
		{
			case 0:
			block->offsetY -= 8;
			break;
			case 1:
			block->offsetX += 8;
			break;
			case 2:
			block->offsetY += 8;
			break;
			case 3:
			block->offsetX -= 8;
			break;
			default:
			break;
		}

		if (block->offsetX < 0)
		{
			if (westResultSize == 0)
			{
				block->offsetX = 15;
				block->x -= 1;
			}
			else
			{
				block->offsetX = 8;
				block->moving = 0;
			}
		}
		else if (block->offsetX > 15)
		{
			if (eastResultSize == 0)
			{
				block->offsetX = 0;
				block->x += 1;
			}
			else
			{
				block->offsetX = 8;
				block->moving = 0;
			}
		}
		if (block->offsetY < 0)
		{
			if (northResultSize == 0)
			{
				block->offsetY = 15;
				block->y -= 1;
			}
			else
			{
				block->offsetY = 8;
				block->moving = 0;
			}
		}
		else if (block->offsetY > 15)
		{
			if (southResultSize == 0)
			{
				block->offsetY = 0;
				block->y += 1;
			}
			else
			{
				block->offsetY = 8;
				block->moving = 0;
			}
		}
		
		block->lastMovementUpdate = currTime;
	}
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
		case GAMEBLOCK:
		break;
		case ICEBLOCK:
		update_iceBlock((IceBlock*)entity, currTime);
		break;
		case ENEMY_CRAWLER:
		update_enemy((Enemy*)entity, currTime);
		break;
		default:
		printf("unregognized entity type updated\n");
		break;
	}
	
	return;
}
