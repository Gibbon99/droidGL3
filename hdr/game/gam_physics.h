#pragma once

//#include "hdr/game/gam_levels.h"
#include "hdr/system/sys_main.h"
#include "hdr/libGL/chipmunk/chipmunk.h"

enum PHYSIC_TYPES
{
	PHYSIC_TYPE_PLAYER = 21,      // one more than the number of levels
	PHYSIC_TYPE_ENEMY,
	PHYSIC_TYPE_BULLET,
	PHYSIC_TYPE_WALL,
	PHYSIC_TYPE_DOOR
};

extern bool                        physicsStarted;
extern cpSpace                      *space;

extern float                        shipDamping;       // Set from startup script
extern float                        collisionSlop;     // Set from startup script
extern float                        wallFriction;      // Set from startup script
extern float                        wallRadius;        // Set from startup script
extern float                        gravity;           // Set from startup script

typedef struct
{
	cpBody      *body = nullptr;
	cpShape     *shape = nullptr;
} _physicObject;

// Setup Physics engine - run once
bool sys_setupPhysicsEngine();

// Cleanup Physics engine - run once
void sys_freePhysicsEngine ();

// Create the solid walls for this level
void sys_createSolidWalls ( std::string levelName );

// Destroy all the bodies and shapes for this level
void sys_destroyPhysicObjects ( std::string whichLevel );

// Create the physics bodies and shapes for the enemy droids
void sys_createEnemyPhysics( std::string levelName);

// Update the droids information from physics properties
void drd_updateDroidPosition ( std::string levelName, int whichDroid );

// Remove the physics body for the enemy droid
void sys_removeEnemyPhysics ( int whichDroid, std::string levelName );