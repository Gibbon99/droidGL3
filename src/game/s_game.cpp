#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/s_healing.h>
#include <hdr/game/s_player.h>
#include <hdr/game/s_doors.h>
#include "s_game.h"

//------------------------------------------------------------------------------
//
// Start a new game
void gam_startNewGame ()
//------------------------------------------------------------------------------
{
  //
  // Turn off timers while changing level
	gam_setHealingState (false);
	gam_setPlayerAnimateState (false);
	gam_setDoorAnimateState (false);

	gl_createAllSprites ();

	lvl_changeToLevel ("Upper cargo");

	gam_doorTriggerSetup();

	sys_changeMode (MODE_GAME);
}