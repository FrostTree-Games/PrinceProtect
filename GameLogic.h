/*
 * GameLogic.h
 * --------
 * This header represents the API and information for the game's logic. This includes things like,
 * scores, player health, modes, game state, wave information, etc.
 */
 
#ifndef DRAWLOGIC_H
#define DRAWLOGIC_H
 
#include <SDL/SDL.h>
 
/* xrand()
 * Purpose: Less-biased alternative to rand()
 * Returns: Random unsigned integer
 */
unsigned int xrand();
 
/* clearResetGame()
 * Purpose: Sets the game logic to a clean, uninitalized state; also specify number of players (1 or 2)
 * Returns: 0 on success, 1 on some sort of error
 */
int clearResetGame(int playerCount);

/* beginGame()
 * Purpose: Starts a clean, uninitalized game
 * Returns: 0 on success, 1 on some sort of error
 */
int beginGame();

/* updateGameLogic()
 * Purpose: Performs any necessary game logic as needed
 * Returns: N/A
 */
void updateGameLogic();

/* getGameState()
 * Purpose: Returns the game state
 * Returns: 0 -> no game running, 1 -> regular game play, 2 -> player health depleted, 3 -> ice cream depleted
 */
int getGameState();

/* getPlayerCount()
 * Purpose: Returns the number of players ingame
 * Returns: 0 -> no game running, 1 -> 1 player, 2 -> 2 players
 */
int getPlayerCount();

/* [various player health methods]
 * Purpose: APIs for modifying player health attributes (1 = player 1; 2 = player 2)
 * Returns: various
 */
int getPlayerHealth(int playerNo);
int getPlayerMaxHealth(int playerNo);
void modPlayerHealth(int playerNo, int delta);
void modPlayerMaxHealth(int playerNo, int delta);

/* getScore()
 * Purpose: Gets the current game score
 * Returns: integer representing score, -1 if no game is running
 */
int getScore();

/* addScore()
 * Purpose: Adds a bonus (or deduction) to the score
 * Returns: N/A
 */
void addScore(int delta);

/* getIceCreamCount()
 * Purpose: Gets the current number of ice creams in the game
 * Returns: integer representing ice cream
 */
int getIceCreamCount();


#endif
