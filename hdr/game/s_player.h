#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_droids.h"

extern _droid      playerDroid;

// Get the tile type the player is currently over
void s_getTileUnderPlayer(string levelName, float posX, float posY);

void s_renderPlayerSprite ();

