#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/game/gam_levels.h"
#include "hdr/io/io_textures.h"

extern float    g_scaleViewBy;      // From script
extern int      g_playFieldSize;    // From script
extern GLuint                    fullLevel_FBO;
extern float       zDistance;

// Copy all the tiles that are visible this screen to the array
void gam_drawAllTiles ( string whichShader, string levelName, GLuint whichTexture );

void gam_setSingleTileCoords(int posX, int totalWidth);

void gam_renderFullLevel (const string levelName, const string whichShader, GLuint sourceTexture, float interpolate);

// Init the drawing for a level - called on a level change
void gam_initLevelDrawing (const string levelName );

// Reset the level init
void gam_resetRenderLevelInit ();
