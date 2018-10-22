#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------

struct _basicHealing {
	int         numOnLevel;         // first record holds number of tiles in level
	int         pos;                // position in array
	int         numFrames;          // how many frames make up the animation
	int         currentFrame;       // which frame are we on
	float       frameDelay;         // speed to animate them at
	float       nextFrame;          // counter for incrementing to next frame
	cpVect      worldPosition;      // Used for aStar finding
};

extern vector<_basicHealing>        healing; // hold information for animating healing tiles

// Find out where on the level the healing tiles are
// Remember for this level for animation
void gam_findHealingTiles ( std::string levelName );

// Animate the healing tiles
void gam_animateHealingTiles();