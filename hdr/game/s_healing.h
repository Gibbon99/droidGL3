#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------

struct _basicHealing
{
	int         pos;                // position in array
	int         currentFrame;       // which frame are we on
	float       frameDelay;         // animation counter
};

extern vector<_basicHealing>        healing; // hold information for animating healing tiles
extern Uint32                       healingAnimateInterval;

// Find out where on the level the healing tiles are
// Remember for this level for animation
void gam_findHealingTiles ( std::string levelName );

// Animate the healing tiles
void gam_animateHealingTiles();

// Initiate the timer to animate the healing tiles
//
// Pass in time in milliseconds
void gam_initHealingAnimateTimer ( Uint32 interval );

// Set the state of the healing tile timer
void gam_setHealingState ( bool newState );

