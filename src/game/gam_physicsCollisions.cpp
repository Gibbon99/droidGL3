#include <hdr/game/gam_physics.h>
#include <hdr/game/gam_player.h>
#include <hdr/game/gam_physicsPlayer.h>
#include "hdr/game/gam_physicsCollisions.h"


cpCollisionHandler *handlerWallPlayer;

bool useCollisionDetection = true;

cpShapeFilter   testShapeFilter = {WALL_GROUP,          PHYSIC_TYPE_WALL,   PHYSIC_TYPE_PLAYER};
cpShapeFilter   testShapeFilterPlayer = {PLAYER_GROUP,  PHYSIC_TYPE_PLAYER, PHYSIC_TYPE_WALL};

//-------------------------------------------------------------------
//
// Collision between player and wall
// Used to ignore collisions for debugging
//
bool handleCollisionPlayerWall ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
//	playerDroid.velocity = cpBodyGetVelocity (playerPhysicsObject.body );

	if ( !useCollisionDetection )
		return cpFalse;
	else
		return cpTrue;
}

//-------------------------------------------------------------------
//
// Setup all the collision handlers
void sys_setupCollisionHandlers ()
//-------------------------------------------------------------------
{
	//
	// Handle PLAYER hitting a WALL
	//
	handlerWallPlayer = cpSpaceAddCollisionHandler (space, PHYSIC_TYPE_PLAYER, PHYSIC_TYPE_WALL);
	handlerWallPlayer->beginFunc = (cpCollisionBeginFunc) handleCollisionPlayerWall;
}
