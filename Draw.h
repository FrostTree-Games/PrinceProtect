/*
 * Draw.h
 * --------
 * This header file contains definitions for methods that get used for rendering.
 * This will likely need to be refactored and redesigned a few times.
 * As of originally writing this method I'm not entirely sure how to do it.
 */
 
#ifndef DRAW_H
#define DRAW_H

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#include <SDL/SDL.h>

/* setupAssets()
 * Purpose: Loads the necessary TTF fonts and images; do this on startup
 * Returns: 0 on success, 1 on failure
 */
int setupAssets();

/* clearAssets
 * Purpose: Frees fonts and images from memory; do this when closing the game
 * Returns: n/a
 */
void clearAssets();

/* drawGameOverScreen()
 * Purpose: Draw a game over screen to the buffer; menuoption is either 1 or 2, high score character is 0 to 2, array of 3 chars
 * Returns: n/a
 */
void drawGameOverScreen(SDL_Surface* buffer, int menuOption, int selectedHighScoreCharacter, char* characters);

/* drawKeyConfigScreen()
 * Purpose: Draws the key config screen onto a buffer
 * Returns: n/a
 */
void drawKeyConfigScreen(SDL_Surface* buffer, int menuPosition, int keyCheck);

/* drawDevScreen()
 * Purpose: Fills a buffer with black and writes either all black (-1), the publisher logo (devScreenNumber = 0), or the developer logo (devScreenNumber = 1) to it
 * Returns: n/a
 */
void drawDevScreen(SDL_Surface* buffer, int devScreenNumber);

/* drawToTitleWipe()
 * Purpose: draws a to-title screen wipe; also passes a float to indicate how wide the curtains are (0.0 = no curtains, 1.0 = fully closed curtains)
 * Returns: n/a
 */
void drawToTitleWipe(SDL_Surface* buffer, float curtainLength);

/* drawToExitWipe()
 * Purpose: draws a to-black screen wipe; also passes a float to indicate how wide the curtains are (0.0 = no curtains, 1.0 = fully closed curtains); draws over any current bitmap on the screen
 * Returns: n/a
 */
void drawToExitWipe(SDL_Surface* buffer, float curtainLength);

/* drawWideScreenWalls()
 * Purpose: draws widescreen bars
 * Returns: n/a
 */
void drawWideScreenWalls(SDL_Surface* buffer);

/* drawGetReadyScreen()
 * Purpose: draws the pep talk screen; woot!
 * Returns: n/a
 */
void drawGetReadyScreen(SDL_Surface* buffer, int playerCount, int seed);

/* drawTitleScreen()
 * Purpose: draws the title screen; also passes in selected menu item and time since screen started
 * Returns: n/a
 */
void drawTitleScreen(SDL_Surface* buffer, int mSelected, Uint32 delta);

/* testDrawFunction()
 * Purpose: Getting something out the door for now.
 * Returns: n/a
 */
void testDraw(SDL_Surface* buffer);

/* get/setFullScreen()
 * Purpose: Sets fullscreen variable; this only works at startup; full screen can not be changed on the fly
 * Returns: n/a
 */
void setFullScreen(int state);
int getFullScreen();

/* get/setWideScreen()
 * Purpose: Sets widescreen variable; this only works at startup; widescreen can not be changed on the fly
 * Returns: n/a
 */
void setWideScreen(int state);
int getWideScreen();


#endif
