/*
 * GameScreens.h
 * --------
 * This header file contains methods that compose the extra screens which make up the complete
 * visual interface of Whimsy Block Go. This includes:
 * - Loading/Load failed splash screen
 * - Preamble splash screen (publisher logo, developer logo)
 * - Title screen
 * - Keyconfig 
 * - Miscellanous Options
 * - Game over screen
 * - Random encouragement screen
 * - Any sort of filter or wipe that makes a transition between screens
 *
 * Each of these screens for now will be a self-contained function with its own while loop.
 * Every screen with have a return value.
 * Every screen is considered a blocking function.
 */
 
#ifndef SCREENS_H
#define SCREENS_H

#define SCREEN_TIMEOUT_SECONDS 20
 
/* keyConfigScreen()
 * Purpose: Displays the screen that lets the player check key info
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int keyConfigScreen(SDL_Surface* screen);

/* preambleSpashScreen()
 * Purpose: Displays the publisher and splash screens for around _____ seconds
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int preambleSplashScreen(SDL_Surface* screen);

/* titleTransitionScreen()
 * Purpose: Opens black curtains to the title screen
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int titleTransitionScreen(SDL_Surface* screen);

/* exitTransitionScreen()
 * Purpose: closes black curtains
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int exitTransitionScreen(SDL_Surface* screen);

/* getReadyScreen()
 * Purpose: peps the player up; pass in 1 or 2 for playerCount
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int getReadyScreen(SDL_Surface* screen, int playerCount);

/* titleScreen()
 * Purpose: Displays the title screen in a sexy loop. After SCREEN_TIMEOUT_SECONDS the game defines
 * Returns: integer representing menu result press
 *		0 -> EXIT
 *		1 -> 1P GAME
 *		2 -> 2P GAME
 *		3 -> KEY CONFIG
 *		4 -> SETTINGS
 *		returns -1 if (X) button on window was pressed
 */
int titleScreen(SDL_Surface* screen);


#endif

