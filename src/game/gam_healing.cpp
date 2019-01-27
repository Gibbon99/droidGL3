#include <hdr/game/gam_pathFind.h>
#include "hdr/game/gam_healing.h"

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------
bool            doHealingAnimate = true;
SDL_TimerID     timerHealingAnimate;
Uint32          healingAnimateInterval;      // From script

// ----------------------------------------------------------------------------
//
// Animate healing tiles called from timer callback
// Does healing tiles on all levels
Uint32 gam_healingAnimateTimerCallback ( Uint32 interval, void *param )
// ----------------------------------------------------------------------------
{
	if ( !doHealingAnimate )
		return interval;

	for ( auto &levelItr : levelInfo )
	{
		if ( !levelItr.second.healing.empty ())
		{
			for ( auto &healingItr: levelItr.second.healing )
			{
				healingItr.currentFrame++;
				if ( healingItr.currentFrame > HEALING_TILE + 3 )
					healingItr.currentFrame = HEALING_TILE;

				levelItr.second.tiles[healingItr.pos] = healingItr.currentFrame;
			}
		}

        gam_AStarProcessPaths (levelItr.first );

	}
	return healingAnimateInterval;
}

// ----------------------------------------------------------------------------
//
// Set the state of the healing tile timer
void gam_setHealingState ( bool newState )
// ----------------------------------------------------------------------------
{
	doHealingAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the healing tiles
//
// Pass in time in milliseconds
void gam_initHealingAnimateTimer ( Uint32 interval )
// ----------------------------------------------------------------------------
{
	timerHealingAnimate = evt_registerTimer ( interval, gam_healingAnimateTimerCallback, "Healing tile animation" );
}

// ----------------------------------------------------------------------------
//
// Find out where on the level the healing tiles are
// Remember for this level for animation
void gam_findHealingTiles ( string levelName )
// ----------------------------------------------------------------------------
{
	int             index;
	int             currentTile;
	int             countX, countY;
	int             countHealing;
	_basicHealing   tempHealing{};

	CHECK_LEVEL_NAME

	for ( index = 0; index < levelInfo.at ( levelName ).levelDimensions.x * levelInfo.at ( levelName ).levelDimensions.y; index++ )
	{
		switch ( levelInfo.at ( levelName ).tiles[index] )
		{
			case HEALING_TILE:
			case HEALING_TILE + 1:
			case HEALING_TILE + 2:
			case HEALING_TILE + 3:
				tempHealing.pos = index;
				tempHealing.currentFrame = HEALING_TILE;
				tempHealing.frameDelay = 0.0f;
				levelInfo.at ( levelName ).healing.push_back ( tempHealing );
				break;

			default:
				break;
		}
	}

	countHealing = 0;
	countX = 0;
	countY = 0;
	for (index = 0; index < levelInfo.at(levelName).levelDimensions.x * levelInfo.at(levelName).levelDimensions.y; index++)
	{
		currentTile = levelInfo.at(levelName).tiles[ ( ( countY * ( levelInfo.at(levelName).levelDimensions.x ) ) + countX )];

		switch (currentTile )
		{
			case HEALING_TILE:
			case HEALING_TILE + 1:
			case HEALING_TILE + 2:
			case HEALING_TILE + 3:
				levelInfo.at(levelName).healing[countHealing].worldPos.x = countX * TILE_SIZE;
				levelInfo.at(levelName).healing[countHealing].worldPos.y = countY * TILE_SIZE;

				countHealing++;

				break;

			default:
				break;
		}

		countX++;

		if ( countX == levelInfo.at(levelName).levelDimensions.x )
		{
			countX = 0;
			countY++;
		}
	}
}


