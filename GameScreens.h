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
 
/* preambleSpashScreen()
 * Purpose: Displays the publisher and splash screens for around _____ seconds
 * Returns: Zero normally, 1 if the (X) button on the window was pressed
 */
int preambleSplashSreen(SDL_Surface* screen);


#endif

