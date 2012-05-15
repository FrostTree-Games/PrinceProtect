#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Entity.h"

Entity* create_entity(EntityType type, int newX, int newY)
{
	Entity* newEntity = malloc(sizeof(Entity));
	newEntity->type = type;
	
	switch (type)
	{
		case PLAYER1:
		newEntity->player.x = newX;
		newEntity->player.y = newY;
                newEntity->player.width = 1;
                newEntity->player.height = 1;
                newEntity->player.health = 20;
                newEntity->player.maxhealth = 20;
                newEntity->player.lastUpdate = SDL_GetTicks();
		break;
		case PERMABLOCK:
		newEntity->permaBlock.x = newX;
		newEntity->permaBlock.y = newY;
		newEntity->permaBlock.width = 1;
		newEntity->permaBlock.height = 1;
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

void update_player(Player* pl, Uint32 currTime)
{
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
		default:
		printf("unregognized entity type updated\n");
		break;
	}
	
	return;
}
