#pragma once

#define WAYPOINT_UP         0x01
#define WAYPOINT_DOWN       0x02

#include "hdr/system/sys_main.h"

// Set the new waypoint and destination for the passed in Droid
void ai_getNextWaypoint ( const string &levelName, int whichDroid );

void ai_processDroidMovement ( const string &levelName );