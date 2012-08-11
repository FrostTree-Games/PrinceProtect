/*
 * HighScore.h
 * --------
 * This header file contains definitions for high score functionality.
 * Methods can be used here to save/load the high scores from disk.
 *
 * High scores are to be always saved in pp.hisc in the local directory.
 */

#ifndef SCORE_H
#define SCORE_H

#define NUMBER_OF_HIGH_SCORE_SLOTS 3

// this structure is used for passing highscore data; it's size is always 8 bytes(!)
typedef struct
{
	char playerCount; //1 or 2
	char name[3]; //3 characters used for the player initals (eg: AAA, DAN, KIM, etc.)
	int score;
} HighScore;

/* getHighScoreList()
 * Purpose: Fills an empty list with high scores.
 * Returns: n/a
 */
void getHighScoreList(HighScore* scoreList);

/* isHighScore()
 * Purpose: Determines if a score is high or not
 * Returns: 0 if false, 1 if true
 */
int isHighScore(int score);

/* pushHighScore()
 * Purpose: Pushes a high score to the high scores list, provided it's high enough.
 * Returns: n/a
 */
void pushHighScore(HighScore h);

/* loadHighScores()
 * Purpose: Loads the high scores from disk onto RAM. If there are no high scores found on disk, then a default list is created.
 * Returns: 0 on failure, 1 on success
 */
int loadHighScores();
 
/* saveHighScores()
 * Purpose: Saves the high scores onto disk.
 * Returns: 0 on failure, 1 on success
 */
int saveHighScores();

#endif