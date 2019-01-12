#pragma once

#include "hdr/game/gam_database.h"
#include "hdr/game/gam_droidAI.h"
#include "hdr/game/gam_pathFind.h"
#include "hdr/game/gam_levels.h"
#include "hdr/system/sys_main.h"


//---------------------------------------------------------
//
// AI Routines for health check
//
//---------------------------------------------------------

// Droid checks his health status
int ai_isDroidHealthy ( int whichDroid, std::string levelName );

// Find the closet healing tile to the current position
int ai_findNearestHealingTile ( int whichDroid, std::string levelName );

int ai_reachedHealingTile ( int whichDroid, std::string levelName );

// Is the droid currently located on a healing tile
int ai_onHealingTile ( int whichDroid, std::string levelName );

// Reset health flags for when found healing tile or changing level
void ai_resetHealthFlags ( int whichDroid, std::string levelName );
