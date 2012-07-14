#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Entity.h"
#include "Keyboard.h"
#include "GameLogic.h"

#define PLAYER_WALK_SPEED 3

// Master list of all entities in play
Entity** entityList = NULL; // set to NULL when uninitalized
const int entityListMaxSize = 552; // the area is a 32 by 16 tile zone (plus 40 extra overhead locations)
int entityListCurrentSize = 0;

// THESE SHOULD BE EVENTUALLY MOVED
Uint32 timeSingleton = 0;
Uint32 getTimeSingleton()
{
	return timeSingleton;
}
void setTimeSingleton(Uint32 newTime)
{
	timeSingleton = newTime;
}

int xor(int value1, int value2)
{
	return ( (value1 && !value2) || (!value1 && value2) );
}

void pair_teleblocks(TeleBlock* placed, TeleBlock* mirror)
{
	placed->twin = mirror;
	mirror->twin = placed;
}

Entity* create_entity(EntityType type, int newX, int newY)
{
	Entity* newEntity = malloc(sizeof(Entity));
	newEntity->type = type;

	switch (type)
	{
		case PLAYER1:
		newEntity->player.x = newX;
		newEntity->player.y = newY;
		newEntity->player.offsetX = 8;
		newEntity->player.offsetY = 8;
                newEntity->player.upKeyDown = 0;
                newEntity->player.downKeyDown = 0;
                newEntity->player.leftKeyDown = 0;
                newEntity->player.rightKeyDown = 0;
                newEntity->player.aKeyDown = 0;
                newEntity->player.lastUpdate = 0;
                newEntity->player.holding = NULL;
                newEntity->player.swordTimer = 0;
                newEntity->player.isThrusting = 0;
                newEntity->player.thrustHit = 0;
                newEntity->player.knockBackDirection = 255;
                newEntity->player.lastFrameUpdate = getTimeSingleton();
                newEntity->player.frame = 0;
                newEntity->player.holdingSuperHammer = 0;
                newEntity->player.dead = 0;
		break;
		case PLAYER2:
		newEntity->player.x = newX;
		newEntity->player.y = newY;
		newEntity->player.offsetX = 8;
		newEntity->player.offsetY = 8;
                newEntity->player.upKeyDown = 0;
                newEntity->player.downKeyDown = 0;
                newEntity->player.leftKeyDown = 0;
                newEntity->player.rightKeyDown = 0;
                newEntity->player.aKeyDown = 0;
                newEntity->player.lastUpdate = 0;
                newEntity->player.holding = NULL;
                newEntity->player.swordTimer = 0;
                newEntity->player.isThrusting = 0;
                newEntity->player.knockBackDirection = 255;
                newEntity->player.lastFrameUpdate = getTimeSingleton();
                newEntity->player.frame = 0;
                newEntity->player.holdingSuperHammer = 0;
                newEntity->player.dead = 0;
		break;
		case PERMABLOCK:
		newEntity->permaBlock.x = newX;
		newEntity->permaBlock.y = newY;
		break;
		case GAMEBLOCK:
		newEntity->gBlock.x = newX;
		newEntity->gBlock.y = newY;
		newEntity->gBlock.bType = GREEN_BLOCK;
		newEntity->gBlock.height = 100;
		newEntity->gBlock.lastUpdateTime = 0;
		newEntity->gBlock.startTime = getTimeSingleton();
		break;  
		case ICEBLOCK:
		newEntity->iBlock.x = newX;
		newEntity->iBlock.y = newY;
		newEntity->iBlock.moving = 0;
		newEntity->iBlock.direction = 0;
		newEntity->iBlock.offsetX = 8;
		newEntity->iBlock.offsetY = 8;
		newEntity->iBlock.health = 3;
		newEntity->iBlock.startTime = getTimeSingleton();
		break;
		case EXPLOSION:
		newEntity->exp.x = newX;
		newEntity->exp.y = newY;
		newEntity->exp.startTime = getTimeSingleton();
		break;
		case LASER:
		newEntity->laser.x = newX;
		newEntity->laser.y = newY;
		newEntity->laser.direction = 0;
		newEntity->laser.offsetX = 8;
		newEntity->laser.offsetY = 8;
		newEntity->laser.allegiance = 0;
		break;
		case TELEBLOCK:
		newEntity->tBlock.x = newX;
		newEntity->tBlock.y = newY;
		newEntity->tBlock.startTime = getTimeSingleton();
		newEntity->tBlock.lastClickTime = getTimeSingleton();
		newEntity->tBlock.frame = 0;
		if (newX < 32/2)
		{
			newEntity->tBlock.side = 0;
		}
		else
		{
			newEntity->tBlock.side = 1;
		}
		break;
		case ICECREAM:
		newEntity->iceCream.x = newX;
		newEntity->iceCream.y = newY;
		break;
		case SUPERHAMMER:
		newEntity->hammer.x = newX;
		newEntity->hammer.y = newY;
		break;
		case GLUE:
		newEntity->glue.x = newX;
		newEntity->glue.y = newY;
		newEntity->glue.startTime = getTimeSingleton();
		break;
		case ENEMY_CRAWLER:
		newEntity->enemy.x = newX;
		newEntity->enemy.y = newY;
		newEntity->enemy.direction = rand() % 4;
		newEntity->enemy.offsetX = 8;
		newEntity->enemy.offsetY = 8;
		newEntity->enemy.lastMovementUpdate = 0;
		newEntity->enemy.knockBackDirection = -1;
		newEntity->enemy.cream = NULL;
		newEntity->enemy.health = 3;
		newEntity->enemy.AISlot1 = 0;
		newEntity->enemy.AISlot2 = 0;
		newEntity->enemy.AISlot3 = 0;
		newEntity->enemy.timer = 0;
		newEntity->enemy.lastFrameUpdate = getTimeSingleton();
		newEntity->enemy.frame = 0;
		break;
		case ENEMY_SHOOTER:
		newEntity->enemy.x = newX;
		newEntity->enemy.y = newY;
		newEntity->enemy.direction = rand() % 5;
		newEntity->enemy.offsetX = 8;
		newEntity->enemy.offsetY = 8;
		newEntity->enemy.lastMovementUpdate = 0;
		newEntity->enemy.knockBackDirection = -1;
		newEntity->enemy.cream = NULL;
		newEntity->enemy.health = 5;
		newEntity->enemy.AISlot1 = 0;
		newEntity->enemy.AISlot2 = 0;
		newEntity->enemy.AISlot3 = 0;
		newEntity->enemy.lastFrameUpdate = getTimeSingleton();
		newEntity->enemy.frame = 0;
		break;
		case ENEMY_BOXERGREG:
		newEntity->enemy.x = newX;
		newEntity->enemy.y = newY;
		newEntity->enemy.direction = rand() % 5;
		newEntity->enemy.offsetX = 8;
		newEntity->enemy.offsetY = 8;
		newEntity->enemy.lastMovementUpdate = 0;
		newEntity->enemy.knockBackDirection = -1;
		newEntity->enemy.cream = NULL;
		newEntity->enemy.health = 4;
		newEntity->enemy.AISlot1 = 0;
		newEntity->enemy.AISlot2 = 0;
		newEntity->enemy.AISlot3 = 0;
		newEntity->enemy.lastFrameUpdate = getTimeSingleton();
		newEntity->enemy.frame = 0;
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
	
	entityListCurrentSize = 0;
	
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
	//if a TeleBlock is specified, this inline is called to create a reciporical TeleBlock
	Entity* pushPairBlock(int newX, int newY)
	{
		if (entityList == NULL)
		{
			return NULL;
		}
	
		if (entityListCurrentSize >= entityListMaxSize)
		{
			return NULL;
		}
		
		Entity* checkList[5];
		int checkResultSize = 0;
		occupyingOnHere(BOARD_WIDTH - newX, newY, checkList, 5, &checkResultSize);

		if (checkResultSize > 0)
		{
			return NULL;
		}

		Entity* newEnt = create_entity(TELEBLOCK, BOARD_WIDTH - newX, newY);

		if (newEnt == NULL)
		{
			return NULL;
		}
		
		entityList[entityListCurrentSize] = newEnt;
		entityListCurrentSize++;
		return newEnt;
	}
	
	// if a teleBlock is going to be created, this pointer is used as its twin
	Entity* potentialTwin = NULL;

	if (entityList == NULL)
	{
		return NULL;
	}

	if (entityListCurrentSize >= entityListMaxSize)
	{
		return NULL;
	}
	
	if (type == TELEBLOCK)
	{
		potentialTwin = pushPairBlock(newX, newY);
		
		if (potentialTwin == NULL)
		{
			return NULL;
		}
	}

	Entity* newEnt = create_entity(type, newX, newY);
	
	if (type == TELEBLOCK)
	{
		pair_teleblocks((TeleBlock*)newEnt, (TeleBlock*)potentialTwin);
	}

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

int filterOccupyWalls(int x, int y, Entity** list, int listMaxSize, int* returnedSize)
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
			if ((*returnedSize) < listMaxSize && (en->type == PERMABLOCK || en->type == GAMEBLOCK || en->type == ICEBLOCK || en->type == ENEMY_SHOOTER || en->type == PLAYER1 || en->type == PLAYER2))
			{
				list[(*returnedSize)] = en;
				(*returnedSize)++;
			}

			if ((*returnedSize) >= listMaxSize)
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

// je suis lazy
int filterOccupyWallsForPlayer(int x, int y, Entity** list, int listMaxSize, int* returnedSize)
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

			if ((en->type == PERMABLOCK || en->type == GAMEBLOCK || en->type == ICEBLOCK || en->type == ICECREAM || en->type == ENEMY_CRAWLER || en->type == ENEMY_BOXERGREG || en->type == ENEMY_SHOOTER))
			{
				list[(*returnedSize)] = en;
				(*returnedSize)++;
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
	int i,j;
	int xInc = 0;
	int yInc = 0;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currResultSize;
	
	Entity* northListFull[5];
	Entity* southListFull[5];
	Entity* eastListFull[5];
	Entity* westListFull[5];
	int northResultFullSize;
	int southResultFullSize;
	int eastResultFullSize;
	int westResultFullSize;
	
	occupyingOnHere(pl->x, pl->y - 1, northListFull, 5, &northResultFullSize);
	occupyingOnHere(pl->x, pl->y + 1, southListFull, 5, &southResultFullSize);
	occupyingOnHere(pl->x + 1, pl->y, eastListFull, 5, &eastResultFullSize);
	occupyingOnHere(pl->x - 1, pl->y, westListFull, 5, &westResultFullSize);

	filterOccupyWallsForPlayer(pl->x, pl->y - 1, northList, 5, &northResultSize);
	filterOccupyWallsForPlayer(pl->x, pl->y + 1, southList, 5, &southResultSize);
	filterOccupyWallsForPlayer(pl->x + 1, pl->y, eastList, 5, &eastResultSize);
	filterOccupyWallsForPlayer(pl->x - 1, pl->y, westList, 5, &westResultSize);
	occupyingOnHere(pl->x, pl->y, currList, 5, &currResultSize);
	
	//quick and hasty animation
	if (currTime - pl->lastFrameUpdate > 250)
	{
		if (pl->frame == 0)
		{
			pl->frame = 1;
		}
		else
		{
			pl->frame = 0;
		}
		
		pl->lastFrameUpdate = currTime;
	}
	
	if (pl->dead == 1)
	{
		return;
	}
	
	if (getPlayerHealth(1) < 1)
	{
		pl->dead = 1;
	}
	
	//this is a quick way of preventing block timeout
	if (pl->holding != NULL)
	{
		pl->holding->startTime = getTimeSingleton();
	}
	
	// if the player gets completely knocked back, then he/she cannot do logic for that iteration
	if (pl->knockBackDirection != 255)
	{
		switch(pl->knockBackDirection)
		{
			case 0:
			if (northResultSize == 0)
			{
				pl->y -= 1;
			}
			break;
			case 1:
			if (eastResultSize == 0 && pl->x + 1 < BOARD_WIDTH)
			{
				pl->x += 1;
			}
			break;
			case 2:
			if (southResultSize == 0)
			{
				pl->y += 1;
			}
			break;
			case 3:
			if (westResultSize == 0 && pl->x - 1 >= 0)
			{
				pl->x -= 1;
			}
			break;
			default:
			printf("Player knock back all odd! value:%d\n", pl->knockBackDirection);
			break;
		}

		pl->knockBackDirection = 255;
		return;
	}
	
	for (i = 0; i < currResultSize; i++)
	{
		if (currList[i]->type == SUPERHAMMER && pl->holdingSuperHammer == 0)
		{
			pl->holdingSuperHammer = 1;
			currList[i]->type = DELETE_ME_PLEASE;
			break;
		}
	}
	
	if (getKey(P1_B) && !(pl->bKeyDown))
	{
		pl->bKeyDown = 1;
		pl->swordTimer = currTime;
		pl->isThrusting = 1;
		pl->thrustHit = 1;
	}
	else if (!getKey(P1_B) && pl->bKeyDown)
	{
		pl->bKeyDown = 0;
	}

	if (pl->isThrusting)
	{
		if (pl->holdingSuperHammer == 1)
		{
			pl->holdingSuperHammer = 0;
			pl->thrustHit = 0;

			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
				{
					pushEntity(EXPLOSION, pl->x - 1 + i, pl->y - 1 + j);
				}
			}
		}

		if (currTime - pl->swordTimer > 150)
		{
			pl->isThrusting = 0;
			pl->thrustHit = 0;
		}

		switch (pl->direction)
		{
			case 0:
			if (northResultSize > 0)
			{

				for (i = 0; i < northResultSize; i++)
				{
					if (pl->thrustHit == 1 && (northList[i]->type == ENEMY_CRAWLER || northList[i]->type == ENEMY_SHOOTER || northList[i]->type == ENEMY_BOXERGREG))
					{
						northList[i]->enemy.knockBackDirection = pl->direction;
						northList[i]->enemy.offsetX = 8;
						northList[i]->enemy.offsetY = 9;
						northList[i]->enemy.health -= 1;
						northList[i]->enemy.timer = currTime;

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (northList[i]->type == ICEBLOCK))
					{
						northList[i]->iBlock.moving = 1;
						northList[i]->iBlock.direction = pl->direction;
						northList[i]->iBlock.health -= 1;
						northList[i]->iBlock.startTime = currTime;
						
						pl->thrustHit = 0;
					}
				}
			}
			break;
			case 1:
			if (eastResultSize > 0)
			{
				for (i = 0; i < eastResultSize; i++)
				{
					if (pl->thrustHit == 1 && (eastList[i]->type == ENEMY_CRAWLER || eastList[i]->type == ENEMY_SHOOTER || eastList[i]->type == ENEMY_BOXERGREG))
					{
						eastList[i]->enemy.knockBackDirection = pl->direction;
						eastList[i]->enemy.offsetX = 9;
						eastList[i]->enemy.offsetY = 8;
						eastList[i]->enemy.health -= 1;
						eastList[i]->enemy.timer = currTime;
						
						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (eastList[i]->type == ICEBLOCK))
					{
						eastList[i]->iBlock.moving = 1;
						eastList[i]->iBlock.direction = pl->direction;
						eastList[i]->iBlock.health -= 1;
						eastList[i]->iBlock.startTime = currTime;

						pl->thrustHit = 0;
					}
				}
			}
			break;
			case 2:
			if (southResultSize > 0)
			{
				for (i = 0; i < southResultSize; i++)
				{
					if (pl->thrustHit == 1 && (southList[i]->type == ENEMY_CRAWLER || southList[i]->type == ENEMY_SHOOTER || southList[i]->type == ENEMY_BOXERGREG))
					{
						southList[i]->enemy.knockBackDirection = pl->direction;
						southList[i]->enemy.offsetX = 8;
						southList[i]->enemy.offsetY = 7;
						southList[i]->enemy.health -= 1;
						southList[i]->enemy.timer = currTime;

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (southList[i]->type == ICEBLOCK))
					{
						southList[i]->iBlock.moving = 1;
						southList[i]->iBlock.direction = pl->direction;
						southList[i]->iBlock.health -= 1;
						southList[i]->iBlock.startTime = currTime;
						
						pl->thrustHit = 0;
					}
				}
			}
			break;
			case 3:
			if (westResultSize > 0)
			{
				for (i = 0; i < westResultSize; i++)
				{
					if (pl->thrustHit == 1 && (westList[i]->type == ENEMY_CRAWLER || westList[i]->type == ENEMY_SHOOTER || westList[i]->type == ENEMY_BOXERGREG))
					{
						westList[i]->enemy.knockBackDirection = pl->direction;
						westList[i]->enemy.offsetX = 7;
						westList[i]->enemy.offsetY = 8;
						westList[i]->enemy.health -= 1;
						westList[i]->enemy.timer = currTime;

						pl->thrustHit = 0;
					}

					if (pl->thrustHit == 1 && (westList[i]->type == ICEBLOCK))
					{
						westList[i]->iBlock.moving = 1;
						westList[i]->iBlock.direction = pl->direction;
						westList[i]->iBlock.health -= 1;
						westList[i]->iBlock.startTime = currTime;
						
						pl->thrustHit = 0;
					}
				}
			}
			break;
			default:
			fprintf(stderr, "Bad direction for thrusting player: %d\n", pl->direction);
			break;
		}

		return; //don't move and thrust sword at the same time
	}

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
						if (northList[i]->type == GAMEBLOCK && northList[i]->gBlock.height == 0)
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
						if (eastList[i]->type == GAMEBLOCK && eastList[i]->gBlock.height == 0)
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
						if (southList[i]->type == GAMEBLOCK && southList[i]->gBlock.height == 0)
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
						if (westList[i]->type == GAMEBLOCK && westList[i]->gBlock.height == 0)
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
				if (northResultFullSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y - 1;
					pl->holding = NULL;
				}
				break;
				case 1:
				if (eastResultFullSize == 0)
				{
					(pl->holding)->y = pl->y;
					(pl->holding)->x = pl->x + 1;
					pl->holding = NULL;
				}
				break;
				case 2:
				if (southResultFullSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y + 1;
					pl->holding = NULL;
				}
				break;
				case 3:
				if (westResultFullSize == 0)
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
			
			return; //prevents player from walking onto their own placed block
		}
	}
	else if (!getKey(P1_A) && pl->aKeyDown)
	{
		pl->aKeyDown = 0;
	}

	if (getKey(P1_RIGHT))
	{
		if (pl->direction == 0 || pl->direction == 2)
		{
			if (pl->offsetY % 4 == 1)
			{
				pl->offsetY--;
			}
			else if (pl->offsetY % 4 == 3)
			{
				pl->offsetY++;
			}
		}
		pl->offsetX += PLAYER_WALK_SPEED;
		pl->direction = 1;
		
		if (getKey(P1_UP))
		{
			pl->offsetX -= PLAYER_WALK_SPEED/2;
			pl->offsetY += PLAYER_WALK_SPEED/2;
		}
		else if (getKey(P1_DOWN))
		{
			pl->offsetX -= PLAYER_WALK_SPEED/2;
			pl->offsetY -= PLAYER_WALK_SPEED/2;
		}

		if (pl->offsetX > 16)
		{
			if (eastResultSize < 1)
			{
				xInc++;
				pl->offsetX -= 16;
			}
			else
			{
				pl->offsetX = 16;
			}
		}
	}
	
	if (getKey(P1_DOWN))
	{
		if (pl->direction == 1 || pl->direction == 3)
		{
			if (pl->offsetX % 4 == 1)
			{
				pl->offsetX--;
			}
			else if (pl->offsetX % 4 == 3)
			{
				pl->offsetX++;
			}
		}
		pl->offsetY += PLAYER_WALK_SPEED;
		pl->direction = 2;
		
		if (pl->offsetY > 16)
		{
			if (southResultSize < 1)
			{
				yInc++;
				pl->offsetY -= 16;
			}
			else
			{
				pl->offsetY = 16;
			}
		}
	}
	
	if (getKey(P1_LEFT))
	{
		if (pl->direction == 0 || pl->direction == 2)
		{
			if (pl->offsetY % 4 == 1)
			{
				pl->offsetY--;
			}
			else if (pl->offsetY % 4 == 3)
			{
				pl->offsetY++;
			}
		}
		pl->offsetX -= PLAYER_WALK_SPEED;
		pl->direction = 3;

		if (getKey(P1_UP))
		{
			pl->offsetX += PLAYER_WALK_SPEED/2;
			pl->offsetY += PLAYER_WALK_SPEED/2;
		}
		else if (getKey(P1_DOWN))
		{
			pl->offsetX += PLAYER_WALK_SPEED/2;
			pl->offsetY -= PLAYER_WALK_SPEED/2;
		}
		
		if (pl->offsetX < 0)
		{
			if (westResultSize < 1)
			{
				xInc--;
				pl->offsetX += 16;
			}
			else
			{
				pl->offsetX = 0;
			}
		}

	}
	
	if (getKey(P1_UP))
	{
		if (pl->direction == 1 || pl->direction == 3)
		{
			if (pl->offsetX % 4 == 1)
			{
				pl->offsetX--;
			}
			else if (pl->offsetX % 4 == 3)
			{
				pl->offsetX++;
			}
		}
		pl->offsetY -= PLAYER_WALK_SPEED;
		pl->direction = 0;
		
		if (pl->offsetY < 0)
		{
			if ( northResultSize < 1)
			{
				yInc--;
				pl->offsetY += 16;
			}
			else
			{
				pl->offsetY = 0;
			}
		}
	}

	pl->x += xInc;
	pl->y += yInc;

	return;
}

void update_player2(Player* pl, Uint32 currTime)
{
	int i,j;
	int xInc = 0;
	int yInc = 0;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currResultSize;
	
	Entity* northListFull[5];
	Entity* southListFull[5];
	Entity* eastListFull[5];
	Entity* westListFull[5];
	int northResultFullSize;
	int southResultFullSize;
	int eastResultFullSize;
	int westResultFullSize;
	
	occupyingOnHere(pl->x, pl->y - 1, northListFull, 5, &northResultFullSize);
	occupyingOnHere(pl->x, pl->y + 1, southListFull, 5, &southResultFullSize);
	occupyingOnHere(pl->x + 1, pl->y, eastListFull, 5, &eastResultFullSize);
	occupyingOnHere(pl->x - 1, pl->y, westListFull, 5, &westResultFullSize);

	filterOccupyWallsForPlayer(pl->x, pl->y - 1, northList, 5, &northResultSize);
	filterOccupyWallsForPlayer(pl->x, pl->y + 1, southList, 5, &southResultSize);
	filterOccupyWallsForPlayer(pl->x + 1, pl->y, eastList, 5, &eastResultSize);
	filterOccupyWallsForPlayer(pl->x - 1, pl->y, westList, 5, &westResultSize);
	occupyingOnHere(pl->x, pl->y, currList, 5, &currResultSize);
	
	//quick and hasty animation
	if (currTime - pl->lastFrameUpdate > 250)
	{
		if (pl->frame == 0)
		{
			pl->frame = 1;
		}
		else
		{
			pl->frame = 0;
		}
		
		pl->lastFrameUpdate = currTime;
	}
	
	if (pl->dead == 1)
	{
		return;
	}
	
	if (getPlayerHealth(1) < 1)
	{
		pl->dead = 1;
	}
	
	//this is a quick way of preventing block timeout
	if (pl->holding != NULL)
	{
		pl->holding->startTime = getTimeSingleton();
	}
	
	// if the player gets completely knocked back, then he/she cannot do logic for that iteration
	if (pl->knockBackDirection != 255)
	{
		switch(pl->knockBackDirection)
		{
			case 0:
			if (northResultSize == 0)
			{
				pl->y -= 1;
			}
			break;
			case 1:
			if (eastResultSize == 0 && pl->x + 1 < BOARD_WIDTH)
			{
				pl->x += 1;
			}
			break;
			case 2:
			if (southResultSize == 0)
			{
				pl->y += 1;
			}
			break;
			case 3:
			if (westResultSize == 0 && pl->x - 1 >= 0)
			{
				pl->x -= 1;
			}
			break;
			default:
			printf("Player knock back all odd! value:%d\n", pl->knockBackDirection);
			break;
		}

		pl->knockBackDirection = 255;
		return;
	}
	
	for (i = 0; i < currResultSize; i++)
	{
		if (currList[i]->type == SUPERHAMMER && pl->holdingSuperHammer == 0)
		{
			pl->holdingSuperHammer = 1;
			currList[i]->type = DELETE_ME_PLEASE;
			break;
		}
	}
	
	if (getKey(P2_B) && !(pl->bKeyDown))
	{
		pl->bKeyDown = 1;
		pl->swordTimer = currTime;
		pl->isThrusting = 1;
	}
	else if (!getKey(P2_B) && pl->bKeyDown)
	{
		pl->bKeyDown = 0;
	}

	if (pl->isThrusting)
	{
		if (currTime - pl->swordTimer > 150)
		{
			pl->isThrusting = 0;
			
			if (pl->holdingSuperHammer == 1)
			{
				pl->holdingSuperHammer = 0;
				for (i = 0; i < 3; i++)
				{
					for (j = 0; j < 3; j++)
					{
						pushEntity(EXPLOSION, pl->x - 1 + i, pl->y - 1 + j);
					}
				}
			}
		}

		switch (pl->direction)
		{
			case 0:
			if (northResultSize > 0)
			{

				for (i = 0; i < northResultSize; i++)
				{
					if (northList[i]->type == ENEMY_CRAWLER || northList[i]->type == ENEMY_SHOOTER || northList[i]->type == ENEMY_BOXERGREG)
					{
						northList[i]->enemy.knockBackDirection = pl->direction;
						northList[i]->enemy.offsetX = 8;
						northList[i]->enemy.offsetY = 9;
						northList[i]->enemy.health -= 1;
						northList[i]->enemy.timer = currTime;
						
						pl->isThrusting = 0;
					}
					
					if (northList[i]->type == ICEBLOCK)
					{
						northList[i]->iBlock.moving = 1;
						northList[i]->iBlock.direction = pl->direction;
						northList[i]->iBlock.health -= 1;
						
						pl->isThrusting = 0;
					}
				}
			}
			break;
			case 1:
			if (eastResultSize > 0)
			{
				for (i = 0; i < eastResultSize; i++)
				{
					if (eastList[i]->type == ENEMY_CRAWLER || eastList[i]->type == ENEMY_SHOOTER || eastList[i]->type == ENEMY_BOXERGREG)
					{
						eastList[i]->enemy.knockBackDirection = pl->direction;
						eastList[i]->enemy.offsetX = 9;
						eastList[i]->enemy.offsetY = 8;
						eastList[i]->enemy.health -= 1;
						eastList[i]->enemy.timer = currTime;
						
						pl->isThrusting = 0;
					}
					
					if (eastList[i]->type == ICEBLOCK)
					{
						eastList[i]->iBlock.moving = 1;
						eastList[i]->iBlock.direction = pl->direction;
						eastList[i]->iBlock.health -= 1;

						pl->isThrusting = 0;
					}
				}
			}
			break;
			case 2:
			if (southResultSize > 0)
			{
				for (i = 0; i < southResultSize; i++)
				{
					if (southList[i]->type == ENEMY_CRAWLER || southList[i]->type == ENEMY_SHOOTER || southList[i]->type == ENEMY_BOXERGREG)
					{
						southList[i]->enemy.knockBackDirection = pl->direction;
						southList[i]->enemy.offsetX = 8;
						southList[i]->enemy.offsetY = 7;
						southList[i]->enemy.health -= 1;
						southList[i]->enemy.timer = currTime;

						pl->isThrusting = 0;
					}
					
					if (southList[i]->type == ICEBLOCK)
					{
						southList[i]->iBlock.moving = 1;
						southList[i]->iBlock.direction = pl->direction;
						southList[i]->iBlock.health -= 1;
						
						pl->isThrusting = 0;
					}
				}
			}
			break;
			case 3:
			if (westResultSize > 0)
			{
				for (i = 0; i < westResultSize; i++)
				{
					if (westList[i]->type == ENEMY_CRAWLER || westList[i]->type == ENEMY_SHOOTER || westList[i]->type == ENEMY_BOXERGREG)
					{
						westList[i]->enemy.knockBackDirection = pl->direction;
						westList[i]->enemy.offsetX = 7;
						westList[i]->enemy.offsetY = 8;
						westList[i]->enemy.health -= 1;
						westList[i]->enemy.timer = currTime;

						pl->isThrusting = 0;
					}

					if (westList[i]->type == ICEBLOCK)
					{
						westList[i]->iBlock.moving = 1;
						westList[i]->iBlock.direction = pl->direction;
						westList[i]->iBlock.health -= 1;
						
						pl->isThrusting = 0;
					}
				}
			}
			break;
			default:
			fprintf(stderr, "Bad direction for thrusting player: %d\n", pl->direction);
			break;
		}

		return; //don't move and thrust sword at the same time
	}

	if (getKey(P2_A) && !(pl->aKeyDown))
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
						if (northList[i]->type == GAMEBLOCK && northList[i]->gBlock.height == 0)
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
						if (eastList[i]->type == GAMEBLOCK && eastList[i]->gBlock.height == 0)
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
						if (southList[i]->type == GAMEBLOCK && southList[i]->gBlock.height == 0)
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
						if (westList[i]->type == GAMEBLOCK && westList[i]->gBlock.height == 0)
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
				if (northResultFullSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y - 1;
					pl->holding = NULL;
				}
				break;
				case 1:
				if (eastResultFullSize == 0)
				{
					(pl->holding)->y = pl->y;
					(pl->holding)->x = pl->x + 1;
					pl->holding = NULL;
				}
				break;
				case 2:
				if (southResultFullSize == 0)
				{
					(pl->holding)->x = pl->x;
					(pl->holding)->y = pl->y + 1;
					pl->holding = NULL;
				}
				break;
				case 3:
				if (westResultFullSize == 0)
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
			
			return; //prevents player from walking onto their own placed block
		}
	}
	else if (!getKey(P2_A) && pl->aKeyDown)
	{
		pl->aKeyDown = 0;
	}

	if (getKey(P2_RIGHT))
	{
		if (pl->direction == 0 || pl->direction == 2)
		{
			if (pl->offsetY % 4 == 1)
			{
				pl->offsetY--;
			}
			else if (pl->offsetY % 4 == 3)
			{
				pl->offsetY++;
			}
		}
		pl->offsetX += PLAYER_WALK_SPEED;
		pl->direction = 1;
		
		if (getKey(P2_UP))
		{
			pl->offsetX -= PLAYER_WALK_SPEED/2;
			pl->offsetY += PLAYER_WALK_SPEED/2;
		}
		else if (getKey(P2_DOWN))
		{
			pl->offsetX -= PLAYER_WALK_SPEED/2;
			pl->offsetY -= PLAYER_WALK_SPEED/2;
		}

		if (pl->offsetX > 16)
		{
			if (eastResultSize < 1)
			{
				xInc++;
				pl->offsetX -= 16;
			}
			else
			{
				pl->offsetX = 16;
			}
		}
	}
	
	if (getKey(P2_DOWN))
	{
		if (pl->direction == 1 || pl->direction == 3)
		{
			if (pl->offsetX % 4 == 1)
			{
				pl->offsetX--;
			}
			else if (pl->offsetX % 4 == 3)
			{
				pl->offsetX++;
			}
		}
		pl->offsetY += PLAYER_WALK_SPEED;
		pl->direction = 2;
		
		if (pl->offsetY > 16)
		{
			if (southResultSize < 1)
			{
				yInc++;
				pl->offsetY -= 16;
			}
			else
			{
				pl->offsetY = 16;
			}
		}
	}
	
	if (getKey(P2_LEFT))
	{
		if (pl->direction == 0 || pl->direction == 2)
		{
			if (pl->offsetY % 4 == 1)
			{
				pl->offsetY--;
			}
			else if (pl->offsetY % 4 == 3)
			{
				pl->offsetY++;
			}
		}
		pl->offsetX -= PLAYER_WALK_SPEED;
		pl->direction = 3;

		if (getKey(P2_UP))
		{
			pl->offsetX += PLAYER_WALK_SPEED/2;
			pl->offsetY += PLAYER_WALK_SPEED/2;
		}
		else if (getKey(P2_DOWN))
		{
			pl->offsetX += PLAYER_WALK_SPEED/2;
			pl->offsetY -= PLAYER_WALK_SPEED/2;
		}
		
		if (pl->offsetX < 0)
		{
			if (westResultSize < 1)
			{
				xInc--;
				pl->offsetX += 16;
			}
			else
			{
				pl->offsetX = 0;
			}
		}

	}
	
	if (getKey(P2_UP))
	{
		if (pl->direction == 1 || pl->direction == 3)
		{
			if (pl->offsetX % 4 == 1)
			{
				pl->offsetX--;
			}
			else if (pl->offsetX % 4 == 3)
			{
				pl->offsetX++;
			}
		}
		pl->offsetY -= PLAYER_WALK_SPEED;
		pl->direction = 0;
		
		if (pl->offsetY < 0)
		{
			if ( northResultSize < 1)
			{
				yInc--;
				pl->offsetY += 16;
			}
			else
			{
				pl->offsetY = 0;
			}
		}
	}

	pl->x += xInc;
	pl->y += yInc;

	return;
}

void update_enemy(Enemy* enemy, Uint32 currTime)
{
	int i;
	int moveSpeed = 1;
	Uint32 delta = currTime - enemy->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currListSize;

	filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
	filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
	filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
	filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	occupyingOnHere(enemy->x, enemy->y, currList, 5, &currListSize);
	
	if (currTime - enemy->lastFrameUpdate > 150)
	{
		if (enemy->frame == 0)
		{
			enemy->frame = 1;
		}
		else
		{
			enemy->frame = 0;
		}
		
		enemy->lastFrameUpdate = currTime;
	}
	
	if(enemy->health < 1)
	{
		enemy->type = DELETE_ME_PLEASE;
		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;
		}
		
		addScore(50);

		return;
	}
	
	if (enemy->x < 0)
	{
		if (enemy->cream != NULL)
		{
			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

			enemy->type = DELETE_ME_PLEASE;
			return;
		}
	}
	else if (enemy->x > BOARD_WIDTH - 1)
	{
		if (enemy->cream != NULL)
		{
			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

			enemy->type = DELETE_ME_PLEASE;
			return;
		}
	}

	for (i = 0; i < currListSize; i++)
	{
		if (currList[i]->type == ICECREAM && enemy->cream == NULL)
		{
			enemy->cream = (IceCream*)currList[i];
			enemy->cream->x = -5;
			enemy->cream->y = -5;
		}
		
		if (currList[i]->type == GLUE)
		{
			if (enemy->knockBackDirection != 255)
			{
				enemy->knockBackDirection = 255;
			}
				
			if  (xrand() % 2 == 0)
			{
				moveSpeed = 0;
			}
		}
	}

	if (delta / 32 > 0)
	{
		if (enemy->knockBackDirection == 255)
		{
			switch (enemy->direction)
			{
				case 0:
				enemy->offsetY -= moveSpeed;
				break;
				case 1:
				enemy->offsetX += moveSpeed;
				break;
				case 2:
				enemy->offsetY += moveSpeed;
				break;
				case 3:
				enemy->offsetX -= moveSpeed;
				break;
				default:
				break;
			}
		}
		else
		{
			switch (enemy->knockBackDirection)
			{
				case 0:
				enemy->offsetY -= 4;
				break;
				case 1:
				enemy->offsetX += 4;
				break;
				case 2:
				enemy->offsetY += 4;
				break;
				case 3:
				enemy->offsetX -= 4;
				break;
				default:
				break;
			}
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
			
			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
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
			
			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
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
			
			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
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
			
			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
			}
		}
		
		enemy->lastMovementUpdate = currTime;
	}
}

void update_shooter(Enemy* enemy, Uint32 currTime)
{
	int newDir(Enemy* en)
	{
		if (en->x < 0)
		{
			if (en->cream != NULL)
			{
				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;
				
				en->type = DELETE_ME_PLEASE;
				return 3;
			}
			else
			{
				return 1;
			}
		}
		else if (en->x >= BOARD_WIDTH)
		{
			if (en->cream != NULL)
			{
				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;
				
				en->type = DELETE_ME_PLEASE;

				return 1;
			}
			else
			{
				return 3;
			}
		}
		
		if (en->y == BOARD_TOP_WALL + 1)
		{
			if (xrand() % 3 == 1)
			{
				return 2;
			}
		}
		if (en->y == BOARD_BOTTOM_WALL - 1)
		{
			if (xrand() % 3 == 1)
			{
				return 0;
			}
		}
		
		if (rand() % 3 == 0)
		{
			return 4;
		}
		
		if ( (en->x <= 8 && en->cream == NULL) || (en->x > 8 && en->cream != NULL) )
		{
			int randNumber = xrand() % 10;

			if (randNumber < 2)
			{
				return 3;
			}
			else if (randNumber < 4)
			{
				return 0;
			}
			else if (randNumber < 6)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}
		else if ( (en->x <= 8 && en->cream != NULL) || (en->x > 8 && en->cream == NULL) )
		{
			int randNumber = xrand() % 10;

			if (randNumber < 2)
			{
				return 1;
			}
			else if (randNumber < 4)
			{
				return 0;
			}
			else if (randNumber < 6)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
		else
		{
			return 4;
		}
	}

	int i;
	int moveSpeed = 4;
	Uint32 delta = currTime - enemy->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currListSize;

	filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
	filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
	filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
	filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	occupyingOnHere(enemy->x, enemy->y, currList, 5, &currListSize);
	
	if(enemy->health < 1)
	{
		enemy->type = DELETE_ME_PLEASE;
		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;

			addScore(50);
		}
		return;
	}

	for (i = 0; i < currListSize; i++)
	{
		if (currList[i]->type == ICECREAM && enemy->cream == NULL)
		{
			enemy->cream = (IceCream*)currList[i];
			enemy->cream->x = -5;
			enemy->cream->y = -5;
		}
		
		if (currList[i]->type == GLUE)
		{
			if (xrand() % 4 == 0)
			{
				moveSpeed = 1;
			}
			else
			{
				moveSpeed = 0;
			}
			
			if (enemy->knockBackDirection != 255)
			{
				enemy->knockBackDirection = 255;
			}
		}
	}

	if (delta / 32 > 0)
	{
		if (enemy->x < 0)
		{
			//
		}	
		if ((enemy->direction == 0 || enemy->direction == 2) && enemy->offsetY == 8 && enemy->direction < 4)
		{
			if (rand() % 4 <= 1)
			{
				enemy->direction = newDir(enemy) % 5;
			}
		}
		else if ((enemy->direction == 1 || enemy->direction == 3) && enemy->offsetX == 8 && enemy->direction < 4)
		{
			if (rand() % 4 <= 1)
			{
				enemy->direction = newDir(enemy) % 5;
			}
		}

		if (enemy->knockBackDirection == 255)
		{
			switch (enemy->direction)
			{
				case 0:
				enemy->offsetY -= moveSpeed;
				break;
				case 1:
				enemy->offsetX += moveSpeed;
				break;
				case 2:
				enemy->offsetY += moveSpeed;
				break;
				case 3:
				enemy->offsetX -= moveSpeed;
				break;
				case 4:
				{
					if (abs(enemy->offsetX) - 8 < 4)
					{
						enemy->offsetX = 8;
					}
					else if (enemy->offsetX > 8)
					{
						enemy->offsetX -= 4;
					}
					else if (enemy->offsetX < 8)
					{
						enemy->offsetX += 4;
					}

					if (abs(enemy->offsetY) - 8 < 4)
					{
						enemy->offsetY = 8;
					}
					else if (enemy->offsetY > 8)
					{
						enemy->offsetY -= 4;
					}
					else if (enemy->offsetY < 8)
					{
						enemy->offsetY += 4;
					}
				}
				default:
				break;
			}
		}
		else
		{
			switch (enemy->knockBackDirection)
			{
				case 0:
				enemy->offsetY -= 8;
				break;
				case 1:
				enemy->offsetX += 8;
				break;
				case 2:
				enemy->offsetY += 8;
				break;
				case 3:
				enemy->offsetX -= 8;
				break;
				default:
				break;
			}
		}

		if (enemy->offsetX < 0)
		{
			if (westResultSize == 0)
			{
				enemy->offsetX = 16;
				enemy->x -= 1;
			}
			else
			{
				enemy->offsetX = 8;
				enemy->direction = newDir(enemy) % 5;
			}

			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
			}
		}
		else if (enemy->offsetX > 16)
		{
			if (eastResultSize == 0)
			{
				enemy->offsetX = 0;
				enemy->x += 1;
			}
			else
			{
				enemy->offsetX = 8;
				enemy->direction = newDir(enemy) % 5;
			}

			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
			}
		}

		if (enemy->offsetY < 0)
		{
			if (northResultSize == 0)
			{
				enemy->offsetY = 16;
				enemy->y -= 1;
			}
			else
			{
				enemy->offsetY = 8;
				enemy->direction = newDir(enemy) % 5;
			}

			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
			}
		}
		else if (enemy->offsetY > 16)
		{
			if (southResultSize == 0)
			{
				enemy->offsetY = 0;
				enemy->y += 1;
			}
			else
			{
				enemy->offsetY = 8;
				enemy->direction = newDir(enemy) % 5;
			}

			if (enemy->knockBackDirection < 255)
			{
				enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;
			}
		}
		
		if (enemy->direction == 4 && enemy->knockBackDirection == 255)
		{
			if (rand() % 10 == 0)
			{
				enemy->direction = newDir(enemy);
				
				Entity* newLaser = pushEntity(LASER, enemy->x, enemy->y);
				
				newLaser->laser.direction = xrand() % 4;
				
				switch (newLaser->laser.direction)
				{
					case 0:
					newLaser->laser.y -= 1;
					newLaser->laser.offsetY = 16;
					break;
					case 1:
					newLaser->laser.x += 1;
					newLaser->laser.offsetX = 0;
					break;
					case 2:
					newLaser->laser.y += 1;
					newLaser->laser.offsetY = 0;
					break;
					case 3:
					newLaser->laser.x -= 1;
					newLaser->laser.offsetX = 16;
					break;
					default:
					break;
				}
			}
		}
		
		enemy->lastMovementUpdate = currTime;
	}
}

void update_boxergreg(Enemy* enemy, Uint32 currTime)
{
	int newDir(Enemy* en)
	{
		if (en->x < 0)
		{
			if (en->cream != NULL)
			{
				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;
				
				en->type = DELETE_ME_PLEASE;
				return 3;
			}
			return 1;
		}
		else if (en->x >= BOARD_WIDTH)
		{
			if (en->cream != NULL)
			{
				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;
				
				en->type = DELETE_ME_PLEASE;
				return 1;
			}
			return 3;
		}
		
		if (en->y == BOARD_TOP_WALL + 1)
		{
			if (xrand() % 3 == 1)
			{
				return 2;
			}
		}
		if (en->y == BOARD_BOTTOM_WALL - 1)
		{
			if (xrand() % 3 == 1)
			{
				return 0;
			}
		}
		
		if (rand() % 4 < 3)
		{
			if (en->direction != 4)
			{
				en->AISlot2 = en->direction;
			}
			return 4;
		}
		
		if ( (en->x <= 8 && en->cream == NULL) || (en->x > 8 && en->cream != NULL) )
		{
			int randNumber = xrand() % 10;

			if (randNumber < 2)
			{
				return 3;
			}
			else if (randNumber < 4)
			{
				return 0;
			}
			else if (randNumber < 6)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}
		else if ( (en->x <= 8 && en->cream != NULL) || (en->x > 8 && en->cream == NULL) )
		{
			int randNumber = xrand() % 10;

			if (randNumber < 2)
			{
				return 1;
			}
			else if (randNumber < 4)
			{
				return 0;
			}
			else if (randNumber < 6)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
		else
		{
			return 4;
		}
	}

	int i;
	int moveSpeed = 1;
	Uint32 delta = currTime - enemy->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currListSize;

	filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
	filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
	filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
	filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	occupyingOnHere(enemy->x, enemy->y, currList, 5, &currListSize);
	
	if (currTime - enemy->lastFrameUpdate > 150)
	{
		if (enemy->frame == 0)
		{
			enemy->frame = 1;
		}
		else
		{
			enemy->frame = 0;
		}
		
		enemy->lastFrameUpdate = currTime;
	}
	

	if(enemy->health < 1)
	{
		enemy->type = DELETE_ME_PLEASE;
		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;
		}
		
		addScore(50);

		return;
	}
	
	if (enemy->x < 0)
	{
		if (enemy->cream != NULL)
		{
			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

			enemy->type = DELETE_ME_PLEASE;
			return;
		}
	}
	else if (enemy->x >= BOARD_WIDTH)
	{
		if (enemy->cream != NULL)
		{
			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

			enemy->type = DELETE_ME_PLEASE;
			return;
		}
	}

	for (i = 0; i < currListSize; i++)
	{
		if (currList[i]->type == ICECREAM && enemy->cream == NULL  && enemy->AISlot1 == 0)
		{
			enemy->cream = (IceCream*)currList[i];
			enemy->cream->x = -5;
			enemy->cream->y = -5;
		}
		
		if (currList[i]->type == GLUE)
		{
			if (enemy->knockBackDirection != 255)
			{
				enemy->knockBackDirection = 255;
			}
				
			if (xrand() % 2 == 1)
			{
				moveSpeed = 0;
			}
		}
	}

	if (delta / 32 > 0)
	{
		if (enemy->offsetX == 8 && enemy->offsetY == 8 && enemy->AISlot1 == 0 && enemy->knockBackDirection == 255)
		{
			enemy->direction = newDir(enemy);
		}

		
			if (enemy->knockBackDirection < 255 && xor((enemy->offsetY == 8) && (enemy->knockBackDirection == 0 || enemy->knockBackDirection == 2), (enemy->offsetX == 8) && (enemy->knockBackDirection == 1 || enemy->knockBackDirection == 3)))
			{
				/*enemy->direction = enemy->knockBackDirection;
				enemy->knockBackDirection = 255;   */
				enemy->direction = 4;
				switch (enemy->knockBackDirection)
				{
					case 0:
					enemy->AISlot2 = 2;
					break;
					case 1:
					enemy->AISlot2 = 3;
					break;
					case 2:
					enemy->AISlot2 = 0;
					break;
					case 3:
					enemy->AISlot2 = 1;
					break;
					default:
					enemy->AISlot2 = enemy->knockBackDirection;
					break;
				}
	
				enemy->knockBackDirection = 255;
			}


		if (enemy->knockBackDirection == 255)
		{
			switch (enemy->direction)
			{
				case 0:
				enemy->offsetY -= moveSpeed;
				break;
				case 1:
				enemy->offsetX += moveSpeed;
				break;
				case 2:
				enemy->offsetY += moveSpeed;
				break;
				case 3:
				enemy->offsetX -= moveSpeed;
				break;
				default:
				break;
			}
		}
		else
		{
			switch (enemy->knockBackDirection)
			{
				case 0:
				enemy->offsetY -= 8;
				break;
				case 1:
				enemy->offsetX += 8;
				break;
				case 2:
				enemy->offsetY += 8;
				break;
				case 3:
				enemy->offsetX -= 8;
				break;
				default:
				break;
			}
		}

		if (enemy->offsetX < 0)
		{
			if (westResultSize == 0)
			{
				enemy->offsetX = 16;
				enemy->x -= 1;
			}
			else
			{
				enemy->offsetX = 8;
				enemy->direction = newDir(enemy);
			}
		}
		else if (enemy->offsetX > 16)
		{
			if (eastResultSize == 0)
			{
				enemy->offsetX = 0;
				enemy->x += 1;
			}
			else
			{
				enemy->offsetX = 8;
				enemy->direction = newDir(enemy);
			}
		}

		if (enemy->offsetY < 0)
		{
			if (northResultSize == 0)
			{
				enemy->offsetY = 16;
				enemy->y -= 1;
			}
			else
			{
				enemy->offsetY = 8;
				enemy->direction = newDir(enemy);
			}
		}
		else if (enemy->offsetY > 16)
		{
			if (southResultSize == 0)
			{
				enemy->offsetY = 0;
				enemy->y += 1;
			}
			else
			{
				enemy->offsetY = 8;
				enemy->direction = newDir(enemy);
			}
		}
		
		if (enemy->direction == 4 && enemy->knockBackDirection == 255)
		{
			if (enemy->AISlot1 == 1)
			{
				switch(enemy->AISlot2)
				{
					case 0:
					for (i = 0; i < northResultSize; i++)
					{
						if (northList[i]->type == PLAYER1)
						{
							if (northList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(1, -1);
								northList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
						if (northList[i]->type == PLAYER2)
						{
							if (northList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								northList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
					}
					break;
					case 1:
					for (i = 0; i < eastResultSize; i++)
					{
						if (eastList[i]->type == PLAYER1)
						{
							if (eastList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(1, -1);
								eastList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
						if (eastList[i]->type == PLAYER2)
						{
							if (eastList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								eastList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
					}
					break;
					case 2:
					for (i = 0; i < southResultSize; i++)
					{
						if (southList[i]->type == PLAYER1)
						{
							if (southList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(1, -1);
								southList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
						if (southList[i]->type == PLAYER2)
						{
							if (southList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								southList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
					}
					break;
					case 3:
					for (i = 0; i < westResultSize; i++)
					{
						if (westList[i]->type == PLAYER1)
						{
							if (westList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(1, -1);
								westList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
						if (westList[i]->type == PLAYER2)
						{
							if (westList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								westList[i]->player.knockBackDirection = enemy->AISlot2;
							}
						}
					}
					break;
					default:
					printf("Boxer Greg tried to punch in a bad direction!\n");
					break;
				}
				
				if (currTime - enemy->timer > 1000)
				{
					enemy->AISlot1 = 0;
				}
				
				if (xrand() % 5 < 1)
				{
					enemy->AISlot2 = xrand() % 4;
				}
			}
			else if (enemy->cream == NULL && enemy->AISlot1 == 0 && xrand() % 10 < 3)
			{
				enemy->AISlot1 = 1;
				enemy->timer = currTime;
			}
			else if (enemy->AISlot1 == 0 && xrand() % 20 == 0)
			{
				enemy->direction = newDir(enemy) % 4;
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
	
	if (block->health < 1)
	{
		block->type = DELETE_ME_PLEASE;
		return;
	}
	
	if (currTime - block->startTime > 10 * 1000)
	{
		block->health -= 1;
		block->startTime = currTime;
	}

	filterOccupyWalls(block->x, block->y - 1, northList, 5, &northResultSize);
	filterOccupyWalls(block->x, block->y + 1, southList, 5, &southResultSize);
	filterOccupyWalls(block->x + 1, block->y, eastList, 5, &eastResultSize);
	filterOccupyWalls(block->x - 1, block->y, westList, 5, &westResultSize);

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
				block->offsetX = 16;
				block->x -= 1;
			}
			else
			{
				block->offsetX = 8;
				block->moving = 0;
			}
		}
		else if (block->offsetX > 16)
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
				block->offsetY = 16;
				block->y -= 1;
			}
			else
			{
				block->offsetY = 8;
				block->moving = 0;
			}
		}
		else if (block->offsetY > 16)
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

void update_laser(Laser* block, Uint32 currTime)
{
	int i;
	Uint32 delta = currTime - block->lastMovementUpdate;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	Entity* currList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	int currResultSize;

	filterOccupyWalls(block->x, block->y - 1, northList, 5, &northResultSize);
	filterOccupyWalls(block->x, block->y + 1, southList, 5, &southResultSize);
	filterOccupyWalls(block->x + 1, block->y, eastList, 5, &eastResultSize);
	filterOccupyWalls(block->x - 1, block->y, westList, 5, &westResultSize);
	occupyingOnHere(block->x, block->y, currList, 5, &currResultSize);

	for (i = 0; i < currResultSize; i++)
	{
		if (((currList[i]->type == PLAYER1) || (currList[i]->type == PLAYER2)) && block->allegiance == 0)
		{
			if (currList[i]->type == PLAYER1)
			{
				modPlayerHealth(1, -1);
			}

			if (currList[i]->type == PLAYER2)
			{
				modPlayerHealth(2, -1);
			}

			block->type = DELETE_ME_PLEASE;
			return;
		}

		if (currList[i]->type == EXPLOSION && block->allegiance == 0)
		{
			block->type = DELETE_ME_PLEASE;
			return;
		}
		
		if (currList[i]->type == LASER && (currList[i]->laser.direction % 2 == block->direction % 2) && currList[i]->laser.allegiance != block->allegiance)
		{
			currList[i]->type = DELETE_ME_PLEASE;
			block->type = DELETE_ME_PLEASE;
			return;
		}

		if (currList[i]->type == PERMABLOCK || currList[i]->type == GAMEBLOCK || currList[i]->type == ICEBLOCK || currList[i]->type == ICECREAM)
		{
			block->type = DELETE_ME_PLEASE;
			return;
		}
		
		if ((block->allegiance == 1) && (currList[i]->type == ENEMY_CRAWLER || currList[i]->type == ENEMY_SHOOTER || currList[i]->type == ENEMY_SHOOTER))
		{
			currList[i]->enemy.health -= 3;

			block->type = DELETE_ME_PLEASE;
			return;
		}
	}

	if (delta / 32 > 0)
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
			block->offsetX = 15;
			block->x -= 1;
		}
		else if (block->offsetX > 16)
		{
			block->offsetX = 0;
			block->x += 1;
		}
		if (block->offsetY < 0)
		{
			block->offsetY = 15;
			block->y -= 1;
		}
		else if (block->offsetY > 16)
		{
			block->offsetY = 0;
			block->y += 1;
		}
		
		block->lastMovementUpdate = currTime;
	}
}

void update_explosion(Explosion* exp)
{
	int i;
	Uint32 currTime = getTimeSingleton();
	Uint32 delta = currTime - exp->startTime;
	
	Entity* blastList[5];
	int blastResultSize = 0;
	occupyingOnHere(exp->x, exp->y, blastList, 5, &blastResultSize);

	for (i = 0; i < blastResultSize; i++)
	{
		switch (blastList[i]->type)
		{
			case ENEMY_CRAWLER:
			case ENEMY_SHOOTER:
			case ENEMY_BOXERGREG:
			blastList[i]->enemy.health -= 5;
			break;
			default:
			break;
		}
	}

	if (delta > 750)
	{
		exp->type = DELETE_ME_PLEASE;
	}
}

void update_teleblock(TeleBlock* tb)
{
	int i;
	Entity* checkList[5];
	int checkResultSize = 0;
	occupyingOnHere(tb->x, tb->y, checkList, 5, &checkResultSize);
	
	if (getTimeSingleton() - tb->lastClickTime > 100)
	{
		if (tb->frame == 0)
		{
			tb->frame = 1;
		}
		else if (tb->frame == 1)
		{
			tb->frame = 0;
		}
		
		tb->lastClickTime = getTimeSingleton();	
	}

	if (checkResultSize > 0)
	{
		for (i = 0; i < checkResultSize; i++)
		{
			if (checkList[i]->type == TELEBLOCK || checkList[i]->type == DELETE_ME_PLEASE || checkList[i]->type == EXPLOSION)
			{
				continue;
			}
			else
			{
				checkList[i]->base.x = tb->twin->x;
				checkList[i]->base.y = tb->twin->y;
				tb->twin->type = DELETE_ME_PLEASE;
				tb->type = DELETE_ME_PLEASE;
				break;
			}
		}
	}
}

void update_glue(FloorGlue* gl)
{
	/*if (getTimeSingleton() - gl->startTime > 30 * 1000)
	{
		gl->type = DELETE_ME_PLEASE;
	}*/
}

void update_gameBlock(GameBlock* gb, Uint32 currTime)
{
	int i;

	Entity* northList[5];
	Entity* southList[5];
	Entity* eastList[5];
	Entity* westList[5];
	int northResultSize;
	int southResultSize;
	int eastResultSize;
	int westResultSize;
	
	int sameColouredNeighbour = 0;

	filterOccupyType(gb->x, gb->y - 1, northList, 5, &northResultSize, GAMEBLOCK);
	filterOccupyType(gb->x, gb->y + 1, southList, 5, &southResultSize, GAMEBLOCK);
	filterOccupyType(gb->x + 1, gb->y, eastList, 5, &eastResultSize, GAMEBLOCK);
	filterOccupyType(gb->x - 1, gb->y, westList, 5, &westResultSize, GAMEBLOCK);
	
	for (i = 0; i < northResultSize; i++)
	{
		if (northList[i]->gBlock.bType == gb->bType)
		{
			sameColouredNeighbour++;
		}
	}
	for (i = 0; i < eastResultSize; i++)
	{
		if (eastList[i]->gBlock.bType == gb->bType)
		{
			sameColouredNeighbour++;
		}
	}
	for (i = 0; i < southResultSize; i++)
	{
		if (southList[i]->gBlock.bType == gb->bType)
		{
			sameColouredNeighbour++;
		}
	}
	for (i = 0; i < westResultSize; i++)
	{
		if (westList[i]->gBlock.bType == gb->bType)
		{
			sameColouredNeighbour++;
		}
	}

	if (sameColouredNeighbour > 0)
	{
		gb->startTime = currTime;
	}
	else if (currTime - gb->startTime > 30 * 1000)
	{
		gb->type = DELETE_ME_PLEASE;
		return;
	}

	if (gb->lastUpdateTime == 0)
	{
		gb->lastUpdateTime = currTime;
		return;
	}

	if (currTime - gb->lastUpdateTime > 10)
	{
		if (gb->height > 0)
		{
			gb->height -= 2;
		}
		else if (gb->height < 0)
		{
			gb->height = 0;
		}

		gb->lastUpdateTime = currTime;
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
		update_player( (Player*)entity, currTime);
		break;
		case PLAYER2:
		update_player2( (Player*)entity, currTime);
		break;
		case PERMABLOCK:
		break;
		case GAMEBLOCK:
		update_gameBlock( (GameBlock*)entity, currTime);
		break;
		case ICEBLOCK:
		update_iceBlock( (IceBlock*)entity, currTime);
		break;
		case EXPLOSION:
		update_explosion( (Explosion*)entity);
		break;
		case LASER:
		update_laser( (Laser*)entity, currTime);
		break;
		case TELEBLOCK:
		update_teleblock( (TeleBlock*)entity);
		break;
		case ENEMY_CRAWLER:
		update_enemy( (Enemy*)entity, currTime);
		break;
		case ENEMY_SHOOTER:
		update_shooter( (Enemy*)entity, currTime);
		break;
		case ENEMY_BOXERGREG:
		update_boxergreg( (Enemy*)entity, currTime);
		break;
		case SUPERHAMMER:
		break;
		case GLUE:
		update_glue( (FloorGlue*)entity);
		break;
		case ICECREAM:
		break;
		default:
		printf("unregognized entity type updated: %d\n", entity->type);
		break;
	}
	
	return;
}
