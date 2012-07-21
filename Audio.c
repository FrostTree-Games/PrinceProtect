#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "Audio.h"

//only 1 or two channels are possible
#define NUMBER_OF_CHANNELS 2

int audioInitalized = 0;

Mix_Chunk* swordStrike;

int loadSFXFiles()
{
	int returnVal = 0;

	if ( (swordStrike = Mix_LoadWAV("aud/sfx/sword.wav")) == NULL)
	{
		fprintf(stderr, "Error loading sword sound effect. Check for aud/sfx/sword.wav\n");
		returnVal = 1;
	}

	return returnVal;
}

int setupAudio()
{
	// load support for the OGG sample/music formats
	int flags=MIX_INIT_OGG;
	int initted=Mix_Init(flags);
	if(initted&flags != flags)
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
	Mix_CloseAudio();
	
	audioInitalized = 0;

	return;
}

void playSFX(SFXType fx)
{
	if (fx == SFX_SWORD)
	{
		Mix_PlayChannel(-1, swordStrike, 0);
	}
}

