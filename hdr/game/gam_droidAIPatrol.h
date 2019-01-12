#pragma once

#define WAYPOINT_UP         0x01
#define WAYPOINT_DOWN       0x02

#include "hdr/system/sys_main.h"

extern          float wayPointDestinationSize;      // From script

//--------------------------------------------------------------------
//
// Functions run by the aiTree - processing Droid actions
//
//--------------------------------------------------------------------
//
// Process all Droid movements
int ai_getNextWaypoint ( int whichDroid, std::string levelName );

// Can this droid make a move this turn
//
// Will fail if it is currently in a collision with either another droid or
// the player
int ai_canMove(int whichDroid, std::string levelName);

// This droid has collided - can it reverse direction and move
int ai_canReverseDirection(int whichDroid, std::string levelName);

int ai_moveDroidToWaypoint ( int whichDroid, const string levelName );


void ai_processDroidMovement ( std::string levelName );
