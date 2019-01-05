#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/gam_physics.h"


// Setup droid information for this level
void drd_setupLevel ( std::string levelName );

// Render the droids for this level
void drd_renderThisLevel ( std::string levelName, float interpolate );

// Animate the droid
void drd_animateThisLevel ( std::string levelName );
