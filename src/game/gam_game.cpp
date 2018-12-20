#include <hdr/game/gam_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/gam_healing.h>
#include <hdr/game/gam_player.h>
#include <hdr/game/gam_doors.h>
#include <hdr/network/net_client.h>
#include "gam_game.h"

//------------------------------------------------------------------------------
//
// Start a new game
void gam_startNewGame (int gameType)
//------------------------------------------------------------------------------
{
	switch (gameType)
	{
		case NET_GAME_JOIN_NETWORK:
			isServer = false;
			break;

		case NET_GAME_SINGLE:
			isServer = true;
			break;

		default:
			break;
	}
	net_initLibrary ();

	net_startConnectionToServer(USER_EVENT_TIMER_ON);

  //
  // Turn off timers while changing level
	gam_setHealingState (false);
	gam_setPlayerAnimateState (false);
	gam_setDoorAnimateState (false);

	gl_createAllSprites ();

	gam_initialPlayerSetup ();

	lvl_changeToLevel ("Upper cargo");

	gam_doorTriggerSetup();

	sys_changeMode (MODE_GAME);

	// Turn on timers for animations
	gam_setHealingState (true);
	gam_setPlayerAnimateState(true);
	gam_setDoorAnimateState (true);
}
