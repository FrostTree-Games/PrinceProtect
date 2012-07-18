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
#define SCREEN_HEIGHT 272

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
 * Purpose: Draw a game over screen to the buffer
 * Returns: n/a
 */
void drawGameOverScreen(SDL_Surface* buffer);

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

/* drawTitleScreen()
 * Purpose: draws the title screen
 * Returns: n/a
 */
void drawTitleScreen(SDL_Surface* buffer, int mSelected);

/* testDrawFunction()
 * Purpose: Getting something out the door for now.
 * Returns: n/a
 */
void testDraw(SDL_Surface* buffer);

#endif
