#include <stdio.h>
#include <stdlib.h>

#include "HighScore.h"

HighScore scores[NUMBER_OF_HIGH_SCORE_SLOTS]; // index 2 is the lowest score on the list, 3 is the highest

void getHighScoreList(HighScore* scoreList)
{
	int i;
	
	for (i = 0; i < NUMBER_OF_HIGH_SCORE_SLOTS; i++)
	{
		scoreList[i] = scores[i];
	}
}

int isHighScore(int score)
{
	if (scores[NUMBER_OF_HIGH_SCORE_SLOTS - 1].score > score)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void pushHighScore(HighScore h)
{
	int i,j;

	if (!isHighScore(h.score))
	{
		return;
	}
	
	for (i = 0; i < NUMBER_OF_HIGH_SCORE_SLOTS; i++)
	{
		if (h.score > scores[i].score)
		{
			for (j = NUMBER_OF_HIGH_SCORE_SLOTS - 1; j > i; j--)
			{
				scores[j] = scores[j - 1];
			}
			
			scores[i] = h;
			break;
		}
	}
}

int loadHighScores()
{
	FILE* scoresFile;
	
	scoresFile = fopen("pp.hisc", "rb");
	
	if (scoresFile != NULL)
	{
		fread((void*)scores, sizeof(HighScore), 3, scoresFile);
		
		fclose(scoresFile);
		
		return 1;
	}
	else
	{
		scores[0].playerCount = 1;
		scores[0].name[0] = '-'; scores[0].name[1] = '-'; scores[0].name[2] = '-';
		scores[0].score = 0;

		scores[1].playerCount = 2;
		scores[1].name[0] = '-'; scores[1].name[1] = '-'; scores[1].name[2] = '-';
		scores[1].score = 0;

		scores[2].playerCount = 1;
		scores[2].name[0] = '-'; scores[2].name[1] = '-'; scores[2].name[2] = '-';
		scores[2].score = 0;
		
		return 0;
	}
}

int saveHighScores()
{
	int i;
	FILE* scoresFile;
	
	scoresFile = fopen("pp.hisc", "wb");
	
	if (scoresFile != NULL)
	{
		for (i = 0; i < NUMBER_OF_HIGH_SCORE_SLOTS; i++)
		{
			fwrite(&(scores[i]), sizeof(HighScore), 1, scoresFile);
		}
		
		fclose(scoresFile);
		
		return 1;
	}
	else
	{
		fprintf(stderr, "Unable to write scores file!");
		
		return 0;
	}
}
