#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/gam_physics.h"

//Setup initial values for enemy droids
void gam_initDroidValues ( std::string levelName );

// Render the droids for this level
void drd_renderThisLevel ( std::string levelName, float interpolate );

// Animate the droid
void drd_animateThisLevel ( std::string levelName );

// damageSource can be either a bullet, explosion or collision
void drd_damageToDroid ( int whichLevel, int whichDroid, int damageSource, int sourceDroid );
