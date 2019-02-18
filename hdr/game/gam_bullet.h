#pragma once

#include <bitset>
#include "hdr/system/sys_main.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/gam_levels.h"

#define NUM_STARTING_BULLETS    16

enum bulletTypes
{
	BULLET_TYPE_NORMAL = 0,
	BULLET_TYPE_SINGLE,
	BULLET_TYPE_DOUBLE,
	BULLET_TYPE_DISRUPTER
};

extern float        bulletMass;
extern float        bulletTravelSpeed;
extern Uint32       bulletAnimateInterval;      // From script

// Create a new bullet
void bul_newBullet ( cpVect sourcePos, cpVect destPos, int type, int sourceDroid, const string levelName );

// Move all the alive bullets
void bul_moveBullet ();

// Draw all the alive bullets for this level
void bul_renderBullet ( const string levelName );

// Remove a bullet from the world
void bul_removeBullet(int whichDeck, int whichBullet_1, int whichBullet_2);

// Init bullet array for passed in level
void bul_initArray(const string levelName);


// Test rotate a bullet sprite
void bul_testRotate();

// Initiate the timer to animate the bullet sprites
//
// Pass in time in milliseconds
void bul_initBulletAnimateTimer (Uint32 interval);

// Set the state of the healing tile timer
void bul_setBulletAnimateState (bool newState);

