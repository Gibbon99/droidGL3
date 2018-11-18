#pragma once

#include "hdr/game/gam_levels.h"
#include "hdr/system/sys_main.h"
#include "hdr/libGL/chipmunk/chipmunk.h"

#define PHYSIC_TYPE_WALL            0x01
#define PHYSIC_TYPE_ENEMY            0x02
#define PHYSIC_TYPE_PLAYER            0x04
#define PHYSIC_TYPE_BULLET            0x08
#define PHYSIC_TYPE_DOOR            0x16
#define PHYSIC_TYPE_PARTICLE        0x32

extern cpSpace *space;

extern float shipDamping;       // Set from startup script
extern float collisionSlop;     // Set from startup script
extern float wallFriction;      // Set from startup script
extern float wallRadius;        // Set from startup script
extern float gravity;            // Set from startup script

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
void sys_createSolidWalls ( string levelName );
