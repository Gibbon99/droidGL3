#pragma once

#include <bitset>
#include "hdr/system/sys_main.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/gam_levels.h"

enum bulletTypes
{
	BULLET_TYPE_NORMAL = 0,
	BULLET_TYPE_SINGLE,
	BULLET_TYPE_DOUBLE,
	BULLET_TYPE_DISRUPTER
};

extern float        bulletMass;
extern float        bulletTravelSpeed;

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
