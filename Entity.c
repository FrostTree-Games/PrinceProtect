#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Entity.h"
#include "Keyboard.h"
#include "GameLogic.h"
#include "Particle.h"
#include "Pushdown.h"
#include "Audio.h"

#define MAX_GAME_ENTITY_POPULATION 552

#define PLAYER_WALK_SPEED 3
#define HEALTH_UPGRADE_VALUE 3
#define SHURIKEN_BONUS_VALUE 2

// Master list of all entities in play
Entity** entityList = NULL; // set to NULL when uninitalized
const int entityListMaxSize = MAX_GAME_ENTITY_POPULATION;
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
                newEntity->player.direction = 2;
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
                newEntity->player.direction = 2;
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
		newEntity->laser.frame = newEntity->laser.frame % 2;
		newEntity->laser.lastFrameUpdate = getTimeSingleton();
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
		newEntity->iceCream.frame = 0;
		newEntity->iceCream.decoy = 0;
		newEntity->iceCream.lastFrameUpdateTime = getTimeSingleton();
		newEntity->iceCream.decoyTime = getTimeSingleton();
		break;
		case SUPERHAMMER:
		newEntity->hammer.x = newX;
		newEntity->hammer.y = newY;
		break;
		case SHIRUKEN:
		newEntity->shiruken.x = newX;
		newEntity->shiruken.y = newY;
		break;
		case GLUE:
		newEntity->glue.x = newX;
		newEntity->glue.y = newY;
		newEntity->glue.startTime = getTimeSingleton();
		newEntity->glue.population = 1;
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
		case ENEMY_FAST:
		newEntity->enemy.x = newX;
		newEntity->enemy.y = newY;
		newEntity->enemy.direction = rand() % 5;
		newEntity->enemy.offsetX = 8;
		newEntity->enemy.offsetY = 8;
		newEntity->enemy.lastMovementUpdate = 0;
		newEntity->enemy.knockBackDirection = -1;
		newEntity->enemy.cream = NULL;
		newEntity->enemy.health = 2;
		newEntity->enemy.AISlot1 = 0;
		newEntity->enemy.AISlot2 = 0;
		newEntity->enemy.AISlot3 = 1;
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
		newEntity->enemy.health = 3;
		newEntity->enemy.AISlot1 = 0;
		newEntity->enemy.AISlot2 = 0;
		newEntity->enemy.AISlot3 = 0;
		newEntity->enemy.lastFrameUpdate = getTimeSingleton();
		newEntity->enemy.frame = 0;
		break;
		case POOF:
		newEntity->poof.x = newX;
		newEntity->poof.y = newY;
		newEntity->poof.startTime = getTimeSingleton();
		newEntity->poof.offsetX = 8;
		newEntity->poof.offsetY = 8;
		newEntity->poof.colour = 0;
		newEntity->poof.birthed = 0;
		break;
		case HEART:
		newEntity->base.x = newX;
		newEntity->base.y = newY;
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
			entityList[entityListCurrentSize - 1] = NULL;
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
			if ((*returnedSize) < listMaxSize && (en->type == PERMABLOCK || en->type == GAMEBLOCK || en->type == ICEBLOCK || en->type == ENEMY_SHOOTER || en->type == ENEMY_FAST || en->type == PLAYER1 || en->type == PLAYER2))
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

int filterOccupyWallsAndIceCream(int x, int y, Entity** list, int listMaxSize, int* returnedSize)
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
			if ((*returnedSize) < listMaxSize && (en->type == PERMABLOCK || en->type == GAMEBLOCK || en->type == ICEBLOCK || en->type == ENEMY_SHOOTER || en->type == ENEMY_FAST || en->type == PLAYER1 || en->type == PLAYER2 || en->type == ICECREAM))
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

			if ((en->type == LASER || en->type == PERMABLOCK || en->type == GAMEBLOCK || en->type == ICEBLOCK || en->type == ICECREAM || en->type == ENEMY_CRAWLER || en->type == ENEMY_BOXERGREG || en->type == ENEMY_SHOOTER || en->type == ENEMY_FAST))
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
	
	for (i = 0; i < currResultSize; i++)
	{
		if (currList[i]->base.type == HEART)
		{
			modPlayerHealth(1, HEALTH_UPGRADE_VALUE);

			pushParticle(LIFETEXT, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 0.0f, -1.8f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, -2.0f, -2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 2.0f, -2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 2.0f, 2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, -2.0f, 2.0f);
			
			playSFX(SFX_LIFEUP);

			currList[i]->base.type = DELETE_ME_PLEASE;
		}
	}

	if (pl->dead > 0)
	{
		if (pl->dead == 1 && currTime - pl->swordTimer > 1850)
		{
			pushParticle(SWEAT, pl->x * 16 + pl->offsetX, pl->y * 16 + pl->offsetY - 4, -2.0f, -2.0f);
			pushParticle(SWEAT, pl->x * 16 + pl->offsetX, pl->y * 16 + pl->offsetY - 4, 2.0f, -2.0f);
			
			pl->dead = 2;
		}
		return;
	}
	
	if (getPlayerHealth(1) < 1)
	{
		pl->swordTimer = currTime;
		pl->knockBackDirection = 255;
		pl->dead = 1;
		
		playSFX(SFX_PLAYER_DEATH);

		return;
	}
	
	//this is a quick way of preventing block timeout
	if (pl->holding != NULL)
	{
		pl->holding->startTime = getTimeSingleton();
	}
	
	if (pl->holdingSuperHammer == -1)
	{
		pushParticle(FIRE, (16 * pl->x) + pl->offsetX + ((xrand() % 17) - 8), (16 * pl->y) + pl->offsetY - 8, 2.0f, -2.0f);
	}

	// if the player gets completely knocked back, then he/she cannot do logic for that iteration
	if (pl->knockBackDirection != 255)
	{
		if (currTime - pl->swordTimer > 210)
		{
			switch (pl->knockBackDirection)
			{
				case 0:
				pl->direction = 2;
				break;
				case 1:
				pl->direction = 3;
				break;
				case 2:
				pl->direction = 0;
				break;
				case 3:
				pl->direction = 1;
				break;
				default:
				break;
			}

			pl->knockBackDirection = 255;
		}
		
		if (currTime - pl->swordTimer < 130)
		{
			switch(pl->knockBackDirection)
			{
				case 0:
				pl->offsetY -= 4;
				break;
				case 1:
				pl->offsetX += 4;
				break;
				case 2:
				pl->offsetY += 4;
				break;
				case 3:
				pl->offsetX -= 4;
				break;
				default:
				printf("Player knock back all odd! value:%d\n", pl->knockBackDirection);
				case 255:
				break;
			}
		}
	}
	
	for (i = 0; i < currResultSize; i++)
	{
		if (currList[i]->type == SUPERHAMMER && pl->holdingSuperHammer == 0)
		{
			pl->holdingSuperHammer = -1;
			currList[i]->type = DELETE_ME_PLEASE;
			
			playSFX(SFX_GET_HAMMER);
			
			if (xrand() % 10 == 0)
			{
				pushNewMessage("Hammertime!"); //obligatory
			}

			break;
		}
		
		if (currList[i]->type == SHIRUKEN)
		{
			if (pl->holdingSuperHammer < 0)
			{
				pl->holdingSuperHammer = 0;
			}
				
			pl->holdingSuperHammer += SHURIKEN_BONUS_VALUE;
			currList[i]->type = DELETE_ME_PLEASE;
			
			playSFX(SFX_GET_HAMMER);

			break;
		}
	}
	
	if (getKey(P1_B) && !(pl->bKeyDown) && pl->knockBackDirection == 255)
	{
		pl->bKeyDown = 1;
		pl->swordTimer = currTime;
		pl->isThrusting = 1;
		pl->thrustHit = 1;

		if (pl->holdingSuperHammer == 0)
		{
			playSFX(SFX_SWORD);
		}
		else if (pl->holdingSuperHammer > 0)
		{
			pl->thrustHit = 0;
			
			Laser* sk;

			switch (pl->direction)
			{
				case 0:
				sk = (Laser*)pushEntity(LASER, pl->x, pl->y - 1);
				sk->direction = 0;
				sk->offsetX = pl->offsetX;
				sk->offsetY = 15;
				break;
				case 1:
				sk = (Laser*)pushEntity(LASER, pl->x + 1, pl->y);
				sk->direction = 1;
				sk->offsetX = 1;
				sk->offsetY = pl->offsetY;
				break;
				case 2:
				sk = (Laser*)pushEntity(LASER, pl->x, pl->y + 1);
				sk->direction = 2;
				sk->offsetX = pl->offsetX;
				sk->offsetY = 1;
				break;
				case 3:
				sk = (Laser*)pushEntity(LASER, pl->x - 1, pl->y);
				sk->direction = 3;
				sk->offsetX = 15;
				sk->offsetY = pl->offsetY;
				break;
				default:
				fprintf(stderr, "Player facing odd direction for throwing\n");
				break;
			}
			
			playSFX(SFX_SWORD);
			
			pl->holdingSuperHammer--;
		}
		else
		{
			playSFX(SFX_EXPLOSION);
		}
	}
	else if (!getKey(P1_B) && pl->bKeyDown)
	{
		pl->bKeyDown = 0;
	}

	if (pl->isThrusting && pl->knockBackDirection == 255)
	{
		if (pl->holdingSuperHammer == -1)
		{
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
			if(pl->holdingSuperHammer == -1)
			{
				pl->holdingSuperHammer = 0;
			}

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
					if (pl->thrustHit == 1 && (northList[i]->type == ENEMY_CRAWLER || northList[i]->type == ENEMY_SHOOTER ||northList[i]->type == ENEMY_FAST || northList[i]->type == ENEMY_BOXERGREG))
					{
						northList[i]->enemy.knockBackDirection = pl->direction;
						northList[i]->enemy.offsetX = 8;
						northList[i]->enemy.offsetY = 9;
						northList[i]->enemy.health -= 1;
						northList[i]->enemy.timer = currTime;

						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}

					if (pl->thrustHit == 1 && (northList[i]->type == ICEBLOCK))
					{
						northList[i]->iBlock.moving = 1;
						northList[i]->iBlock.direction = pl->direction;
						northList[i]->iBlock.startTime = currTime;
						
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, -2.0f, 2.0f);
						
						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (northList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);

						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 8, pl->y * 16 + pl->offsetY - 10, -2.0f, -2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 8, pl->y * 16 + pl->offsetY - 10, 2.0f, -2.0f);

						northList[i]->laser.direction = 0;
					}
				}
			}
			break;
			case 1:
			if (eastResultSize > 0)
			{
				for (i = 0; i < eastResultSize; i++)
				{
					if (pl->thrustHit == 1 && (eastList[i]->type == ENEMY_CRAWLER || eastList[i]->type == ENEMY_SHOOTER || eastList[i]->type == ENEMY_FAST || eastList[i]->type == ENEMY_BOXERGREG))
					{
						eastList[i]->enemy.knockBackDirection = pl->direction;
						eastList[i]->enemy.offsetX = 9;
						eastList[i]->enemy.offsetY = 8;
						eastList[i]->enemy.health -= 1;
						eastList[i]->enemy.timer = currTime;
						
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (eastList[i]->type == ICEBLOCK))
					{
						eastList[i]->iBlock.moving = 1;
						eastList[i]->iBlock.direction = pl->direction;
						eastList[i]->iBlock.startTime = currTime;

						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, -2.0f, 2.0f);

						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (eastList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 10, pl->y * 16 + pl->offsetY - 8, 2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 10, pl->y * 16 + pl->offsetY - 8, 2.0f, -2.0f);

						eastList[i]->laser.direction = 1;
					}
				}
			}
			break;
			case 2:
			if (southResultSize > 0)
			{
				for (i = 0; i < southResultSize; i++)
				{
					if (pl->thrustHit == 1 && (southList[i]->type == ENEMY_CRAWLER || southList[i]->type == ENEMY_SHOOTER|| southList[i]->type == ENEMY_FAST || southList[i]->type == ENEMY_BOXERGREG))
					{
						southList[i]->enemy.knockBackDirection = pl->direction;
						southList[i]->enemy.offsetX = 8;
						southList[i]->enemy.offsetY = 7;
						southList[i]->enemy.health -= 1;
						southList[i]->enemy.timer = currTime;
						
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (southList[i]->type == ICEBLOCK))
					{
						southList[i]->iBlock.moving = 1;
						southList[i]->iBlock.direction = pl->direction;
						southList[i]->iBlock.startTime = currTime;
						
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, -2.0f, 2.0f);

						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (southList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 8, pl->y * 16 + pl->offsetY + 8, 2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 8, pl->y * 16 + pl->offsetY + 8, -2.0f, 2.0f);

						southList[i]->laser.direction = 2;
					}
				}
			}
			break;
			case 3:
			if (westResultSize > 0)
			{
				for (i = 0; i < westResultSize; i++)
				{
					if (pl->thrustHit == 1 && (westList[i]->type == ENEMY_CRAWLER || westList[i]->type == ENEMY_SHOOTER || westList[i]->type == ENEMY_FAST || westList[i]->type == ENEMY_BOXERGREG))
					{
						westList[i]->enemy.knockBackDirection = pl->direction;
						westList[i]->enemy.offsetX = 7;
						westList[i]->enemy.offsetY = 8;
						westList[i]->enemy.health -= 1;
						westList[i]->enemy.timer = currTime;

						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}

					if (pl->thrustHit == 1 && (westList[i]->type == ICEBLOCK))
					{
						westList[i]->iBlock.moving = 1;
						westList[i]->iBlock.direction = pl->direction;
						westList[i]->iBlock.startTime = currTime;
					
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, -2.0f, 2.0f);
						
						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (westList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 2, pl->y * 16 + pl->offsetY - 8, -2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 2, pl->y * 16 + pl->offsetY - 8, -2.0f, -2.0f);

						westList[i]->laser.direction = 3;
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

	if (getKey(P1_A) && !(pl->aKeyDown) && pl->knockBackDirection == 255)
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
				if (eastResultFullSize == 0 && pl->x != BOARD_WIDTH - 1)
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
				if (westResultFullSize == 0 && pl->x != 0)
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
	else if (!getKey(P1_A) && pl->aKeyDown && pl->knockBackDirection == 255)
	{
		pl->aKeyDown = 0;
	}

	if (getKey(P1_RIGHT) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetX > 8 && pl->x == BOARD_WIDTH - 1)
		{
			pl->offsetX = 8;
		}
	}
	
	if (getKey(P1_DOWN) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetY > 8 && pl->y == BOARD_BOTTOM_WALL - 1)
		{
			pl->offsetY = 8;
		}
	}
	
	if (getKey(P1_LEFT) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetX < 8 && pl->x == 0)
		{
			pl->offsetX = 8;
		}
	}
	
	if (getKey(P1_UP) && pl->knockBackDirection == 255)
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
	

	for (i = 0; i < currResultSize; i++)
	{
		if (currList[i]->base.type == HEART)
		{
			modPlayerHealth(2, HEALTH_UPGRADE_VALUE);
			
			pushParticle(LIFETEXT, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 0.0f, -1.8f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, -2.0f, -2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 2.0f, -2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, 2.0f, 2.0f);
			pushParticle(HEALTHSPARK, currList[i]->base.x * 16 + 8, currList[i]->base.y * 16 + 8, -2.0f, 2.0f);
			
			playSFX(SFX_LIFEUP);

			currList[i]->base.type = DELETE_ME_PLEASE;
		}
	}


	if (pl->dead > 0)
	{
		if (pl->dead == 1 && currTime - pl->swordTimer > 1850)
		{
			pushParticle(SWEAT, pl->x * 16 + pl->offsetX, pl->y * 16 + pl->offsetY - 4, -2.0f, -2.0f);
			pushParticle(SWEAT, pl->x * 16 + pl->offsetX, pl->y * 16 + pl->offsetY - 4, 2.0f, -2.0f);
			
			pl->dead = 2;
		}
		return;
	}
	
	if (getPlayerHealth(2) < 1)
	{
		pl->swordTimer = currTime;
		pl->knockBackDirection = 255;
		pl->dead = 1;
		
		playSFX(SFX_PLAYER_DEATH);

		return;
	}
	
	//this is a quick way of preventing block timeout
	if (pl->holding != NULL)
	{
		pl->holding->startTime = getTimeSingleton();
	}
	
	if (pl->holdingSuperHammer == -1)
	{
		pushParticle(FIRE, (16 * pl->x) + pl->offsetX + ((xrand() % 17) - 8), (16 * pl->y) + pl->offsetY - 8, 2.0f, -2.0f);
	}

	// if the player gets completely knocked back, then he/she cannot do logic for that iteration
	if (pl->knockBackDirection != 255)
	{
		if (currTime - pl->swordTimer > 210)
		{
			switch (pl->knockBackDirection)
			{
				case 0:
				pl->direction = 2;
				break;
				case 1:
				pl->direction = 3;
				break;
				case 2:
				pl->direction = 0;
				break;
				case 3:
				pl->direction = 1;
				break;
				default:
				break;
			}

			pl->knockBackDirection = 255;
		}
		
		if (currTime - pl->swordTimer < 130)
		{
			switch(pl->knockBackDirection)
			{
				case 0:
				pl->offsetY -= 4;
				break;
				case 1:
				pl->offsetX += 4;
				break;
				case 2:
				pl->offsetY += 4;
				break;
				case 3:
				pl->offsetX -= 4;
				break;
				default:
				printf("Player knock back all odd! value:%d\n", pl->knockBackDirection);
				case 255:
				break;
			}
		}
	}
	
	for (i = 0; i < currResultSize; i++)    
	{
		if (currList[i]->type == SUPERHAMMER && pl->holdingSuperHammer == 0)
		{
			pl->holdingSuperHammer = -1;
			currList[i]->type = DELETE_ME_PLEASE;
			
			playSFX(SFX_GET_HAMMER);
			
			if (xrand() % 10 == 0)
			{
				pushNewMessage("Hammertime!"); //obligatory
			}

			break;
		}
		
		if (currList[i]->type == SHIRUKEN)
		{
			if (pl->holdingSuperHammer < 0)
			{
				pl->holdingSuperHammer = 0;
			}
				
			pl->holdingSuperHammer += SHURIKEN_BONUS_VALUE;
			currList[i]->type = DELETE_ME_PLEASE;
			
			playSFX(SFX_GET_HAMMER);

			break;
		}
	}
	
	if (getKey(P2_B) && !(pl->bKeyDown) && pl->knockBackDirection == 255)
	{
		pl->bKeyDown = 1;
		pl->swordTimer = currTime;
		pl->isThrusting = 1;
		pl->thrustHit = 1;

		if (pl->holdingSuperHammer == 0)
		{
			playSFX(SFX_SWORD);
		}
		else if (pl->holdingSuperHammer > 0)
		{
			pl->thrustHit = 0;
			
			Laser* sk;

			switch (pl->direction)
			{
				case 0:
				sk = (Laser*)pushEntity(LASER, pl->x, pl->y - 1);
				sk->direction = 0;
				sk->offsetX = pl->offsetX;
				sk->offsetY = 15;
				break;
				case 1:
				sk = (Laser*)pushEntity(LASER, pl->x + 1, pl->y);
				sk->direction = 1;
				sk->offsetX = 1;
				sk->offsetY = pl->offsetY;
				break;
				case 2:
				sk = (Laser*)pushEntity(LASER, pl->x, pl->y + 1);
				sk->direction = 2;
				sk->offsetX = pl->offsetX;
				sk->offsetY = 1;
				break;
				case 3:
				sk = (Laser*)pushEntity(LASER, pl->x - 1, pl->y);
				sk->direction = 3;
				sk->offsetX = 15;
				sk->offsetY = pl->offsetY;
				break;
				default:
				fprintf(stderr, "Player facing odd direction for throwing\n");
				break;
			}
			
			playSFX(SFX_SWORD);
			
			pl->holdingSuperHammer--;
		}
		else
		{
			playSFX(SFX_EXPLOSION);
		}
	}
	else if (!getKey(P2_B) && pl->bKeyDown)
	{
		pl->bKeyDown = 0;
	}

	if (pl->isThrusting && pl->knockBackDirection == 255)
	{
		if (pl->holdingSuperHammer == -1)
		{
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
			if(pl->holdingSuperHammer == -1)
			{
				pl->holdingSuperHammer = 0;
			}

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
					if (pl->thrustHit == 1 && (northList[i]->type == ENEMY_CRAWLER || northList[i]->type == ENEMY_SHOOTER || northList[i]->type == ENEMY_FAST || northList[i]->type == ENEMY_BOXERGREG))
					{
						northList[i]->enemy.knockBackDirection = pl->direction;
						northList[i]->enemy.offsetX = 8;
						northList[i]->enemy.offsetY = 9;
						northList[i]->enemy.health -= 1;
						northList[i]->enemy.timer = currTime;

						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (northList[i]->enemy.x * 16) + northList[i]->enemy.offsetX, (northList[i]->enemy.y * 16) + northList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}

					if (pl->thrustHit == 1 && (northList[i]->type == ICEBLOCK))
					{
						northList[i]->iBlock.moving = 1;
						northList[i]->iBlock.direction = pl->direction;
						northList[i]->iBlock.startTime = currTime;
						
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, northList[i]->iBlock.x * 16, northList[i]->iBlock.y * 16, -2.0f, 2.0f);
						
						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (northList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);

						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 8, pl->y * 16 + pl->offsetY - 10, -2.0f, -2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 8, pl->y * 16 + pl->offsetY - 10, 2.0f, -2.0f);

						northList[i]->laser.direction = 0;
					}
				}
			}
			break;
			case 1:
			if (eastResultSize > 0)
			{
				for (i = 0; i < eastResultSize; i++)
				{
					if (pl->thrustHit == 1 && (eastList[i]->type == ENEMY_CRAWLER || eastList[i]->type == ENEMY_SHOOTER || eastList[i]->type == ENEMY_FAST || eastList[i]->type == ENEMY_BOXERGREG))
					{
						eastList[i]->enemy.knockBackDirection = pl->direction;
						eastList[i]->enemy.offsetX = 9;
						eastList[i]->enemy.offsetY = 8;
						eastList[i]->enemy.health -= 1;
						eastList[i]->enemy.timer = currTime;
						
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (eastList[i]->enemy.x * 16) + eastList[i]->enemy.offsetX, (eastList[i]->enemy.y * 16) + eastList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (eastList[i]->type == ICEBLOCK))
					{
						eastList[i]->iBlock.moving = 1;
						eastList[i]->iBlock.direction = pl->direction;
						eastList[i]->iBlock.startTime = currTime;

						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, eastList[i]->iBlock.x * 16, eastList[i]->iBlock.y * 16, -2.0f, 2.0f);

						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (eastList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 10, pl->y * 16 + pl->offsetY - 8, 2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX + 10, pl->y * 16 + pl->offsetY - 8, 2.0f, -2.0f);

						eastList[i]->laser.direction = 1;
					}
				}
			}
			break;
			case 2:
			if (southResultSize > 0)
			{
				for (i = 0; i < southResultSize; i++)
				{
					if (pl->thrustHit == 1 && (southList[i]->type == ENEMY_CRAWLER || southList[i]->type == ENEMY_SHOOTER || southList[i]->type == ENEMY_FAST || southList[i]->type == ENEMY_BOXERGREG))
					{
						southList[i]->enemy.knockBackDirection = pl->direction;
						southList[i]->enemy.offsetX = 8;
						southList[i]->enemy.offsetY = 7;
						southList[i]->enemy.health -= 1;
						southList[i]->enemy.timer = currTime;
						
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (southList[i]->enemy.x * 16) + southList[i]->enemy.offsetX, (southList[i]->enemy.y * 16) + southList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}
					
					if (pl->thrustHit == 1 && (southList[i]->type == ICEBLOCK))
					{
						southList[i]->iBlock.moving = 1;
						southList[i]->iBlock.direction = pl->direction;
						southList[i]->iBlock.startTime = currTime;
						
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, southList[i]->iBlock.x * 16, southList[i]->iBlock.y * 16, -2.0f, 2.0f);

						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (southList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 8, pl->y * 16 + pl->offsetY + 8, 2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 8, pl->y * 16 + pl->offsetY + 8, -2.0f, 2.0f);

						southList[i]->laser.direction = 2;
					}
				}
			}
			break;
			case 3:
			if (westResultSize > 0)
			{
				for (i = 0; i < westResultSize; i++)
				{
					if (pl->thrustHit == 1 && (westList[i]->type == ENEMY_CRAWLER || westList[i]->type == ENEMY_SHOOTER || westList[i]->type == ENEMY_FAST || westList[i]->type == ENEMY_BOXERGREG))
					{
						westList[i]->enemy.knockBackDirection = pl->direction;
						westList[i]->enemy.offsetX = 7;
						westList[i]->enemy.offsetY = 8;
						westList[i]->enemy.health -= 1;
						westList[i]->enemy.timer = currTime;

						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, -2.0f, -2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, 2.0f, -2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, -2.0f, 2.0f);
						pushParticle(BLOOD, (westList[i]->enemy.x * 16) + westList[i]->enemy.offsetX, (westList[i]->enemy.y * 16) + westList[i]->enemy.offsetY, 2.0f, 2.0f);

						playSFX(SFX_ENEMY_HURT);

						pl->thrustHit = 0;
					}

					if (pl->thrustHit == 1 && (westList[i]->type == ICEBLOCK))
					{
						westList[i]->iBlock.moving = 1;
						westList[i]->iBlock.direction = pl->direction;
						westList[i]->iBlock.startTime = currTime;
					
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, -2.0f, -2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, 2.0f, -2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, 2.0f, 2.0f);
						pushParticle(ICE, westList[i]->iBlock.x * 16, westList[i]->iBlock.y * 16, -2.0f, 2.0f);
						
						playSFX(SFX_ICE_TINK);

						pl->thrustHit = 0;
					}
					
					if (westList[i]->type == LASER && pl->holdingSuperHammer <= 0)
					{
						playSFX(SFX_LASER_1);
						
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 2, pl->y * 16 + pl->offsetY - 8, -2.0f, 2.0f);
						pushParticle(SWEAT, pl->x * 16 + pl->offsetX - 2, pl->y * 16 + pl->offsetY - 8, -2.0f, -2.0f);

						westList[i]->laser.direction = 3;
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

	if (getKey(P2_A) && !(pl->aKeyDown) && pl->knockBackDirection == 255)
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
				if (eastResultFullSize == 0 && pl->x != BOARD_WIDTH - 1)
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
				if (westResultFullSize == 0 && pl->x != 0)
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
	else if (!getKey(P2_A) && pl->aKeyDown && pl->knockBackDirection == 255)
	{
		pl->aKeyDown = 0;
	}

	if (getKey(P2_RIGHT) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetX > 8 && pl->x == BOARD_WIDTH - 1)
		{
			pl->offsetX = 8;
		}
	}
	
	if (getKey(P2_DOWN) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetY > 8 && pl->y == BOARD_BOTTOM_WALL - 1)
		{
			pl->offsetY = 8;
		}
	}
	
	if (getKey(P2_LEFT) && pl->knockBackDirection == 255)
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
		
		if (pl->offsetX < 8 && pl->x == 0)
		{
			pl->offsetX = 8;
		}
	}
	
	if (getKey(P2_UP) && pl->knockBackDirection == 255)
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
	
	if (enemy->cream == NULL)
	{
		filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}
	else
	{
		filterOccupyWallsAndIceCream(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWallsAndIceCream(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWallsAndIceCream(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWallsAndIceCream(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}

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
	
	if (enemy->cream != NULL)
	{
		if (enemy->cream->decoy == 1)
		{
			if (getTimeSingleton() - enemy->cream->decoyTime > 44 * 1000)
			{
				enemy->cream = NULL;
			}
		}
	}

	if(enemy->health < 1 && enemy->knockBackDirection == 255)
	{
		enemy->type = POOF;
		((Entity*)enemy)->poof.colour = 0;
		if (enemy->cream == NULL)
		{
			((Entity*)enemy)->poof.birthed = 1;
		}
		((Entity*)enemy)->poof.startTime = currTime;

		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;
		}
		
		addScore(50);
		
		playSFX(SFX_ENEMY_DIE);

		return;
	}
	
	if (enemy->x < 0)
	{
		if (enemy->cream != NULL)
		{
			if (enemy->cream->decoy == 0)
			{
				enemy->type = DELETE_ME_PLEASE;
			}
			
			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

			return;
		}
	}
	else if (enemy->x > BOARD_WIDTH - 1)
	{
		if (enemy->cream != NULL)
		{
			if (enemy->cream->decoy == 0)
			{
				enemy->type = DELETE_ME_PLEASE;
			}

			enemy->cream->type = DELETE_ME_PLEASE;
			enemy->cream = NULL;

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
			
			if (enemy->cream->decoy == 1)
			{
				playSFX(SFX_DECOY_YELL);
			}
			else
			{
				playSFX(SFX_PRINCESS_YELL);
			}
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
				
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, -2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, 2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, -2.0f, 1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, 2.0f, 1.9f);
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
				if (enemy->cream->decoy == 0)
				{
					enemy->type = DELETE_ME_PLEASE;
				}

				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;

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
				if (enemy->cream->decoy == 0)
				{
					enemy->type = DELETE_ME_PLEASE;
				}

				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;

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

	int i;
	int moveSpeed = 2;
	Uint32 delta = currTime - enemy->lastMovementUpdate;
	
	if (enemy->AISlot3 == 1)
	{
		moveSpeed = 4;
	}

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

	if (enemy->cream == NULL)
	{
		filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}
	else
	{
		filterOccupyWallsAndIceCream(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWallsAndIceCream(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWallsAndIceCream(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWallsAndIceCream(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}
	occupyingOnHere(enemy->x, enemy->y, currList, 5, &currListSize);
	
	if(enemy->health < 1 && enemy->knockBackDirection == 255)
	{
		enemy->type = POOF;
		if (enemy->cream == NULL)
		{
			((Entity*)enemy)->poof.birthed = 1;
		}
		((Entity*)enemy)->poof.colour = 0;
		((Entity*)enemy)->poof.startTime = currTime;
		
		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;
		}
		
		addScore(50);
		
		playSFX(SFX_ENEMY_DIE);

		return;
	}
	
	if (enemy->cream != NULL)
	{
		if (enemy->cream->decoy == 1)
		{
			if (getTimeSingleton() - enemy->cream->decoyTime > 44 * 1000)
			{
				enemy->cream = NULL;
			}
		}
	}

	for (i = 0; i < currListSize; i++)
	{
		if (currList[i]->type == ICECREAM && enemy->cream == NULL)
		{
			enemy->cream = (IceCream*)currList[i];
			enemy->cream->x = -5;
			enemy->cream->y = -5;
			
			if (enemy->cream->decoy == 1)
			{
				playSFX(SFX_DECOY_YELL);
			}
			else
			{
				playSFX(SFX_PRINCESS_YELL);
			}
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
				
				pushParticle(MUD, enemy->x * 16, enemy->y * 16 + 16, -2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16 + 16, enemy->y * 16 + 16, 2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16, enemy->y * 16 + 16, -2.0f, 1.9f);
				pushParticle(MUD, enemy->x * 16 + 16, enemy->y * 16 + 16, 2.0f, 1.9f);
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
			if (enemy->AISlot3 == 1)
			{
				if (xrand() % 4 == 0)
				{
					enemy->direction = newDir(enemy);
				}
			}
			else if (rand() % 10 == 0)
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
				
				if (xrand() % 2 == 0)
				{
					playSFX(SFX_LASER_1);
				}
				else
				{
					playSFX(SFX_LASER_2);
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
				if (enemy->cream->decoy == 0)
				{
					enemy->type = DELETE_ME_PLEASE;
				}

				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;

				return 3;
			}
			return 1;
		}
		else if (en->x >= BOARD_WIDTH)
		{
			if (en->cream != NULL)
			{
				if (enemy->cream->decoy == 0)
				{
					enemy->type = DELETE_ME_PLEASE;
				}

				en->cream->type = DELETE_ME_PLEASE;
				en->cream = NULL;

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
	int moveSpeed = 2;
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

	if (enemy->cream == NULL)
	{
		filterOccupyWalls(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWalls(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWalls(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWalls(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}
	else
	{
		filterOccupyWallsAndIceCream(enemy->x, enemy->y - 1, northList, 5, &northResultSize);
		filterOccupyWallsAndIceCream(enemy->x, enemy->y + 1, southList, 5, &southResultSize);
		filterOccupyWallsAndIceCream(enemy->x + 1, enemy->y, eastList, 5, &eastResultSize);
		filterOccupyWallsAndIceCream(enemy->x - 1, enemy->y, westList, 5, &westResultSize);
	}

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
	

	if(enemy->health < 1 && enemy->knockBackDirection == 255)
	{
		enemy->type = POOF;
		((Entity*)enemy)->poof.colour = 0;
		if (enemy->cream == NULL)
		{
			((Entity*)enemy)->poof.birthed = 1;
		}
		((Entity*)enemy)->poof.startTime = currTime;

		if (enemy->cream != NULL)
		{
			enemy->cream->x = enemy->x;
			enemy->cream->y = enemy->y;
		}

		addScore(50);
		
		playSFX(SFX_ENEMY_DIE);

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
	
	if (enemy->cream != NULL)
	{
		if (enemy->cream->decoy == 1)
		{
			if (getTimeSingleton() - enemy->cream->decoyTime > 44 * 1000)
			{
				enemy->cream = NULL;
			}
		}
	}

	for (i = 0; i < currListSize; i++)
	{
		if (currList[i]->type == ICECREAM && enemy->cream == NULL  && enemy->AISlot1 == 0)
		{
			enemy->cream = (IceCream*)currList[i];
			enemy->cream->x = -5;
			enemy->cream->y = -5;

			if (enemy->cream->decoy == 1)
			{
				playSFX(SFX_DECOY_YELL);
			}
			else
			{
				playSFX(SFX_PRINCESS_YELL);
			}
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

				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, -2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, 2.0f, -1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, -2.0f, 1.9f);
				pushParticle(MUD, enemy->x * 16 + 8, enemy->y * 16 + 16, 2.0f, 1.9f);
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

				enemy->direction = newDir(enemy);
				enemy->AISlot1 = 0;

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
								northList[i]->player.swordTimer = currTime;
								northList[i]->player.isThrusting = 0;
								
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, 2.0f, -2.0f);
							}
						}
						if (northList[i]->type == PLAYER2)
						{
							if (northList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								northList[i]->player.knockBackDirection = enemy->AISlot2;
								northList[i]->player.swordTimer = currTime;
								northList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (northList[i]->player.x * 16) + northList[i]->player.offsetX, (northList[i]->player.y * 16) + northList[i]->player.offsetY + 8, 2.0f, -2.0f);
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
								eastList[i]->player.swordTimer = currTime;
								eastList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, 2.0f, -2.0f);
							}
						}
						if (eastList[i]->type == PLAYER2)
						{
							if (eastList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								eastList[i]->player.knockBackDirection = enemy->AISlot2;
								eastList[i]->player.swordTimer = currTime;
								eastList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (eastList[i]->player.x * 16) + eastList[i]->player.offsetX, (eastList[i]->player.y * 16) + eastList[i]->player.offsetY + 8, 2.0f, -2.0f);
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
								southList[i]->player.swordTimer = currTime;
								southList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, 2.0f, -2.0f);
							}
						}
						if (southList[i]->type == PLAYER2)
						{
							if (southList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								southList[i]->player.knockBackDirection = enemy->AISlot2;
								southList[i]->player.swordTimer = currTime;
								southList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (southList[i]->player.x * 16) + southList[i]->player.offsetX, (southList[i]->player.y * 16) + southList[i]->player.offsetY + 8, 2.0f, -2.0f);
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
								westList[i]->player.swordTimer = currTime;
								westList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, 2.0f, -2.0f);
							}
						}
						if (westList[i]->type == PLAYER2)
						{
							if (westList[i]->player.knockBackDirection == 255)
							{
								modPlayerHealth(2, -1);
								westList[i]->player.knockBackDirection = enemy->AISlot2;
								westList[i]->player.swordTimer = currTime;
								westList[i]->player.isThrusting = 0;

								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, -2.0f, 2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, 2.0f, 2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, -2.0f, -2.0f);
								pushParticle(BLOOD, (westList[i]->player.x * 16) + westList[i]->player.offsetX, (westList[i]->player.y * 16) + westList[i]->player.offsetY + 8, 2.0f, -2.0f);
							}
						}
					}
					break;
					default:
					printf("Boxer Greg tried to punch in a bad direction: %d\n", enemy->AISlot2);
					break;
				}

				if (currTime - enemy->timer > 1000)
				{
					enemy->AISlot1 = 0;
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
		
		pushParticle(ICE, block->x * 16, block->y * 16, -2.0f, -1.9f);
		pushParticle(ICE, block->x * 16, block->y * 16, 2.0f, -1.9f);
		pushParticle(ICE, block->x * 16, block->y * 16, 2.0f, 1.9f);
		pushParticle(ICE, block->x * 16, block->y * 16, -2.0f, 1.9f);
		
		playSFX(SFX_ICE_TINK);
	}

	filterOccupyWallsAndIceCream(block->x, block->y - 1, northList, 5, &northResultSize);
	filterOccupyWallsAndIceCream(block->x, block->y + 1, southList, 5, &southResultSize);
	filterOccupyWallsAndIceCream(block->x + 1, block->y, eastList, 5, &eastResultSize);
	filterOccupyWallsAndIceCream(block->x - 1, block->y, westList, 5, &westResultSize);

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

	filterOccupyWallsAndIceCream(block->x, block->y - 1, northList, 5, &northResultSize);
	filterOccupyWallsAndIceCream(block->x, block->y + 1, southList, 5, &southResultSize);
	filterOccupyWallsAndIceCream(block->x + 1, block->y, eastList, 5, &eastResultSize);
	filterOccupyWallsAndIceCream(block->x - 1, block->y, westList, 5, &westResultSize);
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
			
			playSFX(SFX_PLAYER_HURT);

			pushParticle(BLOOD, (currList[i]->player.x * 16) + currList[i]->player.offsetX, (currList[i]->player.y * 16) + currList[i]->player.offsetY + 8, -2.0f, 2.0f);
			pushParticle(BLOOD, (currList[i]->player.x * 16) + currList[i]->player.offsetX, (currList[i]->player.y * 16) + currList[i]->player.offsetY + 8, 2.0f, 2.0f);
			pushParticle(BLOOD, (currList[i]->player.x * 16) + currList[i]->player.offsetX, (currList[i]->player.y * 16) + currList[i]->player.offsetY + 8, -2.0f, -2.0f);
			pushParticle(BLOOD, (currList[i]->player.x * 16) + currList[i]->player.offsetX, (currList[i]->player.y * 16) + currList[i]->player.offsetY + 8, 2.0f, -2.0f);

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
		
		if ((currList[i]->type == ENEMY_CRAWLER || currList[i]->type == ENEMY_SHOOTER || currList[i]->type == ENEMY_FAST || currList[i]->type == ENEMY_BOXERGREG))
		{
			pushParticle(BLOOD, (currList[i]->enemy.x * 16) + currList[i]->enemy.offsetX, (currList[i]->enemy.y * 16) + currList[i]->enemy.offsetY, -2.0f, -2.0f);
			pushParticle(BLOOD, (currList[i]->enemy.x * 16) + currList[i]->enemy.offsetX, (currList[i]->enemy.y * 16) + currList[i]->enemy.offsetY, 2.0f, -2.0f);
			pushParticle(BLOOD, (currList[i]->enemy.x * 16) + currList[i]->enemy.offsetX, (currList[i]->enemy.y * 16) + currList[i]->enemy.offsetY, -2.0f, 2.0f);
			pushParticle(BLOOD, (currList[i]->enemy.x * 16) + currList[i]->enemy.offsetX, (currList[i]->enemy.y * 16) + currList[i]->enemy.offsetY, 2.0f, 2.0f);

			currList[i]->enemy.health -= 3;

			block->type = DELETE_ME_PLEASE;
			return;
		}
	}
	
	if (getTimeSingleton() - block->lastFrameUpdate > 100)
	{
		block->frame = (block->frame + 1) % 2;
		
		block->lastFrameUpdate = getTimeSingleton();
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
			case ENEMY_FAST:
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
			if (checkList[i]->type == TELEBLOCK || checkList[i]->type == DELETE_ME_PLEASE || checkList[i]->type == EXPLOSION || checkList[i]->type == POOF)
			{
				continue;
			}
			else
			{
				checkList[i]->base.x = tb->twin->x;
				checkList[i]->base.y = tb->twin->y;
				tb->twin->type = DELETE_ME_PLEASE;
				tb->type = DELETE_ME_PLEASE;

				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, -2.0f, -2.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, 2.0f, -2.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, -2.0f, 2.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, 2.0f, 2.0f);
				
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, -2.0f, -4.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, 2.0f, -4.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, -2.0f, 4.0f);
				pushParticle(TELESPARK, (tb->x * 16) + 8, (tb->y * 16) + 8, 2.0f, 4.0f);

				playSFX(SFX_TELEPORT);

				break;
			}
		}
	}
}

void update_glue(FloorGlue* gl)
{
	Entity* checkList[5];
	int checkResultSize = 0;
	occupyingOnHere(gl->x, gl->y, checkList, 5, &checkResultSize);

	if (getTimeSingleton() - gl->startTime > 30 * 1000)
	{
		gl->type = DELETE_ME_PLEASE;
	}

	if (gl->population != checkResultSize)
	{
		playSFX(SFX_MUD2);
		gl->population = checkResultSize;
	}
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

void update_poof(Poof* pf)
{
	if (pf->birthed == 1 && getTimeSingleton() - pf->startTime > 125)
	{
		Entity* decoyPrincePointer; 

		switch (pf->colour)
		{
			case 0:
			if (xrand() % 15 == 0)
			{
				pushEntity(HEART, pf->x, pf->y);
			}
			break;
			case 1:
			pushEntity(SHIRUKEN, pf->x, pf->y);
			break;
			case 2:
			pushEntity(TELEBLOCK, pf->x, pf->y);
			break;
			case 3:
			pushEntity(GLUE, pf->x, pf->y);
			break;
			case 4:
			decoyPrincePointer = pushEntity(ICECREAM, pf->x, pf->y);
			if (decoyPrincePointer != NULL)
			{
				decoyPrincePointer->iceCream.decoy = 1;
			}	
			break;
			default:
			break;
		}

		pf->birthed = 0;
	}

	if (getTimeSingleton() - pf->startTime > 250)
	{
		pf->type = DELETE_ME_PLEASE;
	}
}

void update_iceCream(IceCream* cream)
{
	if (getTimeSingleton() - cream->lastFrameUpdateTime > 255)
	{
		cream->frame = (cream->frame + 1) % 2;
		
		cream->lastFrameUpdateTime = getTimeSingleton();
	}
	
	if (cream->decoy == 1 && getTimeSingleton() - cream->decoyTime > 45 * 1000)
	{
		cream->type = DELETE_ME_PLEASE;
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
		case ENEMY_FAST:
		case ENEMY_SHOOTER:
		update_shooter( (Enemy*)entity, currTime);
		break;
		case ENEMY_BOXERGREG:
		update_boxergreg( (Enemy*)entity, currTime);
		break;
		case SUPERHAMMER:
		if (xrand() % 5 == 0)
		{
			pushParticle(FIRE, (16 * entity->base.x) + (xrand() % 17), (16 * entity->base.y), 2.0f, -1.2f);
		}
		break;
		case SHIRUKEN:
		break;
		case GLUE:
		update_glue( (FloorGlue*)entity);
		break;
		case ICECREAM:
		update_iceCream( (IceCream*)entity);
		break;
		case POOF:
		update_poof( (Poof*)entity);
		break;
		case DELETE_ME_PLEASE:
		break;
		case HEART:
		break;
		default:
		printf("unregognized entity type updated: %d\n", entity->type);
		break;
	}
	
	return;
}
