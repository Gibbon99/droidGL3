#include "hdr/game/gam_player.h"
#include "hdr/game/gam_physicsPlayer.h"

float playerMass;            // Set from startup script
float playerRadius;        // Set from startup script
float playerFriction;        // Set from startup script
float playerElastic;        // Set from startup script

_physicObject playerPhysicsObject;

//-------------------------------------------------------------------
//
// Set the player physics position in the world
void sys_setPlayerPhysicsPosition ( cpVect newPosition )
//-------------------------------------------------------------------
{
	if ( playerPhysicsObject.body == nullptr )
		return;

	cpBodySetPosition (playerPhysicsObject.body, newPosition);
}

//-----------------------------------------------------------------------------
//
// Stop player movement
void sys_stopPlayerMovement ()
//-----------------------------------------------------------------------------
{
	playerDroid.velocity.x = 0.0f;
	playerDroid.velocity.y = 0.0f;

	if ( playerPhysicsObject.body != NULL )
		cpBodySetVelocity (playerPhysicsObject.body, playerDroid.velocity);
}

//-------------------------------------------------------------------
//
// Destroy player physics
void sys_destroyPlayerPhysics ()
//-------------------------------------------------------------------
{
#ifdef DEBUG_PHYSICS
	con_print ( true, false, "Removing player physics shape and body." );
#endif

	if ( cpTrue == cpSpaceContainsShape (space, playerPhysicsObject.shape))
	{
		cpSpaceRemoveShape (space, playerPhysicsObject.shape);
		cpShapeFree (playerPhysicsObject.shape);
		playerPhysicsObject.shape = nullptr;
	}

	if ( cpTrue == cpSpaceContainsBody (space, playerPhysicsObject.body))
	{
		cpSpaceRemoveBody (space, playerPhysicsObject.body);
		cpBodyFree (playerPhysicsObject.body);
		playerPhysicsObject.body = nullptr;
	}
}

//-------------------------------------------------------------------
//
// Set the player physics mass after transferring to a new droid
void sys_setPlayerMass ( float newMass )
//-------------------------------------------------------------------
{
	if ( newMass == playerMass )
		return;

	playerMass = newMass;

	cpBodySetMass (playerPhysicsObject.body, playerMass);
}

//-------------------------------------------------------------------
//
// Setup player droid physics information
void sys_setupPlayerPhysics ()
//-------------------------------------------------------------------
{
	cpVect playerOffset;

	playerOffset = {0.0f, 0.0f};

	if ( playerPhysicsObject.body != nullptr )
		return;

	playerPhysicsObject.body = cpSpaceAddBody (space, cpBodyNew (playerMass, cpMomentForCircle (playerMass, 0.0f, playerRadius, playerOffset)));
	cpBodySetMass (playerPhysicsObject.body, playerMass);

	playerPhysicsObject.shape = cpSpaceAddShape (space, cpCircleShapeNew (playerPhysicsObject.body, playerRadius, playerOffset));
	cpShapeSetFriction (playerPhysicsObject.shape, playerFriction);
	cpShapeSetElasticity (playerPhysicsObject.shape, playerElastic);
	cpShapeSetCollisionType (playerPhysicsObject.shape, PHYSIC_TYPE_PLAYER);
	cpShapeSetUserData (playerPhysicsObject.shape, (cpDataPointer) - 1);    // Passed into collision routine
}