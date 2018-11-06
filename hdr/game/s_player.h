#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_droids.h"

extern _droid      playerDroid;
extern Uint32      playerAnimateInterval;      // Work out from current health

// Get the tile type the player is currently over
void s_getTileUnderPlayer(string levelName, float posX, float posY);

void s_renderPlayerSprite ();

// Set the state of the player animation timer
void gam_setPlayerAnimateState ( bool newState );

// Initiate the timer to animate the player sprite
//
// Pass in time in milliseconds
void gam_initPlayerAnimateTimer ( Uint32 interval );