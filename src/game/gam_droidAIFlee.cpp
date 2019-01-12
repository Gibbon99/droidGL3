#include "hdr/game/gam_droidAIFlee.h"

//#define AI_FLEE_DEBUG 1

//------------------------------------------
//
// Droid will flee when it has low health, and there is no healing tile on the level.
//
// Flee direction is based on it's position to the left/right of the player worldpos.
//
// Get the left/right extreme tile from level, and look along the Y for a passable tile.
//
// Set aStar destination to that location
//
//---------------------------------------------

//-----------------------------------------------------------------------------
//
// Droid checks his health status and number of healing tiles
int ai_isDroidHealthyFlee ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	//
	// Put in some tests like - percentage of max and current
	//
	// Maybe Battle droids fight until lower health level than non combatands
	//
	int badHealthLevel = 0;

	badHealthLevel = dataBaseEntry[levelInfo.at ( levelName ).droid[whichDroid].droidType].maxHealth * badHealthFactor;

	if ((levelInfo.at ( levelName ).droid[whichDroid].currentHealth < badHealthLevel) &&
	    (levelInfo.at ( levelName ).healing.empty ()) && (levelInfo.at ( levelName ).numWaypoints > 6))
	{
		return AI_RESULT_FAILED;
	}
	else
	{
		return AI_RESULT_SUCCESS;
	}
}

//-----------------------------------------------------------------------------
// Reset flee flags for when found destination tile or changing level
void ai_resetFleeFlags ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	levelInfo.at ( levelName ).droid[whichDroid].isNotPatrolling = true;
	levelInfo.at ( levelName ).droid[whichDroid].onResumeDestTile = false;
	levelInfo.at ( levelName ).droid[whichDroid].destSet = false;
	levelInfo.at ( levelName ).droid[whichDroid].aStarInitDone = false;
	levelInfo.at ( levelName ).droid[whichDroid].onFleeTile = false;
	levelInfo.at ( levelName ).droid[whichDroid].foundFleeTile = false;
	levelInfo.at ( levelName ).droid[whichDroid].aStarDestinationFound = false;
}

//-----------------------------------------------------------------------------
//
// Has the droid found the flee tile
int ai_onFleeTile ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	//
	// If droid over tile ??
	//
	if ( levelInfo.at ( levelName ).droid[whichDroid].onFleeTile )
	{
#ifdef AI_FLEE_DEBUG
		con_print ( true, false, "Droid [ %i ] found Flee tile destination", whichDroid );
#endif
		levelInfo.at ( levelName ).droid[whichDroid].currentHealth += 5;
		if ( levelInfo.at ( levelName ).droid[whichDroid].currentHealth >
		     dataBaseEntry[levelInfo.at ( levelName ).droid[whichDroid].droidType].maxHealth )
			levelInfo.at ( levelName ).droid[whichDroid].currentHealth = dataBaseEntry[levelInfo.at ( levelName ).droid[whichDroid].droidType].maxHealth;

		//
		// Find way back to origin
		//
		ai_resetFleeFlags ( whichDroid, levelName );
		return AI_RESULT_RUNNING;
	}
	else
	{
		return AI_RESULT_FAILED;
	}
}


//-----------------------------------------------------------------------------
//
// Find the nearest flee tile
int ai_reachedFleeTile ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	cpVect fleeTileLocation;
	cpVect droidWorldPosTiles;

	if ( !levelInfo.at ( levelName ).droid[whichDroid].foundFleeTile )
	{
		if ( !levelInfo.at ( levelName ).droid[whichDroid].aStarInitDone )
		{
			debugAStarIndex = whichDroid;

			levelInfo.at ( levelName ).droid[whichDroid].aStarInitDone = true;    // Reset as well
			fleeTileLocation = ai_findNearestTile ( whichDroid, TILE_TYPE_FLEE, levelName );

#ifdef AI_FLEE_DEBUG
			con_print ( true, true, "Found flee tile World [ %3.2f %3.2f ]", fleeTileLocation.x, fleeTileLocation.y );
#endif

			fleeTileLocation.x = (int) fleeTileLocation.x / TILE_SIZE;
			fleeTileLocation.y = (int) fleeTileLocation.y / TILE_SIZE;

#ifdef AI_FLEE_DEBUG
			con_print ( true, true, "Found flee tile Tile [ %3.2f %3.2f ]", fleeTileLocation.x, fleeTileLocation.y );
					con_print ( true, false, "[ %i ] - Starting AStar to find nearest flee tile", whichDroid );
#endif
			droidWorldPosTiles.x = (int) levelInfo.at ( levelName ).droid[whichDroid].worldPos.x / TILE_SIZE;
			droidWorldPosTiles.y =
					((int) levelInfo.at ( levelName ).droid[whichDroid].worldPos.y + (SPRITE_SIZE / 2)) / TILE_SIZE;

			levelInfo.at ( levelName ).droid[whichDroid].aStarPathIndex = gam_AStarRequestNewPath ( droidWorldPosTiles, fleeTileLocation, whichDroid, levelName );
			if ( levelInfo.at ( levelName ).droid[whichDroid].aStarPathIndex < 0 )
			{
				con_print ( true, false, "Error: Couldn't get a valid path index," );
				return AI_RESULT_FAILED;    //TODO: Check this is the right code to return
			}
		}

		//
		// Create the aStar until we reach the flee tile
		//
		// Ask if the thread has finished creating the path and waypoints
		if ( gam_AStarIsPathReady ( levelInfo.at ( levelName ).droid[whichDroid].aStarPathIndex ))
		{
#ifdef AI_FLEE_DEBUG
			con_print ( true, false, "[ %i ] - Have not found the AStar destination", whichDroid );
#endif
			levelInfo.at ( levelName ).droid[whichDroid].foundFleeTile = true;
			levelInfo.at ( levelName ).droid[whichDroid].currentAStarIndex = gam_AStarGetNumWaypoints ( levelInfo.at ( levelName ).droid[whichDroid].aStarPathIndex );

			//
			// Put pointer at the start of the list
			levelInfo.at ( levelName ).droid[whichDroid].currentAStarIndex++;

			levelInfo.at ( levelName ).droid[whichDroid].ai_moveMode = AI_MODE_ASTAR;
			ai_getNextWaypoint ( whichDroid, levelName );

			return AI_RESULT_RUNNING;
		}
		return AI_RESULT_FAILED; // Now start moving
	}

//
// This is run each time until the droid reaches the flee tile
//
	if ( levelInfo.at ( levelName ).droid[whichDroid].ai_moveMode == AI_PATHFIND_END )
	{
#ifdef AI_FLEE_DEBUG
		con_print ( true, true, "[ %i ] - Droid has reached flee tile.", whichDroid );
			if (debugAStarIndex == whichDroid)
				debugAStarIndex = -1;
#endif
		return AI_RESULT_SUCCESS;
	}
	else
	{
		if ( AI_RESULT_SUCCESS == ai_moveDroidToWaypoint ( whichDroid, levelName ))
		{
			levelInfo.at ( levelName ).droid[whichDroid].ai_moveMode = AI_MODE_ASTAR;
			if ( AI_RESULT_FAILED == ai_getNextWaypoint ( whichDroid, levelName ))
			{
				levelInfo.at ( levelName ).droid[whichDroid].ai_moveMode = AI_PATHFIND_END;
				// Reset ready for next time
				levelInfo.at ( levelName ).droid[whichDroid].onFleeTile = true;
				levelInfo.at ( levelName ).droid[whichDroid].foundFleeTile = false;
				levelInfo.at ( levelName ).droid[whichDroid].aStarInitDone = false;
				gam_AStarRemovePath ( levelInfo.at ( levelName ).droid[whichDroid].aStarPathIndex );
			}
		}
		return AI_RESULT_RUNNING;
	}
	return AI_RESULT_RUNNING;
}
