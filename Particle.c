#include <math.h>

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
		
		if (particleList[i].x > SCREEN_WIDTH - 3 || particleList[i].x < 1 || particleList[i].y > SCREEN_HEIGHT - 3 || particleList[i].y < 3)
		{
			particleList[i].type = NONE;
			continue;
		}
		
		if (particleList[i].type != TELESPARK)
		{
			particleList[i].x += particleList[i].xVelo;
			particleList[i].y += particleList[i].yVelo;
		}
		else
		{
			Uint32 delta = (getTimeSingleton() - particleList[i].startTime)/100;
			particleList[i].x += particleList[i].xVelo;
			particleList[i].y += (delta + exp(0.20 * delta) * sin(10.0 * delta)) * particleList[i].yVelo/2.0f;
		}

		//update any logic necessary
		switch(particleList[i].type)
		{
			case TELESPARK:
			case ICE:
			if (getTimeSingleton() - particleList[i].startTime > 250)
			{
				particleList[i].type = NONE;
			}
			break;
			case BLOOD:
			case SWEAT:
			particleList[i].yVelo += 0.4f;

			if (getTimeSingleton() - particleList[i].startTime > 200)
			{
				particleList[i].type = NONE;
			}
			break;
			case TEARS:
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

