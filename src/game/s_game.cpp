#include <hdr/game/s_levels.h>
#include "s_game.h"

//------------------------------------------------------------------------------
//
// Start a new game
void gam_startNewGame ()
//------------------------------------------------------------------------------
{
	lvl_changeToLevel ("Upper cargo");

	sys_changeMode (MODE_GAME);
}