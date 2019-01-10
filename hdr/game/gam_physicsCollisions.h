#pragma once

#define BYTE_LEVEL          0
#define BYTE_ENEMY_INDEX    1
#define BYTE_PLAYER_FLAG    2
#define BYTE_SPARE          3

#include "hdr/system/sys_main.h"
#include "hdr/libGL/chipmunk/chipmunk.h"

// Setup all the collision handlers
void sys_setupCollisionHandlers ();
