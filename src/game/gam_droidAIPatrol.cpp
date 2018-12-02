#include <hdr/game/gam_levels.h>
#include <hdr/system/sys_maths.h>
#include "hdr/game/gam_droidAIPatrol.h"

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return None
//
// Set the new waypoint and destination for the passed in Droid
void ai_getNextWaypoint ( const string &levelName, int whichDroid )
//-----------------------------------------------------------------------------------------------------
{
	switch (levelInfo.at(levelName).droid[whichDroid].wayPointDirection)
	{
		case WAYPOINT_UP:
			if (levelInfo.at(levelName).droid[whichDroid].wayPointIndex < levelInfo.at(levelName).numWaypoints - 1)
				levelInfo.at(levelName).droid[whichDroid].wayPointIndex++;
			else
				levelInfo.at(levelName).droid[whichDroid].wayPointIndex = 0;
			break;

		case WAYPOINT_DOWN:
			levelInfo.at (levelName).droid[whichDroid].wayPointIndex--;

			if (levelInfo.at(levelName).droid[whichDroid].wayPointIndex < 0)
				levelInfo.at(levelName).droid[whichDroid].wayPointIndex = levelInfo.at(levelName).numWaypoints - 1;
		break;

		default:
			break;
	}
	levelInfo.at (levelName).droid[whichDroid].destinationCoords.x = levelInfo.at(levelName).wayPoints[levelInfo.at(levelName).droid[whichDroid].wayPointIndex].x;
	levelInfo.at (levelName).droid[whichDroid].destinationCoords.y = levelInfo.at(levelName).wayPoints[levelInfo.at(levelName).droid[whichDroid].wayPointIndex].y;
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
// Process the droids movements
void ai_processWaypointMove(const string &levelName, int whichDroid)
//-----------------------------------------------------------------------------------------------------
{
	cpFloat wayPointDestinationSize = 2.0f;

	levelInfo.at (levelName).droid[whichDroid].destDirection = sys_getDirection (levelInfo.at (levelName).droid[whichDroid].destinationCoords, levelInfo.at (levelName).droid[whichDroid].worldPos);
	levelInfo.at (levelName).droid[whichDroid].velocity = cpvnormalize (levelInfo.at (levelName).droid[whichDroid].destDirection);
	levelInfo.at (levelName).droid[whichDroid].velocity = cpvmult (levelInfo.at (levelName).droid[whichDroid].velocity, levelInfo.at (levelName).droid[whichDroid].currentSpeed);

	levelInfo.at (levelName).droid[whichDroid].worldPos = cpvadd (levelInfo.at (levelName).droid[whichDroid].worldPos, levelInfo.at (levelName).droid[whichDroid].velocity);

	//
	// See if the droid has reached it's destination
	if (cpvnear (levelInfo.at (levelName).droid[whichDroid].destinationCoords, levelInfo.at (levelName).droid[whichDroid].worldPos, wayPointDestinationSize))
	{
	//	levelInfo.at (levelName).droid[whichDroid].velocity = cpvzero;
	//	cpBodySetVelocity (levelInfo.at (levelName).droid[whichDroid].body, levelInfo.at (levelName).droid[whichDroid].velocity);
		ai_getNextWaypoint (levelName, whichDroid);
//		return AI_RESULT_SUCCESS;
	}
	else
	{
//		return AI_RESULT_RUNNING; // still hasn't made it to the destination
	}
}

//-----------------------------------------------------------------------------------------------------
//
/// \param argc
/// \param argv
/// \return
void ai_processDroidMovement(const string &levelName)
//-----------------------------------------------------------------------------------------------------
{
	RakNet::BitStream   BSOut{};

	if ( levelInfo.at (levelName).droid.empty ())
		return;     // Nothing to process

	BSOut.Write ((RakNet::MessageID)ID_GAME_MESSAGE_1);
	BSOut.Write (NET_DROID_WORLDPOS);

	for ( int index = 0; index != levelInfo.at (levelName).numDroids; index++ )
	{
		ai_processWaypointMove (levelName, index);

		BSOut.WriteVector (levelInfo.at (levelName).droid[index].worldPos.x, levelInfo.at (levelName).droid[index].worldPos.y, 0.0);
	}

	net_sendPacket ( &BSOut, NETWORK_SEND_DATA, 0 );
}
