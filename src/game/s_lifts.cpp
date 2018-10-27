#include <hdr/game/s_levels.h>
#include "s_lifts.h"

//------------------------------------------------------------------------------
//
// Locate the world position of the passed in lift
cpVect s_getLiftworldPosition(string levelName, int whichLift)
//------------------------------------------------------------------------------
{
	cpVect liftPosition;

	liftPosition.x = levelInfo.at(levelName).lifts[whichLift].posX;
	liftPosition.y = levelInfo.at(levelName).lifts[whichLift].posY;

//	liftPosition.x += (winWidth / TILE_SIZE) / 2;
//	liftPosition.y += (winHeight / TILE_SIZE) / 2;

	return liftPosition;
}

//---------------------------------------------------------
//
// Get the positions of lifts
void lvl_getLiftPositions ( string levelName )
//---------------------------------------------------------
{
	int countX = 0;
	int countY = 0;
	int i;
	int currentTile = 0;
	int countLift = 0;

	countLift = 0;
	countX = 0;
	countY = 0;

	CHECK_LEVEL_NAME

	for ( i = 0; i < levelInfo.at(levelName).levelDimensions.x * levelInfo.at(levelName).levelDimensions.y; i++ )
	{
		currentTile = levelInfo.at(levelName).tiles[ ( ( countY * ( levelInfo.at(levelName).levelDimensions.x ) ) + countX )];

		if ( LIFT_TILE == currentTile )
		{
			levelInfo.at(levelName).lifts[countLift].posX = countX;
			levelInfo.at(levelName).lifts[countLift].posY = countY;
			countLift++;

			printf("Lift [ %i ] Pos [ %i %i ]\n", countLift, countX, countY);
		}

		countX++;

		if ( countX == levelInfo.at(levelName).levelDimensions.x )
		{
			countX = 0;
			countY++;
		}
	}
}

