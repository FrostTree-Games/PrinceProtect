#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "Audio.h"

//only 1 or two channels are possible
#define NUMBER_OF_CHANNELS 2

int audioInitalized = 0;

Mix_Chunk* menuClick;
Mix_Chunk* explosion;
Mix_Chunk* player_hurt;
Mix_Chunk* player_die;
Mix_Chunk* ice;
Mix_Chunk* laser;
Mix_Chunk* laser_alt;
Mix_Chunk* mudSplat;
Mix_Chunk* swordStrike;
Mix_Chunk* teleport;
Mix_Chunk* princessYell;

//disgusting implementation? perhaps.
int loadSFXFiles()
{
	int returnVal = 0;

	if ( (player_die = Mix_LoadWAV("aud/sfx/die.wav")) == NULL)
	{
		fprintf(stderr, "Error loading knockout sound effect. Check for aud/sfx/die.wav\n");
	}
	if ( (menuClick = Mix_LoadWAV("aud/sfx/menu.wav")) == NULL)
	{
		fprintf(stderr, "Error loading menu sound effect. Check for aud/sfx/menu.wav\n");
	}
	if ( (explosion = Mix_LoadWAV("aud/sfx/explosion.wav")) == NULL)
	{
		fprintf(stderr, "Error loading explosion sound effect. Check for aud/sfx/explosion.wav\n");
	}
	if ( (player_hurt = Mix_LoadWAV("aud/sfx/hurt.wav")) == NULL)
	{
		fprintf(stderr, "Error loading player damage sound effect. Check for aud/sfx/hurt.wav\n");
	}
	if ( (ice = Mix_LoadWAV("aud/sfx/ice.wav")) == NULL)
	{
		fprintf(stderr, "Error loading ice sound effect. Check for aud/sfx/ice.wav\n");
	}
	if ( (laser = Mix_LoadWAV("aud/sfx/laser.wav")) == NULL)
	{
		fprintf(stderr, "Error loading shiruken sound effect. Check for aud/sfx/laser.wav\n");
	}
	if ( (laser_alt = Mix_LoadWAV("aud/sfx/laser_alt.wav")) == NULL)
	{
		fprintf(stderr, "Error loading second shiruken sound effect. Check for aud/sfx/laser_alt.wav\n");
	}
	if ( (mudSplat = Mix_LoadWAV("aud/sfx/mud.wav")) == NULL)
	{
		fprintf(stderr, "Error loading glue sound effect. Check for aud/sfx/mud.wav\n");
	}
	if ( (swordStrike = Mix_LoadWAV("aud/sfx/sword.wav")) == NULL)
	{
		fprintf(stderr, "Error loading sword sound effect. Check for aud/sfx/sword.wav\n");
	}
	if ( (teleport = Mix_LoadWAV("aud/sfx/teleport.wav")) == NULL)
	{
		fprintf(stderr, "Error loading teleport sound effect. Check for aud/sfx/teleport.wav\n");
	}
	if ( (princessYell = Mix_LoadWAV("aud/sfx/princess.wav")) == NULL)
	{
		fprintf(stderr, "Error loading princess yelling sound effect. Check for aud/sfx/princess.wav\n");
	}

	return returnVal;
}

int setupAudio()
{
	// load support for the OGG sample/music formats
	int flags=MIX_INIT_OGG;
	int initted=Mix_Init(flags);
	if((initted & flags) != flags)
	{
		fprintf(stderr, "Mix_Init: Failed to init required ogg support!\n");
		fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
	}

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, NUMBER_OF_CHANNELS, 4096) == -1)
	{
		fprintf(stderr, "Error initalizing SDL_mixer. Check various libs.\n");
		fprintf(stderr, "Mix_OpenAudio: %s\n", Mix_GetError());
		return 1;
	}
	
	if (loadSFXFiles() == 1)
	{
		return 1;
	}
	
	audioInitalized = 1;

	return 0;
}

void clearAudio()
{
	Mix_FreeChunk(explosion);
	Mix_FreeChunk(player_hurt);
	Mix_FreeChunk(ice);
	Mix_FreeChunk(laser);
	Mix_FreeChunk(laser_alt);
	Mix_FreeChunk(mudSplat);
	Mix_FreeChunk(swordStrike);
	Mix_FreeChunk(teleport);
	Mix_FreeChunk(princessYell);

	Mix_CloseAudio();
	
	audioInitalized = 0;

	return;
}

void playSFX(SFXType fx)
{
	if (fx == SFX_NONE)
	{
		return;
	}
	if (fx == SFX_EXPLOSION)
	{
		Mix_PlayChannel(-1, explosion, 0);
	}
	if (fx == SFX_PLAYER_HURT)
	{
		Mix_PlayChannel(-1, player_hurt, 0);
	}
	if (fx == SFX_PLAYER_DEATH)
	{
		Mix_PlayChannel(-1, player_die, 0);
	}
	if (fx == SFX_ICE_TINK)
	{
		Mix_PlayChannel(-1, ice, 0);
	}
	if (fx == SFX_LASER_1)
	{
		Mix_PlayChannel(-1, laser, 0);
	}
	if (fx == SFX_LASER_2)
	{
		Mix_PlayChannel(-1, laser_alt, 0);
	}
	if (fx == SFX_MENU)
	{
		Mix_PlayChannel(-1, menuClick, 0);
	}
	if (fx == SFX_MUD)
	{
		Mix_PlayChannel(-1, mudSplat, 0);
	}
	if (fx == SFX_SWORD)
	{
		Mix_PlayChannel(-1, swordStrike, 0);
	}
	if (fx == SFX_TELEPORT)
	{
		Mix_PlayChannel(-1, teleport, 0);
	}
	if (fx == SFX_PRINCESS_YELL)
	{
		Mix_PlayChannel(-1, princessYell, 0);
	}
}

