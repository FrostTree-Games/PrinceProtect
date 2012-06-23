/*
 * GameLogic.h
 * --------
 * This header represents the API and information for the game's logic. This includes things like,
 * scores, player health, modes, game state, wave information, etc.
 */
 
 #include <SDL/SDL.h>
 
/* xrand()
 * Purpose: Less-biased alternative to rand()
 * Returns: Random unsigned integer
 */
unsigned int xrand();
 
/* clearResetGame()
 * Purpose: Sets the game logic to a clean, uninitalized state
 * Returns: 0 on success, 1 on some sort of error
 */
int clearResetGame();

/* updateGameLogic()
 * Purpose: Performs any necessary game logic as needed
 * Returns: N/A
 */
void updateGameLogic();

/* getGameState()
 * Purpose: Returns the game state
 * Returns: 0 = no game; 1 = single player game; 2 = two player game
 */
int getGameState();

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
