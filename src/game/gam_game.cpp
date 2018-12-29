#include "hdr/game/gam_levels.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_doors.h"
#include "hdr/network/net_client.h"
#include "hdr/game/gam_game.h"

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

	//
	// Turn off timers while changing level
	gam_setHealingState (false);
	gam_setPlayerAnimateState (false);
	gam_setDoorAnimateState (false);

	net_initLibrary ();

	net_startConnectionToServer(USER_EVENT_TIMER_ON);

	while (!haveServerConnection);      // Stop here until connected - need to put up some sort of message


	gl_createAllSprites ();     // TODO: Move this startup somewhere

	gam_initialPlayerSetup ();

	lvl_changeToLevel (lvl_getStartingLevel (), true);

	gam_doorTriggerSetup();

	sys_changeMode (MODE_GAME);

	// Turn on timers for animations
	gam_setHealingState (true);
	gam_setPlayerAnimateState(true);
	gam_setDoorAnimateState (true);
}
