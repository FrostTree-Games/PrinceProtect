/*
 * Entity.h
 * --------
 * This header file contains definitions for methods used with game entities.
 * An entity is one of a variety of structs, covered by a union.
 * The first four bytes of an entity must always represent its enumerated type.
 * The next sixteen bytes must always be coordinates, width, and height
 *
 * The master entity list cannot be guarunteed to maintain its order.
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <SDL/SDL.h>

typedef enum
{
	PLAYER1 = 0,
	PLAYER2,
	PERMABLOCK,
	DELETE_ME_PLEASE
} EntityType;

typedef struct
{
	EntityType type;
	int x;
	int y;
} entityBase;

// players 1 and 2 entity types
typedef struct
{
	EntityType type;
	int x;
	int y;
	unsigned char direction; //0->N, 1->E, 2->S, 3->W
	unsigned char upKeyDown; //for checking if keys have been typed, for staggered movement
	unsigned char downKeyDown;
	unsigned char leftKeyDown;
	unsigned char rightKeyDown;
	Uint32 lastUpdate;
} Player;

// entity type for a wall; classy
typedef struct
{
	EntityType type;
	int x;
	int y;
} PermaBlock;

// viva la crude polymorphism!
typedef union
{
	EntityType type;
	entityBase base;
	Player player;
	PermaBlock permaBlock;
} Entity;

/* initEntityList()
 * Purpose: Allociates the entity list to RAM
 * Returns: 1 on success, 0 on failure, 2 if the list already exists
 */
int initEntityList();

/* freeEntityList()
 * Purpose: Frees the entity list from RAM
 * Returns: 1 on success, 2 if the list doesn't exist
 */
int freeEntityList();

/* getEntityListSize()
 * Purpose: Gets the current size of the entity list
 * Returns: Value between 0 and getEntityListMaxSize()
 */
int getEntityListSize();

/* getEntityListMaxSize()
 * Purpose: Gets the maximum size of the entity list
 * Returns: 1 on success, 2 if the list doesn't exist
 */
int getEntityListMaxSize();

/* getEntityList()
 * Purpose: returns the pointer to a list of entity pointers
 * Returns: Entity** of entityListSize;
 */
Entity** getEntityList();

/* create_entity()
 * Purpose: Creates a new entity on the heap according to the specified information
 * Returns: Newly-created entity pointer on success, NULL on failure
 */
Entity* create_entity(EntityType type, int newX, int newY);

/* destroy_entity()
 * Purpose: Deletes Entity from memory
 * Returns: n/a
 */
void destroy_entity(Entity* entity);

/* pushEntity()
 * Purpose: Puts a new entity on the master list
 * Returns: pointer to entity; NULL if failed
 */
Entity* pushEntity(EntityType type, int newX, int newY);

/* popEntity()
 * Purpose: Removes a specified entity from the master list
 * Returns: 1 on success, 0 on failure
 */
int popEntity(Entity* entity);

/* occupyingOnHere
 * Purpose: Returns a list of the entities on a specified coordinate
 * Returns: 1 if there are any entities, 0 if none
 * Note: The caller of this function must allociate and specify the size of the list
 *       to fill. This could have the program crash or cause unwarranted effects.
 */
int occupyingOnHere(int x, int y, Entity** list, int listMaxSize, int* returnedSize);

/* update_entity()
 * Purpose: Updates entity logic
 * Returns: n/a
 */
void update_entity(Entity* entity, Uint32 currTime);

#endif
