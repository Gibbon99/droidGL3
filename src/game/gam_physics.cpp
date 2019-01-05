#include <hdr/game/gam_player.h>
#include <hdr/game/gam_physicsCollisions.h>
#include <hdr/game/gam_database.h>
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_physicsPlayer.h"

cpSpace                     *space = nullptr;
cpVect                      shipGravity;
float                       shipDamping;            // Set from startup script
float                       collisionSlop;          // Set from startup script
float                       wallFriction;           // Set from startup script
float                       wallRadius;             // Set from startup script
float                       gravity;                // Set from startup script

vector<_physicObject>       solidWalls;


//-------------------------------------------------------------------
//
// Create the physics bodies and shapes for the enemy droids
void sys_createEnemyPhysics(const string levelName)
//-------------------------------------------------------------------
{
	int weight;
	float massWeight;

	for ( intptr_t i = 0; i != levelInfo.at (levelName).numDroids; i++ )
	{
		if ( true == levelInfo.at (levelName).droid[i].isAlive )
		{
			weight = static_cast<int>(strtol ( dataBaseEntry[levelInfo.at( levelName).droid[i].droidType].weight.c_str(), nullptr, 10 ));
			massWeight = weight / 100.0f;
			massWeight += playerMass;

			levelInfo.at (levelName).droid[i].body = cpSpaceAddBody ( space, cpBodyNew ( massWeight,  cpMomentForCircle ( massWeight, 0.0f, playerRadius, cpvzero ) ) );

			cpBodySetMass ( levelInfo.at (levelName).droid[i].body, massWeight );

			levelInfo.at (levelName).droid[i].shape = cpSpaceAddShape ( space, cpCircleShapeNew ( levelInfo.at (levelName).droid[i].body, playerRadius, cpvzero ) );
			cpShapeSetFriction ( levelInfo.at (levelName).droid[i].shape, playerFriction );
			cpShapeSetElasticity ( levelInfo.at (levelName).droid[i].shape, playerElastic );

			cpShapeSetCollisionType ( levelInfo.at (levelName).droid[i].shape, PHYSIC_TYPE_ENEMY );

			cpBodySetPosition ( levelInfo.at (levelName).droid[i].body, levelInfo.at (levelName).droid[i].worldPos );

			printf ("worldpos [ %li ] [ %3.3f %3.3f ]\n", i, levelInfo.at (levelName).droid[i].worldPos.x, levelInfo.at (levelName).droid[i].worldPos.y);

			cpShapeSetUserData ( levelInfo.at (levelName).droid[i].shape, (void *)i );	// Passed into collision routine
		}
	}
}


//-------------------------------------------------------------------
//
// Destroy all the bodies and shapes for this level
void sys_destroyPhysicObjects ( const string whichLevel )
//-------------------------------------------------------------------
{
	if (solidWalls.empty())
		return;

	for (auto wallItr : solidWalls)
	{
		if (cpTrue == cpSpaceContainsShape ( space, wallItr.shape ) )
		{
			cpSpaceRemoveShape ( space, wallItr.shape );
			cpShapeFree ( wallItr.shape );
		}

		if (cpTrue == cpSpaceContainsBody ( space, wallItr.body ) )
		{
			cpSpaceRemoveBody ( space, wallItr.body );
			cpBodyFree ( wallItr.body );
		}
	}
	solidWalls.clear();
	cpSpaceReindexStatic (space);

	/*
	for ( int i = 0; i != shipLevel[whichLevel].numDroids; i++ )
	{
		if ( true == shipLevel[whichLevel].droid[i].isAlive )
		{
			sys_removeDroidCPInfo ( i );
		}
	}

	for ( int i = 0; i != numDoorsOnLevel; i++ )
	{
		if (cpTrue == cpSpaceContainsShape ( space, doorTrigger[i].physics.shape ) )
		{
			cpSpaceRemoveShape ( space, doorTrigger[i].physics.shape );
			cpShapeFree ( doorTrigger[i].physics.shape );
		}

		if ( cpTrue == cpSpaceContainsBody ( space, doorTrigger[i].physics.body ) )
		{
			cpSpaceRemoveBody ( space, doorTrigger[i].physics.body );
			cpBodyFree ( doorTrigger[i].physics.body );
		}
	}
	 */
}


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
	cpVect          wallStart, wallFinish;
	_physicObject   tempWall;
	int             indexCount = 0;

	if ( 0 == levelInfo.at(levelName).numLineSegments )
		return;

	solidWalls.clear();
	solidWalls.reserve ( static_cast<unsigned long>(levelInfo.at ( levelName).numLineSegments));

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

		cpShapeSetFilter(solidWalls[i].shape, testShapeFilter);

		cpShapeSetUserData (solidWalls[i].shape, (cpDataPointer) i);
	}
//	sys_createEnemyPhysics ();

	cpSpaceReindexStatic (space);
}

