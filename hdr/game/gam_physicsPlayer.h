#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/gam_physics.h"

extern _physicObject playerPhysicsObject;

extern float playerMass;            // Set from startup script
extern float playerRadius;        // Set from startup script
extern float playerFriction;        // Set from startup script
extern float playerElastic;        // Set from startup script

// Setup player droid physics information
void sys_setupPlayerPhysics ();

// Set the player physics mass after transferring to a new droid
void sys_setPlayerMass ( float newMass );

// Destroy player physics
void sys_destroyPlayerPhysics ();

// Set the player physics position in the world
void sys_setPlayerPhysicsPosition ( cpVect newPosition );

// Stop player movement
void sys_stopPlayerMovement ();