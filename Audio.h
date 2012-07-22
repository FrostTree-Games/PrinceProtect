/*
 * Audio.h
 * --------
 * This header file contains definitions for methods that get in audio-related
 * functionality. This consists of:
 *  1) Background music management
 *  2) Sound effect push functionality
 */
 
#ifndef AUDIO_H
#define AUDIO_H

// enum used to specify specific sound for passing into Audio
typedef enum
{
	BGM_NONE = 0,
	BGM_ANGSTY,
	BGM_ARTIFICAL,
	BGM_WEEDS,
	BGM_GRASS,
	BGM_TITLE
} BGMType;

typedef enum
{
	SFX_NONE = 0,
	SFX_EXPLOSION,
	SFX_CRASH,
	SFX_PLAYER_HURT,
	SFX_ICE_TINK,
	SFX_LASER_1,
	SFX_LASER_2,
	SFX_MENU,
	SFX_MUD,
	SFX_SWORD,
	SFX_TELEPORT,
	SFX_PRINCESS_YELL,
	SFX_PLAYER_DEATH,
	SFX_WIPE_OPEN,
	SFX_WIPE_CLOSE,
	SFX_ENEMY_DIE,
	SFX_ENEMY_HURT
} SFXType;

/* setupAudio()
 * Purpose: Loads the necessary OGG BGM music and WAV SFX; do this on startup
 * Returns: 0 on success, 1 on failure
 */
int setupAudio();

/* clearAudio()
 * Purpose: Frees SFX and BGM from memory; do this when closing the game
 * Returns: n/a
 */
void clearAudio();

/* playSFX()
 * Purpose: Plays a sound effect depending on its SFXType
 * Returns: n/a
 */
void playSFX(SFXType fx);

/* playBGM()
 * Purpose: Stops any BGM playing and starts a new one depending on the specified BGMType
 * Returns: n/a
 */
void playBGM(BGMType bg);

/* stopBGM()
 * Purpose: Stops any BGM playing
 */
void stopBGM();

#endif