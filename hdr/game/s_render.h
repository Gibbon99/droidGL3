#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"
#include "hdr/io/io_textures.h"

struct _tileTexCoords
{
	vec2 texCoord;
};

// Setup up precalculated coords for the tiles
void gam_calcTileTexCoords(string textureName);

// Copy all the tiles that are visible this screen to the array
void gam_drawAllTiles(string whichShader, GLuint whichTexture);