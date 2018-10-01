#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/s_levels.h"
#include "hdr/io/io_textures.h"

struct _tileTexCoords
{
	cpVect texCoord;
};

// Setup up precalculated coords for the tiles
void gam_calcTileTexCoords(string textureName);
