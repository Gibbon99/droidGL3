#include "hdr/game/gam_pathFind.h"
#include <hdr/game/gam_droidAI.h>
#include "hdr/game/gam_levels.h"
#include "hdr/system/sys_maths.h"
#include "hdr/game/gam_database.h"
#include "hdr/game/gam_droidAIPatrol.h"

float wayPointDestinationSize;      // From script


//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void ai_processDroidMovement ( const string levelName )
//-----------------------------------------------------------------------------------------------------
{
	RakNet::BitStream BSOut{};
	cpVect		tempPosition;
	cpVect		maxWorldSize;

	if ( levelInfo.at ( levelName ).droid.empty ())
		return;     // Nothing to process
	//
	// Send updates if this is the server
	if ( isServer )
	{
		BSOut.Write ((RakNet::MessageID) ID_GAME_MESSAGE_1 );
		BSOut.Write ( NET_DROID_WORLDPOS );
		BSOut.Write ( currentServerTick );
		BSOut.Write ( lvl_getDeckNumber ( levelName ));
	}

	for ( int index = 0; index != levelInfo.at ( levelName ).numDroids; index++ )
	{

        if ( levelInfo.at ( levelName ).droid[index].currentMode != DROID_MODE_DEAD)
          {

            maxWorldSize.x = levelInfo.at (levelName).levelDimensions.x * TILE_SIZE;
            maxWorldSize.y = levelInfo.at (levelName).levelDimensions.y * TILE_SIZE;

            //
            // Check body is valid
            //
            if (cpTrue == cpSpaceContainsBody (space, levelInfo.at (levelName).droid[index].body))
              {
                tempPosition = cpBodyGetPosition (levelInfo.at (levelName).droid[index].body);

                if ((tempPosition.x < 0) || (tempPosition.y < 0) || (tempPosition.x > maxWorldSize.x)
                    || (tempPosition.y > maxWorldSize.y))
                  {
                    printf ("ERROR: Setting invalid worldPos [ %3.3f %3.3f ] from body Droid [ %i ] Level [ %s ] Frame [ %i ]\n",
                            tempPosition.x, tempPosition.y, index, levelName.c_str (), static_cast<int>(frameCount));
                    return;
                  }

                levelInfo.at (levelName).droid[index].worldPos = tempPosition;
              }
            else
              {
                printf ("ERROR: Attempting to get position from invalid body - droid [ %i ]\n", index);
                return;
              }

            if (isServer)
              {
                BSOut.WriteVector (levelInfo.at (levelName).droid[index].worldPos.x, levelInfo.at (levelName).droid[index].worldPos.y, 0.0);

                BSOut.WriteVector (levelInfo.at (levelName).droid[index].velocity.x, levelInfo.at (levelName).droid[index].velocity.y, 0.0);
              }
          }
	}

	if ( isServer )
		net_sendPacket ( &BSOut, NETWORK_SEND_DATA, -1 );
}


//-----------------------------------------------------------------------------
//
// Process all Droid movements
int ai_getNextWaypoint ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	//
	// Get the next waypoint index pointer
	switch ( levelInfo.at (levelName).droid[whichDroid].ai_moveMode )
	{
		case AI_MODE_WAYPOINT:

			switch ( levelInfo.at (levelName).droid[whichDroid].wayPointDirection )
			{
				case WAYPOINT_UP:
					if ( levelInfo.at (levelName).droid[whichDroid].wayPointIndex < levelInfo.at (levelName).numWaypoints - 1 )
						levelInfo.at (levelName).droid[whichDroid].wayPointIndex++;
					else
						levelInfo.at (levelName).droid[whichDroid].wayPointIndex = 0;

					break;

				case WAYPOINT_DOWN:
					levelInfo.at (levelName).droid[whichDroid].wayPointIndex--;

					if ( levelInfo.at (levelName).droid[whichDroid].wayPointIndex < 0 )
						levelInfo.at (levelName).droid[whichDroid].wayPointIndex = levelInfo.at (levelName).numWaypoints - 1;

					break;
			}

			//
			// Save current destination in case we need to reverse
			//
			levelInfo.at (levelName).droid[whichDroid].previousWaypoints = levelInfo.at (levelName).droid[whichDroid].destinationCoords;
			//
			// Get new destination to work out direction vector
			//
			levelInfo.at (levelName).droid[whichDroid].destinationCoords.x = levelInfo.at (levelName).wayPoints[levelInfo.at (levelName).droid[whichDroid].wayPointIndex].x;
			levelInfo.at (levelName).droid[whichDroid].destinationCoords.y = levelInfo.at (levelName).wayPoints[levelInfo.at (levelName).droid[whichDroid].wayPointIndex].y;

			break;

		case AI_MODE_ASTAR:
			levelInfo.at (levelName).droid[whichDroid].currentAStarIndex--; // Move index to next waypoint

			if ( levelInfo.at (levelName).droid[whichDroid].currentAStarIndex < 0 )
			{
//					con_print ( true, true, "Droid has reached destination" );
				levelInfo.at (levelName).droid[whichDroid].ai_moveMode = AI_PATHFIND_END;
				return AI_RESULT_FAILED; // No more waypoints
			}

			//
			// Get new destination to work out direction vector
			//
			levelInfo.at (levelName).droid[whichDroid].destinationCoords.x = path[levelInfo.at (levelName).droid[whichDroid].aStarPathIndex].wayPoints[levelInfo.at (levelName).droid[whichDroid].currentAStarIndex].x;
			levelInfo.at (levelName).droid[whichDroid].destinationCoords.y = path[levelInfo.at (levelName).droid[whichDroid].aStarPathIndex].wayPoints[levelInfo.at (levelName).droid[whichDroid].currentAStarIndex].y;
			break;
	}

	return AI_RESULT_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// Can this droid make a move this turn
//
// Will fail if it is currently in a collision with either another droid or
// the player
// TODO: Add in player collision
int ai_canMove ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	//
	// Droid can always move as it's ignoring any collisions with other droids
	//
	if ( levelInfo.at ( levelName).droid[whichDroid].ignoreCollisions )
	{
		return AI_RESULT_SUCCESS;
	}

	if ( levelInfo.at ( levelName).droid[whichDroid].hasCollided )
	{
		return AI_RESULT_FAILED;
	}
	else
	{
		return AI_RESULT_SUCCESS;
	}

	return AI_RESULT_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// Process movements for the passed in droid
// Work out heading vector and velocity
//
// Return AI_RESULT_SUCCESS when we have reached the destination
//
int ai_moveDroidToWaypoint ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	float wayPointDistance;

	float thinkInterval = 1.0f / 30.f;

	wayPointDistance = cpvdist ( levelInfo.at (levelName).droid[whichDroid].destinationCoords, levelInfo.at (levelName).droid[whichDroid].worldPos );

	if ( wayPointDistance < wayPointDestinationSize + 20 )
	{
		levelInfo.at (levelName).droid[whichDroid].currentSpeed = dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].maxSpeed * 0.5f;
	}
	else
	{
		// Acclerate faster on higher alert levels
		switch (currentAlertLevel)
		{
			case ALERT_GREEN_TILE:
				levelInfo.at (levelName).droid[whichDroid].currentSpeed += dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].accelerate; // * thinkInterval;
				break;

			case ALERT_YELLOW_TILE:
				levelInfo.at (levelName).droid[whichDroid].currentSpeed += (dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].accelerate * 1.2f); //  * thinkInterval;
				break;

			case ALERT_RED_TILE:
				levelInfo.at (levelName).droid[whichDroid].currentSpeed += (dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].accelerate * 1.5f); //  * thinkInterval;
				break;
		}

		if ( levelInfo.at (levelName).droid[whichDroid].currentSpeed > dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].maxSpeed )
			levelInfo.at (levelName).droid[whichDroid].currentSpeed = dataBaseEntry[levelInfo.at (levelName).droid[whichDroid].droidType].maxSpeed;
	}


	levelInfo.at (levelName).droid[whichDroid].currentSpeed = 0.3f;



	levelInfo.at (levelName).droid[whichDroid].destDirection = sys_getDirection ( levelInfo.at (levelName).droid[whichDroid].destinationCoords, levelInfo.at (levelName).droid[whichDroid].worldPos );

	levelInfo.at (levelName).droid[whichDroid].velocity = cpvnormalize ( levelInfo.at (levelName).droid[whichDroid].destDirection );
	levelInfo.at (levelName).droid[whichDroid].velocity = cpvmult ( levelInfo.at (levelName).droid[whichDroid].velocity, levelInfo.at (levelName).droid[whichDroid].currentSpeed );
	//
	// Move the droid
	//
	if ( cpFalse == cpBodyIsSleeping ( levelInfo.at (levelName).droid[whichDroid].body ) )
	{
		if ( cpTrue == cpSpaceContainsBody ( space, levelInfo.at (levelName).droid[whichDroid].body ) )
		{
			cpBodySetForce ( levelInfo.at (levelName).droid[whichDroid].body, levelInfo.at (levelName).droid[whichDroid].velocity );
		}
	}
	//
	// See if the droid has reached it's destination
	//
	//
	// How far between current and destination position
	//
	// If it's less then we have reached the waypoint destination
	if ( wayPointDistance < wayPointDestinationSize )
	{
		levelInfo.at (levelName).droid[whichDroid].velocity = cpvzero;
		cpBodySetVelocity ( levelInfo.at (levelName).droid[whichDroid].body, levelInfo.at (levelName).droid[whichDroid].velocity );
		return AI_RESULT_SUCCESS;
	}
	else
	{
		return AI_RESULT_RUNNING; // still hasn't made it to the destination
	}
}

//-----------------------------------------------------------------------------
//
// This droid has collided - can it reverse direction and move
//
int ai_canReverseDirection ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
	if ( levelInfo.at (levelName).droid[whichDroid].droidType < levelInfo.at (levelName).droid[levelInfo.at (levelName).droid[whichDroid].collidedWith].droidType )
	{
		switch ( levelInfo.at (levelName).droid[whichDroid].wayPointDirection )
		{
			case WAYPOINT_DOWN:
				levelInfo.at (levelName).droid[whichDroid].wayPointDirection = WAYPOINT_UP;
				break;

			case WAYPOINT_UP:
				levelInfo.at (levelName).droid[whichDroid].wayPointDirection = WAYPOINT_DOWN;
				break;
		}

		ai_getNextWaypoint ( whichDroid, levelName );
	}

	return AI_RESULT_SUCCESS;
}
