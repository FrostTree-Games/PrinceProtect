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

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 13
#define BOARD_TOP_WALL 5
#define BOARD_BOTTOM_WALL 14

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
	ENEMY_BOXERGREG,
	ENEMY_FAST,
	ICECREAM,
	SUPERHAMMER,
	SHIRUKEN,
	GLUE,
	POOF,
	HEART,
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
	Uint32 startTime;
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
	Uint32 startTime; //icebergs slowly melt and lose health
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
	unsigned char allegiance; //0 -> damagers player; 1 -> damagers enemies
	int frame; //0 or 1
	Uint32 lastFrameUpdate;
} Laser;

typedef struct
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
} Explosion;

typedef struct
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
	int offsetX;
	int offsetY;
	int colour; //0 -> plain, 1 -> red, 2 -> blue, 3 -> green, 4 -> yellow, 5 ->purple
	int birthed; //to determine if a coloured poof has created an entity yet; 0 -> yes/uncoloured, 1 -> no
} Poof;

typedef struct TeleBlock
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
	int side; // 0 = west,  = east
	struct TeleBlock* twin;
	Uint32 lastClickTime;
	int frame; //0 -> first frame; 1 -> second frame
} TeleBlock;

typedef struct
{
	EntityType type;
	int x;
	int y;
	int frame; //0 or 1
	Uint32 lastFrameUpdateTime;
	int decoy; //0 -> real, 1 -> decoy
	Uint32 decoyTime;
} IceCream;

typedef struct
{
	EntityType type;
	int x;
	int y;
} Shiruken;

typedef struct
{
	EntityType type;
	int x;
	int y;
} SuperHammer;

typedef struct
{
	EntityType type;
	int x;
	int y;
	Uint32 startTime;
	int population; //glue keeps track of how many enemies are stuck on it
} FloorGlue;

// players 1 and 2 entity types
typedef struct
{
	EntityType type;
	int x;
	int y;
	int offsetX;
	int offsetY;
	unsigned char direction; //0->N, 1->E, 2->S, 3->W
	unsigned char upKeyDown; //for checking if keys have been typed, for staggered movement
	unsigned char downKeyDown;
	unsigned char leftKeyDown;
	unsigned char rightKeyDown;
	unsigned char aKeyDown;
	unsigned char bKeyDown;
	Uint32 swordTimer; //this generally gets used as a timeout for striking, but also for getting knocked back and dying animations
	int isThrusting; // 0 = false, 1 = true
	int thrustHit; // 0 = false, 1 = true (prevents monster from getting damaged too many times)
	GameBlock* holding;
	Uint32 lastUpdate;
	unsigned char knockBackDirection; //0->N, 1->E, 2->S, 3->W, 255->NOT KNOCKED BACK
	Uint32 lastFrameUpdate;
	int frame; //0 -> first frame ; 1 -> second frame
	int holdingSuperHammer; //0 -> false; -1 -> true; 1... -> number of shiruken on hand
	int dead; //0 -> false ; 1 -> true ; 2 -> true
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
	int AISlot1; //boxerGreg: 0 -> not punching, 1 -> punching
	int AISlot2; //boxerGreg: direction currently standing when direction is 4
	int AISlot3; //last-minute hack: differentiates between shooter and quickie (0 = shooter, 1 = quickie)
	Uint32 timer;
	Uint32 lastFrameUpdate;
	int frame; //0 -> first frame ; 1 -> second frame
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
	SuperHammer hammer;
	Shiruken shiruken;
	FloorGlue glue;
	Enemy enemy;
	Poof poof;
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
