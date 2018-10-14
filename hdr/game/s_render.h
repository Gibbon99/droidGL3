#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"
#include "hdr/io/io_textures.h"

extern float    pixelX, pixelY, viewPixelX, viewPixelY;
extern float    tilePosY, tilePosX;
extern float    aspectRatioX, aspectRatioY;

// Copy all the tiles that are visible this screen to the array
void gam_drawAllTiles(string whichShader, GLuint whichTexture);

void gam_setSingleTileCoords(int posX, int totalWidth);

void gam_drawFullLevel(string levelName, string whichShader, GLuint whichTexture);
