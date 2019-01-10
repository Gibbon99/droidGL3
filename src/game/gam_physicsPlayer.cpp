#include <hdr/game/gam_physicsCollisions.h>
#include <bitset>
#include <hdr/system/sys_maths.h>
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_physicsPlayer.h"

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
	if ( newMass == playerDroid.mass )
		return;

	playerDroid.mass = newMass;

	if ( playerPhysicsObject.body == nullptr )
		return;

	cpBodySetMass (playerPhysicsObject.body, playerDroid.mass);
}

//-------------------------------------------------------------------
//
// Change the physics shape filter for the player on level change
// TODO: Need to have one for each networked client on this level
void sys_changePlayerPhysicsFilter()
//-------------------------------------------------------------------
{
	cpShapeFilter           playerShapeFilter;
	std::bitset<32>         playerBitset;

	playerBitset.reset();
	playerBitset = levelInfo.at ( lvl_getCurrentLevelName () ).deckCategory;        // Set category to this current level
	playerShapeFilter.categories = static_cast<cpBitmask>(playerBitset.to_ulong());

	playerBitset.reset();

	playerBitset = levelInfo.at ( lvl_getCurrentLevelName () ).deckCategory;        // Collide with everything in this category ( droids, walls )

	playerShapeFilter.mask = static_cast<cpBitmask>(playerBitset.to_ulong());
	playerShapeFilter.group = CP_NO_GROUP;

	cpShapeSetFilter(playerPhysicsObject.shape, playerShapeFilter);
}

//-------------------------------------------------------------------
//
// Setup player droid physics information
void sys_setupPlayerPhysics ()
//-------------------------------------------------------------------
{
	cpVect                  playerOffset;
	int                     packedValue;

	playerOffset = {0.0f, 0.0f};

	if ( playerPhysicsObject.body != nullptr )
		return;

	playerPhysicsObject.body = cpSpaceAddBody (space, cpBodyNew (playerDroid.mass, cpMomentForCircle (playerDroid.mass, 0.0f, playerRadius, playerOffset)));
	cpBodySetMass (playerPhysicsObject.body, playerDroid.mass);

	playerPhysicsObject.shape = cpSpaceAddShape (space, cpCircleShapeNew (playerPhysicsObject.body, playerRadius, playerOffset));
	cpShapeSetFriction (playerPhysicsObject.shape, playerFriction);
	cpShapeSetElasticity (playerPhysicsObject.shape, playerElastic);
	cpShapeSetCollisionType (playerPhysicsObject.shape, PHYSIC_TYPE_PLAYER);

	packedValue = sys_pack4Bytes( static_cast<char>(lvl_getDeckNumber ( lvl_getCurrentLevelName () )), 0, 1, 0);       // TODO check this isn't causing a crash in collision detection
	// invalid level
	cpShapeSetUserData (playerPhysicsObject.shape, (cpDataPointer) packedValue);    // Passed into collision routine
}

//-------------------------------------------------------------------
//
// Disable player collision with everything - resets on level change
void sys_disablePlayerCollision()
//-------------------------------------------------------------------
{
	cpShapeFilter noCollideFilter;

	noCollideFilter = {CP_NO_GROUP, ~CP_ALL_CATEGORIES, ~CP_ALL_CATEGORIES};

	cpShapeSetFilter(playerPhysicsObject.shape, noCollideFilter);
};
