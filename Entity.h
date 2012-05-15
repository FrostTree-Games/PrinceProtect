/*
 * blah blah blah
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <SDL/SDL.h>

enum EntityType
{
	PLAYER1,
	PLAYER2,
	PERMABLOCK,
	TEMPBLOCK,
	BULLET,
	DUMBENEMY,
	DELETE_ME_PLEASE
};

struct Player
{
	EntityType type;
	int x;
	int y;
	int width;
	int height;
	float xSpeed;
	float ySpeed;
	unsigned char direction; //0->N, 1->E, 2->S, 3->W
	unsigned char health;
	unsigned char maxhealth;
	unsigned char slot; //one or zero
	Uint32 lastAnimUpdate;
};

struct PermaBlock
{
	EntityType type;
	int x;
	int y;
	int width;
	int height;
};

// viva la crude polymorphism!
union Entity
{
	Player playerEntity;
	PermaBlock permaBlockEntity;
};

#endif
