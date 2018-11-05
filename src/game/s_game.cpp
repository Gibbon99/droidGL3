#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/s_healing.h>
#include "s_game.h"

//------------------------------------------------------------------------------
//
// Start a new game
void gam_startNewGame ()
//------------------------------------------------------------------------------
{
	gam_setHealingState (false);

	gl_createAllSprites ();

	lvl_changeToLevel ("Upper cargo");

	gam_setHealingState (true);

	sys_changeMode (MODE_GAME);
}