#include <hdr/game/gam_player.h>
#include "hdr/game/gam_physics.h"

cpSpace                     *space = nullptr;
cpVect                      shipGravity;
float                       shipDamping;        // Set from startup script
float                       collisionSlop;        // Set from startup script
float                       wallFriction;        // Set from startup script
float                       wallRadius;            // Set from startup script
float                       gravity;            // Set from startup script

vector<_physicObject>       solidWalls;

//-------------------------------------------------------------------
//
// Setup Physics engine - run once
bool sys_setupPhysicsEngine ()
//--------------------------------------------------------------------
{
	shipGravity = cpv (0, 0);
	space = cpSpaceNew ();
	cpSpaceSetGravity (space, shipGravity);
	cpSpaceSetDamping (space, shipDamping);
	// Amount of overlap between shapes that is allowed.
	cpSpaceSetCollisionSlop (space, collisionSlop);

//	sys_setupPlayerPhysics ();

//	sys_setupCollisionHandlers ();

	return true;
}

//-------------------------------------------------------------------
//
// Cleanup Physics engine - run once
void sys_freePhysicsEngine ()
//--------------------------------------------------------------------
{
	if ( nullptr == space )
		return;

	cpSpaceFree (space);
}

//-------------------------------------------------------------------
//
// Create the solid walls for this level
void sys_createSolidWalls (string levelName)
//-------------------------------------------------------------------
{
	cpVect wallStart, wallFinish;
	_physicObject tempWall;
	int indexCount = 0;

	if ( 0 == levelInfo.at(levelName).numLineSegments )
		return;

	solidWalls.clear();
	solidWalls.reserve (levelInfo.at (levelName).numLineSegments);

	for ( intptr_t i = 0; i != levelInfo.at (levelName).numLineSegments; i++ )
	{
		wallStart = levelInfo.at (levelName).lineSegments[indexCount++];
		wallFinish = levelInfo.at (levelName).lineSegments[indexCount++];

		tempWall.body = cpBodyNewStatic ();
		tempWall.shape = cpSegmentShapeNew (tempWall.body, wallStart, wallFinish, wallRadius);
		solidWalls.push_back (tempWall);
		cpSpaceAddShape (space, solidWalls[i].shape);
		cpShapeSetFriction (solidWalls[i].shape, wallFriction);

		cpShapeSetCollisionType (solidWalls[i].shape, PHYSIC_TYPE_WALL);

		cpShapeSetUserData (solidWalls[i].shape, (cpDataPointer) i);
	}

//	sys_createEnemyPhysics ();

	cpSpaceReindexStatic (space);
}

