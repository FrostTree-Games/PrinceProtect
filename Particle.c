#include <SDL/SDL.h>

#include "Entity.h" // for time singleton
#include "Draw.h"
#include "Particle.h"

#define ICE_PARTICLE_TIMEOUT 500

// Master list of all particles in motion
Particle* particleList = NULL; // set to NULL when uninitalized

int initalizeParticleList()
{
	if (particleList != NULL)
	{
		return 1;
	}

	particleList = (Particle*)calloc(PARTICLE_LIST_MAX_SIZE, sizeof(Particle));

	if (particleList == NULL)
	{
		return 0;
	}
	
	return 1;
}

void freeParticleList()
{
	free(particleList);
	
	particleList = NULL;
}

void updateParticles()
{
	int i;
	
	for (i = 0; i < PARTICLE_LIST_MAX_SIZE; i++)
	{
		if (particleList[i].type == NONE)
		{
			continue;
		}
		
		if (particleList[i].x > SCREEN_WIDTH - 3 || particleList[i].x < 3 || particleList[i].y > SCREEN_HEIGHT - 3 || particleList[i].y < 3)
		{
			particleList[i].type = NONE;
			continue;
		}
		
		particleList[i].x += particleList[i].xVelo;
		particleList[i].y += particleList[i].yVelo;

		//update any logic necessary
		switch(particleList[i].type)
		{
			case ICE:
			if (getTimeSingleton() - particleList[i].startTime > 250)
			{
				particleList[i].type = NONE;
			}
			break;
			case MUD:
			if (getTimeSingleton() - particleList[i].startTime > 100)
			{
				particleList[i].type = NONE;
			}
			break;
			default:
			break;
		}
	}
}

int pushParticle(ParticleType p, float newX, float newY, float newXSpeed, float newYSpeed)
{
	int i;
	
	for (i = 0; i < PARTICLE_LIST_MAX_SIZE; i++)
	{
		if (particleList[i].type != NONE)
		{
			continue;
		}
		else
		{
			particleList[i].type = p;
			particleList[i].x = newX;
                        particleList[i].y = newY;
                        particleList[i].xVelo = newXSpeed;
                        particleList[i].yVelo = newYSpeed;
                        particleList[i].startTime = getTimeSingleton();
                        
                        return 1;
		}
	}

	return 0;
}

Particle* getParticleList()
{
	return particleList;
}

