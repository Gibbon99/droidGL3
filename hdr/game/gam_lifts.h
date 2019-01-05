#pragma once

#include <unordered_map>
#include "hdr/system/sys_main.h"

extern int     currentDeckNumber;

// Get the positions of lifts
void gam_getLiftPositions ( const std::string &levelName );

// Locate the world position of the passed in lift
cpVect gam_getLiftworldPosition ( std::string levelName, int whichLift );

// Move the lift position
void gam_moveLift(int direction);

// Setup lifts
void gam_setupLifts ();
