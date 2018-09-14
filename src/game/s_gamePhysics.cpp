#include "hdr/system/sys_main.h"
#include "hdr/libGL/chipmunk/chipmunk.h"

cpSpace *space = nullptr;
cpVect shipGravity;
float shipDamping;        // Set from startup script
float collisionSlop;        // Set from startup script

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