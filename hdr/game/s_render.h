#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"
#include "hdr/io/io_textures.h"

extern float    worldLocationX, worldLocationY, viewWorldLocationX, viewWorldLocationY;
extern float    aspectRatioX, aspectRatioY;
extern float    g_scaleViewBy;      // From script

// Copy all the tiles that are visible this screen to the array
void gam_drawAllTiles ( string whichShader, string levelName, GLuint whichTexture );

void gam_setSingleTileCoords(int posX, int totalWidth);

void gam_drawFullLevel(string levelName, string whichShader, GLuint sourceTexture);
