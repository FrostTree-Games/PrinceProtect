/*
 * Keyboard.h
 * --------
 * This header file contains definitions for methods used to encapsulate SDL's
 * keyboard functionality. Hopefully this will make things like keyboard settings
 * or even gamepad support easy.
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

// Each player is given at most, an NES controller's worth of buttons.
// This forces control simplicity, allows gamepad flexibility, and
// lets us cram 2 people onto a single keyboard.
typedef enum
{
	ESCAPE_KEY,
	P1_UP,
	P1_DOWN,
	P1_LEFT,
	P1_RIGHT,
	P1_A,
	P1_B,
	P1_START,
	P1_SELECT,
	P2_UP,
	P2_DOWN,
	P2_LEFT,
	P2_RIGHT,
	P2_A,
	P2_B,
	P2_START,
	P2_SELECT
} gameKey;

/* pollKeyboard()
 * Purpose: Updates the keyboard in memory. Should be called once every frame. Provides accurate gameKeys at that instant
 * Returns: n/a
 */
void pollKeyboard();

/* setKey()
 * Purpose: Sets a specific player key to a certain gameKey
 * Returns: n/a
 */
void setKey(SDLKey input, gameKey key);

/* getKey()
 * Purpose: Determines if a gameKey is pressed or not.
 * Returns: -1 if pollKeyboard has never been called; 1 if specified key is down; 0 otherwise
 */
int getKey(gameKey key);

/* isEnterKey()
 * Purpose: Checks if enter is down
 * Returns: 0 if no, 1 if yes
 */
int isEnterKey();

/* getSDLKeyValue()
 * Purpose: Determines what key is pressed corresponds to what gameKey
 * Returns: SDLKey of specified gameKey
 */
SDLKey getSDLKeyValue(gameKey key);

#endif