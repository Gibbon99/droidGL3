#include "hdr/game/gam_levels.h"
#include <hdr/system/sys_maths.h>
#include <hdr/game/gam_player.h>
#include "hdr/game/gam_lineOfSight.h"

float		g_LineOfSightStep;
bool		allDroidsVisible;

//-----------------------------------------------------
//
// See if point passed by X and Y is a solid tile
//
// Returns 1 if solid, 0 if passable
bool gam_LOSCalled ( int pass_x, int pass_y, const string levelName)
//-----------------------------------------------------
{
	int tile;

	pass_x = pass_x / TILE_SIZE;
	pass_y = pass_y / TILE_SIZE;

	tile = levelInfo.at ( levelName ).tiles[ ( pass_y * ( levelInfo.at ( levelName ).levelDimensions.x ) + pass_x )];

	if ( tile > NO_PASS_TILE )
		return false;		// not on a solid tile - can see through
	else
		return true;		// tile is solid - set flag to true
}

//-----------------------------------------------------
// Get passed x and y for player and on screen enemy
// draw a line until hit a solid tile or reach enemy.
//
// If solid is hit, set visible to 0 and exit rightaway
void lvl_doLineSight ( cpVect startPos, cpVect destPos, int whichDroid, const string levelName )
//-----------------------------------------------------
{
	float		distance;

	cpVect		vectDirection;
	cpVect		vectVector;
	cpVect		currentPosition;

	vectDirection = sys_getVectorDirection ( startPos, destPos );

	vectVector = cpvnormalize ( vectDirection );

	distance = sys_getDistance ( startPos, destPos );
	distance /= g_LineOfSightStep;

	currentPosition.x = startPos.x;
	currentPosition.y = startPos.y;

	for ( int i = 0; i != distance; i++ )
	{
		currentPosition.x += vectVector.x * g_LineOfSightStep;
		currentPosition.y += vectVector.y * g_LineOfSightStep;

		if ( gam_LOSCalled ( currentPosition.x, currentPosition.y, levelName ))
		{
			levelInfo.at ( levelName ).droid[whichDroid].visibleToPlayer = false;
			return; // Early out - hit a tile before reaching the droid - can't be seen
		}
	}
}

//-----------------------------------------------------
//
// Master routine to check visiblity of all valid sprites
void lvl_LOS(const string levelName)
//-----------------------------------------------------
{
	cpVect tempVect;

	int tempCount = 0;

	for ( tempCount = 0; tempCount != levelInfo.at ( levelName ).numDroids; tempCount++ )
	{
		if ( DROID_MODE_NORMAL == levelInfo.at ( levelName ).droid[tempCount].currentMode )
		{
			levelInfo.at ( levelName ).droid[tempCount].visibleToPlayer = true;
			//
			// Check if it is even visible on the screen
			if ( sys_visibleOnScreen ( levelInfo.at ( levelName ).droid[tempCount].worldPos, SPRITE_SIZE ))
			{
				tempVect = levelInfo.at ( levelName ).droid[tempCount].worldPos;

//				tempVect.y += ( TILE_SIZE / 2 );

				lvl_doLineSight ( playerDroid.middlePosition, tempVect, tempCount, levelName );

				if ( true == allDroidsVisible )
					levelInfo.at ( levelName ).droid[tempCount].visibleToPlayer = true;
			}
		}
	}
}

//-----------------------------------------------------
//
// Debug LOS lines
void lvl_debugLOS(const string levelName)
//-----------------------------------------------------
{
	cpVect screenPos;
	cpVect screenPlayerWorldPos;

	for ( int i = 0; i != levelInfo.at ( levelName ).numDroids; i++ )
	{
		screenPos = sys_worldToScreen ( levelInfo.at ( levelName ).droid[i].worldPos, DROID_BODY_SIZE );
		screenPlayerWorldPos = sys_worldToScreen ( playerDroid.middlePosition, DROID_BODY_SIZE );

//		al_draw_line ( screenPlayerWorldPos.x, screenPlayerWorldPos.y, screenPos.x + TILE_SIZE, screenPos.y + TILE_SIZE, al_map_rgb_f ( 1.0f, 1.0f, 1.0f ), 2 );
	}
}

