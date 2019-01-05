#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/libGL/chipmunk/chipmunk.h"

extern cpShapeFilter   testShapeFilter;
extern cpShapeFilter   testShapeFilterPlayer;

// Setup all the collision handlers
void sys_setupCollisionHandlers ();
