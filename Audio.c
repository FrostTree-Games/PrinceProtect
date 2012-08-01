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
Mix_Chunk* mudSplat2;
Mix_Chunk* swordStrike;
Mix_Chunk* teleport;
Mix_Chunk* princessYell;
Mix_Chunk* crash;
Mix_Chunk* wipe_open;
Mix_Chunk* wipe_close;
Mix_Chunk* enemy_die;
Mix_Chunk* enemy_hurt;
Mix_Chunk* get_hammer;
Mix_Chunk* readyJingle;
Mix_Chunk* losePrincess;

Mix_Music* bgm1;
Mix_Music* bgm2;
Mix_Music* bgm3;
Mix_Music* titleSong;

//disgusting implementation? perhaps.
int loadSFXFiles()
{
	if ( (wipe_open = Mix_LoadWAV("aud/sfx/open.wav")) == NULL)
	{
		fprintf(stderr, "Error loading wipe 1 sound effect. Check for aud/sfx/open.wav\n");
	}
	if ( (wipe_close = Mix_LoadWAV("aud/sfx/close.wav")) == NULL)
	{
		fprintf(stderr, "Error loading wipe 2 sound effect. Check for aud/sfx/close.wav\n");
	}
	if ( (player_die = Mix_LoadWAV("aud/sfx/die.wav")) == NULL)
	{
		fprintf(stderr, "Error loading knockout sound effect. Check for aud/sfx/die.wav\n");
	}
	if ( (crash = Mix_LoadWAV("aud/sfx/crash.wav")) == NULL)
	{
		fprintf(stderr, "Error loading knockout crash effect. Check for aud/sfx/crash.wav\n");
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
		fprintf(stderr, "Error loading poof sound effect. Check for aud/sfx/mud.wav\n");
	}
	if ( (mudSplat2 = Mix_LoadWAV("aud/sfx/mud2.wav")) == NULL)
	{
		fprintf(stderr, "Error loading glue sound effect. Check for aud/sfx/mud2.wav\n");
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
		fprintf(stderr, "Error loading prince yelling sound effect. Check for aud/sfx/princess.wav\n");
	}
	if ( (enemy_die = Mix_LoadWAV("aud/sfx/enemyKill.wav")) == NULL)
	{
		fprintf(stderr, "Error loading enemy death sound effect. Check for aud/sfx/enemyKill.wav\n");
	}
	if ( (enemy_hurt = Mix_LoadWAV("aud/sfx/enemyHurt.wav")) == NULL)
	{
		fprintf(stderr, "Error loading enemy damage sound effect. Check for aud/sfx/enemyHurt.wav\n");
	}
	if ( (get_hammer = Mix_LoadWAV("aud/sfx/getHammer.wav")) == NULL)
	{
		fprintf(stderr, "Error loading hammer sound effect. Check for aud/sfx/getHammer.wav\n");
	}
	if ( (readyJingle = Mix_LoadWAV("aud/sfx/ready.wav")) == NULL)
	{
		fprintf(stderr, "Error loading ready Jingle effect. Check for aud/sfx/ready.wav\n");
	}
	if ( (losePrincess = Mix_LoadWAV("aud/sfx/losePrincess.wav")) == NULL)
	{
		fprintf(stderr, "Error loading prince escape effect. Check for aud/sfx/losePrincess.wav\n");
	}

	return 0;
}

int loadBGMFiles()
{
	if ( (bgm1 = Mix_LoadMUS("aud/bgm/artifical_turf.it")) == NULL)
	{
		fprintf(stderr, "Error loading background music 1. Check for aud/sfx/artifical_turf.it\n");
	}
	if ( (bgm2 = Mix_LoadMUS("aud/bgm/pretty_weeds.it")) == NULL)
	{
		fprintf(stderr, "Error loading background music 2. Check for aud/sfx/pretty_weeds.it\n");
	}
	if ( (bgm3 = Mix_LoadMUS("aud/bgm/real_grass.it")) == NULL)
	{
		fprintf(stderr, "Error loading background music 2. Check for aud/sfx/real_grass.it\n");
	}
	if ( (titleSong = Mix_LoadMUS("aud/bgm/title.ogg")) == NULL)
	{
		fprintf(stderr, "Error loading title screen music! Oh no! Check for aud/sfx/title.ogg\n");
	}

	return 0;
}

int setupAudio()
{
	// load support for the OGG sample/music formats
	int flags=MIX_INIT_OGG | MIX_INIT_MOD;
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
	
	if (loadBGMFiles() == 1)
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
	Mix_FreeChunk(mudSplat2);
	Mix_FreeChunk(swordStrike);
	Mix_FreeChunk(teleport);
	Mix_FreeChunk(princessYell);
	Mix_FreeChunk(crash);
	Mix_FreeChunk(wipe_open);
	Mix_FreeChunk(wipe_close);
	Mix_FreeChunk(enemy_die);
	Mix_FreeChunk(enemy_hurt);
	Mix_FreeChunk(get_hammer);
	Mix_FreeChunk(readyJingle);
	Mix_FreeChunk(losePrincess);
	
	Mix_FreeMusic(bgm1);
	Mix_FreeMusic(bgm2);
	Mix_FreeMusic(bgm3);
	Mix_FreeMusic(titleSong);

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
	if (fx == SFX_MUD2)
	{
		Mix_PlayChannel(-1, mudSplat2, 0);
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
	if (fx == SFX_CRASH)
	{
		Mix_PlayChannel(-1, crash, 0);
	}
	if (fx == SFX_WIPE_OPEN)
	{
		Mix_PlayChannel(-1, wipe_open, 0);
	}
	if (fx == SFX_WIPE_CLOSE)
	{
		Mix_PlayChannel(-1, wipe_close, 0);
	}
	if (fx == SFX_ENEMY_DIE)
	{
		Mix_PlayChannel(-1, enemy_die, 0);
	}
	if (fx == SFX_ENEMY_HURT)
	{
		Mix_PlayChannel(-1, enemy_hurt, 0);
	}
	if (fx == SFX_GET_HAMMER)
	{
		Mix_PlayChannel(-1, get_hammer, 0);
	}
	if (fx == SFX_READY_JINGLE)
	{
		Mix_PlayChannel(-1, readyJingle, 0);
	}
	if (fx == SFX_LOSE_PRINCESS)
	{
		Mix_PlayChannel(-1, losePrincess, 0);
	}
}

void playBGM(BGMType bg)
{
	Mix_HaltMusic();

	switch (bg)
	{
		case BGM_1:
		Mix_PlayMusic(bgm1, -1);
		break;
		case BGM_2:
		Mix_PlayMusic(bgm2, -1);
		break;
		case BGM_3:
		Mix_PlayMusic(bgm3, -1);
		break;
		case BGM_TITLE:
		Mix_PlayMusic(titleSong, 0);
		break;
		default:
		break;
	}
}

void stopBGM()
{
	Mix_HaltMusic();
}

void fadeBGM()
{
	Mix_FadeOutMusic(2000);
}

