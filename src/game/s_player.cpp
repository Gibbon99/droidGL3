#include <hdr/game/s_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/s_render.h>
#include "hdr/game/s_player.h"

cpVect      playerWorldPosition;
_droid      playerDroid;
bool        doPlayerAnimate = true;
SDL_TimerID timerPlayerAnimate;
Uint32      playerAnimateInterval;      // From script

//------------------------------------------------------------------------------
//
// Process the player world position and movement
void s_processPlayerMovement()
//------------------------------------------------------------------------------
{

}

// ----------------------------------------------------------------------------
//
// Return the current animation player speed based on current health
Uint32 gam_getPlayerAnimateInterval()
// ----------------------------------------------------------------------------
{
	return 150;
}

// ----------------------------------------------------------------------------
//
// Set the state of the player animation timer
void gam_setPlayerAnimateState ( bool newState )
// ----------------------------------------------------------------------------
{
	doPlayerAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Animate healing tiles called from timer callback
Uint32 gam_playerAnimateTimerCallback ( Uint32 interval, void *param )
// ----------------------------------------------------------------------------
{
	if ( !doPlayerAnimate )
		return interval;

	playerDroid.currentFrame++;
	if ( playerDroid.currentFrame == sprites.at("001").numberOfFrames )
		playerDroid.currentFrame = 0;

	playerAnimateInterval = gam_getPlayerAnimateInterval ();

	return playerAnimateInterval;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the player sprite
//
// Pass in time in milliseconds
void gam_initPlayerAnimateTimer ( Uint32 interval )
// ----------------------------------------------------------------------------
{
	timerPlayerAnimate = SDL_AddTimer (interval, gam_playerAnimateTimerCallback, nullptr);   // Time in milliseconds
	if ( 0 == timerPlayerAnimate )
	{
		con_print (CON_ERROR, true, "Could not add player animate timer : [ %s ]", SDL_GetError ());
	}
}

//------------------------------------------------------------------------------
//
// Render the player sprite
void s_renderPlayerSprite()
//------------------------------------------------------------------------------
{
//	gl_renderSprite ("001", glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y}, 0, glm::vec3{1.0, 1.0, 1.0} );

    gl_renderSprite ("001", glm::vec2{winWidth / 2, winHeight / 2}, playerDroid.currentFrame, glm::vec3{1.0, 1.0, 1.0} );
}

//------------------------------------------------------------------------------
//
// Setup initial values for player
void s_initialPlayerSetup()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
//
// Get the tile type the player is currently over
void s_getTileUnderPlayer(string levelName, float posX, float posY)
//------------------------------------------------------------------------------
{
	int whichTile;

	whichTile = levelInfo.at(levelName).tiles[((int)(posY) * levelInfo.at(levelName).levelDimensions.x) + (int)posX];

	printf("posX [ %i [ PosY [ %i ] Over tile [ %i ]\n", (int)posX, (int)posY, whichTile);
}
