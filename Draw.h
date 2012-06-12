/*
 * Draw.h
 * --------
 * This header file contains definitions for methods that get used for rendering.
 * This will likely need to be refactored and redesigned a few times.
 * As of originally writing this method I'm not entirely sure how to do it.
 */
 
#ifndef DRAW_H
#define DRAW_H

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 448

#include <SDL/SDL.h>

/* setupFonts()
 * Purpose: Loads the necessary TTF fonts; do this on startup
 * Returns: 0 on success, 1 on failure
 */
int setupFonts();

/* clearFonts
 * Purpose: Frees the necessary fonts from memory; do this on exit
 * Returns: n/a
 */
void clearFonts();

/* testDrawFunction()
 * Purpose: Getting something out the door for now.
 * Returns: n/a
 */
void testDraw(SDL_Surface* buffer);

#endif
