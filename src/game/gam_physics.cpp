#include <hdr/game/gam_player.h>
#include <hdr/game/gam_physicsCollisions.h>
#include <hdr/game/gam_database.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/gam_droidAIPatrol.h>
#include <bitset>
#include <hdr/system/sys_maths.h>
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_physicsPlayer.h"

bool physicsStarted = false;
cpSpace *space = nullptr;
cpVect shipGravity;
float shipDamping;            // Set from startup script
float collisionSlop;          // Set from startup script
float wallFriction;           // Set from startup script
float wallRadius;             // Set from startup script
float gravity;                // Set from startup script


//---------------------------------------------------------------
//
// Update the droids information from physics properties
void drd_updateDroidPosition ( const string levelName, int whichDroid )
//---------------------------------------------------------------
{
	cpVect tempPosition;
	cpVect maxWorldSize;
	static int frameCount = 0;

	maxWorldSize.x = levelInfo.at ( levelName ).levelDimensions.x * TILE_SIZE;
	maxWorldSize.y = levelInfo.at ( levelName ).levelDimensions.y * TILE_SIZE;

	//
	// Check body is valid
	//
	if ( cpTrue == cpSpaceContainsBody ( space, levelInfo.at ( levelName ).droid[whichDroid].body ))
	{
		tempPosition = cpBodyGetPosition ( levelInfo.at ( levelName ).droid[whichDroid].body );

		if ((tempPosition.x < 0) || (tempPosition.y < 0) || (tempPosition.x > maxWorldSize.x) ||
		    (tempPosition.y > maxWorldSize.y))
		{
//			printf ( "ERROR: Setting invalid worldPos [ %3.3f %3.3f ] from body Droid [ %i ] Level [ %s ] Frame [ %i ]\n", tempPosition.x, tempPosition.y, whichDroid, levelName.c_str (), static_cast<int>(frameCount));
			return;
		}

		levelInfo.at ( levelName ).droid[whichDroid].worldPos = tempPosition;
	}
	else
	{
		printf ( "ERROR: Attempting to get position from invalid body - droid [ %i ]\n", whichDroid );
		return;
	}
}

//-------------------------------------------------------------------
//
// Create the physics bodies and shapes for the enemy droids
void sys_createEnemyPhysics ( const string levelName )
//-------------------------------------------------------------------
{
    std::bitset<32>         droidBitset;        // Use standard bitmasks
	cpShapeFilter           droidShapeFilter;
	int                     packedValue;

	if ( !physicsStarted )
	{
		con_print ( CON_ERROR, true, "Attempting to setup droid physics with no engine." );
		return;
	}

	if ( levelInfo.at ( levelName ).droidPhysicsCreated )
		return;

	for ( int i = 0; i != levelInfo.at ( levelName ).numDroids; i++ )
	{
		levelInfo.at ( levelName ).droid[i].body = cpSpaceAddBody ( space, cpBodyNew ( levelInfo.at ( levelName ).droid[i].mass, cpMomentForCircle ( levelInfo.at ( levelName ).droid[i].mass, 0.0f, playerRadius, cpvzero )));

		cpBodySetMass ( levelInfo.at ( levelName ).droid[i].body, levelInfo.at ( levelName ).droid[i].mass );

		levelInfo.at ( levelName ).droid[i].shape = cpSpaceAddShape ( space, cpCircleShapeNew ( levelInfo.at ( levelName ).droid[i].body, playerRadius, cpvzero ));
		cpShapeSetFriction ( levelInfo.at ( levelName ).droid[i].shape, playerFriction );
		cpShapeSetElasticity ( levelInfo.at ( levelName ).droid[i].shape, playerElastic );

		cpShapeSetCollisionType ( levelInfo.at (levelName).droid[i].shape, PHYSIC_TYPE_ENEMY );
		packedValue = sys_pack4Bytes( static_cast<char>(lvl_getDeckNumber ( levelName)), (char)i, 0, 0);
		cpShapeSetUserData ( levelInfo.at ( levelName ).droid[i].shape, (cpDataPointer) packedValue );    // Passed into collision routine

		cpBodySetPosition ( levelInfo.at ( levelName ).droid[i].body, levelInfo.at ( levelName ).droid[i].worldPos );
		//
		// Setup the bitmasks for collision filtering
		//

		droidBitset.reset();                    // Clear it
		droidBitset = levelInfo.at ( levelName ).deckCategory;  // Set category to this level
		droidShapeFilter.categories = static_cast<cpBitmask>(droidBitset.to_ulong());    // Set chipmunk cpBitmask

		droidBitset.reset();                    // Clear it again

		droidBitset = levelInfo.at ( levelName ).deckCategory;      // will collide with everything on this level ( this category )
		droidBitset[PHYSIC_TYPE_PLAYER] = true;                     // and the player
		droidShapeFilter.mask = static_cast<cpBitmask>(droidBitset.to_ulong());          // Set chipmunk cpBitmask

		droidShapeFilter.group = CP_NO_GROUP;                       // Doesn't belong to any group

		cpShapeSetFilter ( levelInfo.at ( levelName ).droid[i].shape, droidShapeFilter );   // Set the category and collision mask
	}

	levelInfo.at ( levelName ).droidPhysicsCreated = true;
}


//-------------------------------------------------------------------
//
// Destroy all the bodies and shapes for this level
void sys_destroyPhysicObjects ( const string whichLevel )
//-------------------------------------------------------------------
{
	if ( levelInfo.at ( whichLevel ).solidWalls.empty ())
		return;

	for ( auto wallItr : levelInfo.at ( whichLevel ).solidWalls )
	{
		if ( cpTrue == cpSpaceContainsShape ( space, wallItr.shape ))
		{
			cpSpaceRemoveShape ( space, wallItr.shape );
			cpShapeFree ( wallItr.shape );
		}

		if ( cpTrue == cpSpaceContainsBody ( space, wallItr.body ))
		{
			cpSpaceRemoveBody ( space, wallItr.body );
			cpBodyFree ( wallItr.body );
		}
	}
	levelInfo.at ( whichLevel ).solidWalls.clear ();
	cpSpaceReindexStatic ( space );

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
	shipGravity = cpv ( 0, 0 );
	space = cpSpaceNew ();
	cpSpaceSetGravity ( space, shipGravity );
	cpSpaceSetDamping ( space, shipDamping );
	// Amount of overlap between shapes that is allowed.
	cpSpaceSetCollisionSlop ( space, collisionSlop );

//	sys_setupPlayerPhysics ();

//	sys_setupCollisionHandlers ();

	physicsStarted = true;

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

	cpSpaceFree ( space );
}

//-------------------------------------------------------------------
//
// Create the solid walls for this level
void sys_createSolidWalls ( string levelName )
//-------------------------------------------------------------------
{
	cpVect              wallStart, wallFinish;
	_physicObject       tempWall;
	int                 indexCount = 0;
	cpShapeFilter       wallShapeFilter;
	std::bitset<32>     wallBitset;

	if ( 0 == levelInfo.at ( levelName ).numLineSegments )
		return;

	// TODO: Remove this after game finished and all physics objects destroyed
	if ( levelInfo.at ( levelName ).wallPhysicsCreated )     // Create the physics as we visit the level
		return;

	levelInfo.at ( levelName ).solidWalls.clear ();
	levelInfo.at ( levelName ).solidWalls.reserve ( static_cast<unsigned long>(levelInfo.at ( levelName ).numLineSegments));

	for ( intptr_t i = 0; i != levelInfo.at ( levelName ).numLineSegments; i++ )
	{
		wallStart = levelInfo.at ( levelName ).lineSegments[indexCount++];
		wallFinish = levelInfo.at ( levelName ).lineSegments[indexCount++];

		tempWall.body = cpBodyNewStatic ();
		tempWall.shape = cpSegmentShapeNew ( tempWall.body, wallStart, wallFinish, wallRadius );
		levelInfo.at ( levelName ).solidWalls.push_back ( tempWall );
		cpSpaceAddShape ( space, levelInfo.at ( levelName ).solidWalls[i].shape );
		cpShapeSetFriction ( levelInfo.at ( levelName ).solidWalls[i].shape, wallFriction );

		cpShapeSetCollisionType (levelInfo.at(levelName).solidWalls[i].shape, PHYSIC_TYPE_WALL);
		//
		// Setup collision shape filtering bitmasks
		//
		wallBitset.reset();                                         // Clear the bitset
		wallBitset = levelInfo.at ( levelName ).deckCategory;       // Each wall on this level is in this category
		wallShapeFilter.categories = static_cast<cpBitmask>(wallBitset.to_ulong());     // Set the category

		wallBitset.reset();                                         // Clear the bitset

		wallBitset = levelInfo.at ( levelName ).deckCategory;       // Collide with everything in this category ( includes other droids on this level )
//		wallBitset[PHYSIC_TYPE_ENEMY] = true;                       // The other droids
		wallBitset[PHYSIC_TYPE_PLAYER] = true;                      // and the player
		wallShapeFilter.mask = static_cast<cpBitmask>(wallBitset.to_ulong());

		wallShapeFilter.group = CP_NO_GROUP;

		cpShapeSetFilter ( levelInfo.at ( levelName ).solidWalls[i].shape, wallShapeFilter );

		cpShapeSetUserData ( levelInfo.at ( levelName ).solidWalls[i].shape, (cpDataPointer) i );
	}

	levelInfo.at ( levelName ).wallPhysicsCreated = true;
	cpSpaceReindexStatic ( space );
}

