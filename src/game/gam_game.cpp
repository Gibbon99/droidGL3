#include "hdr/game/gam_levels.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_doors.h"
#include "hdr/network/net_client.h"
#include "hdr/game/gam_game.h"

float           baseGameSpeed;

//------------------------------------------------------------------------------
//
// Start a new game - initiated from the GUI
void gam_startNewGame (int gameType)
//------------------------------------------------------------------------------
{
	std::string      startingLevel;

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
	// Turn off timers setting up for a new game
	gam_setHealingState (false);
	gam_setPlayerAnimateState (false);
	gam_setDoorAnimateState (false);

	net_initLibrary ();

	net_startConnectionToServer(USER_EVENT_TIMER_ON);

	while (!haveServerConnection);      // Stop here until connected - need to put up some sort of message

	currentAlertLevel = ALERT_GREEN_TILE;

	gam_initialPlayerSetup ();          // Called before setting up enemy physics

	for ( auto &levelItr : levelInfo )
	{
		sys_createSolidWalls    ( levelItr.first );
		gam_initDroidValues     ( levelItr.first );
		sys_createEnemyPhysics  ( levelItr.first );
		gam_findHealingTiles    ( levelItr.first );
		gam_findLiftPositions   ( levelItr.first );
//		gam_doorTriggerSetup();
	}

	gl_createAllSprites ();     // TODO: Move this startup somewhere

	startingLevel = lvl_getStartingLevel ();
	lvl_changeToLevel (startingLevel, true, 0);
	levelInfo.at(startingLevel).containsClient = true;

	sys_changeMode (MODE_GAME);

	// Turn on timers for animations
	gam_setHealingState (true);
	gam_setPlayerAnimateState(true);
	gam_setDoorAnimateState (true);
}
