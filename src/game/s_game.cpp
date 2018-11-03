#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include "s_game.h"

//------------------------------------------------------------------------------
//
// Start a new game
void gam_startNewGame ()
//------------------------------------------------------------------------------
{
	gl_createAllSprites ();

	lvl_changeToLevel ("Upper cargo");

	sys_changeMode (MODE_GAME);
}