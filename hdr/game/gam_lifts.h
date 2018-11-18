#pragma once

#include <unordered_map>
#include "hdr/system/sys_main.h"


//-----------------------------------------------------------------------------
//
// information needed for one lift
//
//-----------------------------------------------------------------------------

struct _liftBasic {
	int tunnel;
	int posX;
	int posY;
};

// Get the positions of lifts
void lvl_getLiftPositions ( string levelName );

// Locate the world position of the passed in lift
cpVect s_getLiftworldPosition(string levelName, int whichLift);