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
	GAMEBLOCK,
	ICEBLOCK,
	EXPLOSION,
	LASER,
	TELEBLOCK,
	ENEMY_CRAWLER,
	ENEMY_SHOOTER,
	ICECREAM,
	DELETE_ME_PLEASE
} EntityType;

//type identifier for the GameBlock
typedef enum
{
	NO_BLOCK,
	RED_BLOCK,
	BLUE_BLOCK,
	GREEN_BLOCK,
	YELLOW_BLOCK
} BlockType;

//entity base (for width, height and whatnot)
typedef struct
{
	EntityType type;
	int x;
	int y;
} entityBase;

// entity type for a wall; classy
typedef struct
{
	EntityType type;
	int x;
	int y;
} PermaBlock;

// a coloured game block to be placed and moved around
typedef struct
{
	EntityType type;
	int x;
	int y;
	int height;
	Uint32 lastUpdateTime;
	BlockType bType;
} GameBlock;

// a block of ice; can be pushed by the player
typedef struct
{
	EntityType type;
	int x;
	int y;
	int moving; // 0 = false, 1 = true
	unsigned char direction; //0->N, 1->E, 2->S, 3->W
	int offsetX;
	int offsetY;
	Uint32 lastMovementUpdate;
	int health; //from 0 to 3; 3 is a perfect iceberg; 0 will mark the iceberg for deletion
} IceBlock;

// a laser projectile
typedef struct
{
	EntityType type;
	int x;
	int y;
	unsigned char direction; //0->N, 1->E, 2->S, 3->W
	int offsetX;
	int offsetY;
	Uint32 lastMovementUpdate;
} Laser;

typedef struct
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
} Explosion;

typedef struct TeleBlock
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
	int side; // 0 = west,  = east
	struct TeleBlock* twin;
} TeleBlock;

typedef struct
{
	EntityType type;
	int x;
	int y;
} IceCream;

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
	unsigned char aKeyDown;
	unsigned char bKeyDown;
	Uint32 swordTimer;
	int isThrusting; // 0 = false, 1 = true
	GameBlock* holding;
	Uint32 lastUpdate;
} Player;

typedef struct
{
	EntityType type;
	int x;
	int y;
	int direction; //4 = still
	int offsetX;
	int offsetY;
	int health;
	Uint32 lastMovementUpdate;
	unsigned char knockBackDirection; // 255->no knock back, 0->N, 1->E, 2->S, 3->W
	IceCream* cream;
} Enemy;

// viva la crude polymorphism!
typedef union
{
	EntityType type;
	entityBase base;
	Player player;
	PermaBlock permaBlock;
	GameBlock gBlock;
	IceBlock iBlock;
	Explosion exp;
	Laser laser;
	TeleBlock tBlock;
	IceCream iceCream;
	Enemy enemy;
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

/* filterOccupyType
 * Purpose: Returns a list of the entities on a specified coordinate of only one type
 * Returns: 1 if there are any entities, 0 if none
 * Note: The caller of this function must allociate and specify the size of the list
 *       to fill. This could have the program crash or cause unwarranted effects.
 */
int filterOccupyType(int x, int y, Entity** list, int listMaxSize, int* returnedSize, EntityType type);

/* update_entity()
 * Purpose: Updates entity logic
 * Returns: n/a
 */
void update_entity(Entity* entity, Uint32 currTime);

// MOVE THIS TO A GAME LOGIC OBEJCT LATER
/* [get/set]TimeSingleton(..)
 * Purpose: Keeps a consistent time value for each iteration
 * Returns: void, Uint32
 */
Uint32 getTimeSingleton();
void setTimeSingleton(Uint32 newTime);

#endif
