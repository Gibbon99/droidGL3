#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/gam_droids.h"

extern  _droid      playerDroid;

// Get the tile type the player is currently over
int gam_getTileUnderPlayer ( string levelName, float posX, float posY );

void gam_renderPlayerSprite ();

// Set the state of the player animation timer
void gam_setPlayerAnimateState ( bool newState );

// Initiate the timer to animate the player sprite
//
// Pass in time in milliseconds
void gam_initPlayerAnimateTimer ( Uint32 interval );

// Setup initial values for player
void gam_initialPlayerSetup ();

// Process the player world position and movement
void gam_processPlayerMovement ();