/*
 * Particle.h
 * --------
 * This header file covers functionality and logic for particle effects.
 *
 * The list of particles is always PARTICLE_LIST_MAX_SIZE at most. Adding,
 * removing, updating is almost always expected to be O(PARTICLE_LIST_MAX_SIZE)
 * big-O notation time. Shush, I'm lazy.
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL/SDL.h>

#define PARTICLE_LIST_MAX_SIZE 100

typedef enum
{
	NONE = 0,
	ICE,
	MUD,
	BLOOD,
	SWEAT,
	TEARS, //not used, but funny
} ParticleType;

typedef struct
{
	ParticleType type;
	float x, y;
	float xVelo, yVelo;
	Uint32 startTime;
} Particle;

/* initalizeParticleList()
 * Purpose: Creates the memory for particles
 * Returns: 0 on failure, 1 on success
 */
int initalizeParticleList();

/* freeParticleList()
 * Purpose: Frees the list of particles
 * Returns: n/a
 */
void freeParticleList();

/* updateParticles();
 * Purpose: Updates particles
 * Returns: n/a
 */
void updateParticles();

/* pushParticle();
 * Purpose: Adds a new particle to the list
 * Returns: 0 on failure, 1 on success
 */
int pushParticle(ParticleType p, float newX, float newY, float newXSpeed, float newYSpeed);

/* getParticleList();
 * Purpose: gets the list of particles; used primarily for the draw function
 * Returns: NULL on failure, pointer on success
 */
Particle* getParticleList();

#endif