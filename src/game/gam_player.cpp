#include <hdr/game/gam_levels.h>
#include <hdr/opengl/gl_renderSprite.h>
#include <hdr/game/gam_render.h>
#include <hdr/game/gam_physicsPlayer.h>
#include <hdr/io/io_keyboard.h>
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_physics.h"

_droid      playerDroid;
bool        doPlayerAnimate = true;
SDL_TimerID timerPlayerAnimate;
Uint32      playerAnimateInterval;      // From script

//------------------------------------------------------------------------------
//
// Process the player world position and movement
void gam_processPlayerMovement ()
//------------------------------------------------------------------------------
{
	float playerMaxVelocity = 10.0f / 10000.0f;

	if ( eventMoveLeft )
	{
		playerDroid.velocity.x -= playerDroid.acceleration;
		if ( playerDroid.velocity.x < -playerMaxVelocity )
			playerDroid.velocity.x = -playerMaxVelocity;
	}
	else if ( eventMoveRight )
	{
		playerDroid.velocity.x += playerDroid.acceleration;
		if ( playerDroid.velocity.x > playerMaxVelocity )
			playerDroid.velocity.x = playerMaxVelocity;
	}

	if ( eventMoveUp )
	{
		playerDroid.velocity.y -= playerDroid.acceleration;
		if (playerDroid.velocity.y < -playerMaxVelocity)
			playerDroid.velocity.y = -playerMaxVelocity;
	}
	else if ( eventMoveDown )
	{
		playerDroid.velocity.y += playerDroid.acceleration;
		if (playerDroid.velocity.y > playerMaxVelocity)
			playerDroid.velocity.y = playerMaxVelocity;
	}

	if ( !eventMoveLeft )
	{
		if ( playerDroid.velocity.x < 0.0f )
		{
			playerDroid.velocity.x += gravity;
			if ( playerDroid.velocity.x > 0.0f )
				playerDroid.velocity.x = 0.0f;
		}
	}

	if ( !eventMoveRight )
	{
		if ( playerDroid.velocity.x > 0.0f )
		{
			playerDroid.velocity.x -= gravity;
			if ( playerDroid.velocity.x < 0.0f )
				playerDroid.velocity.x = 0.0f;
		}
	}

	if ( !eventMoveDown )
	{
		if ( playerDroid.velocity.y > 0.0f )
		{
			playerDroid.velocity.y -= gravity;
			if ( playerDroid.velocity.y < 0.0f )
				playerDroid.velocity.y = 0.0f;
		}
	}

	if ( !eventMoveUp )
	{
		if ( playerDroid.velocity.y < 0.0f )
		{
			playerDroid.velocity.y += gravity;
			if ( playerDroid.velocity.y > 0.0f )
				playerDroid.velocity.y = 0.0f;
		}
	}

	// TODO: Check if the client and the server are the same machine
	// If so - no need to run client updates - ie: copy location from server to client
	// playerDroid.clientWorldPos = playerDroid.serverWorldPos;

	cpBodySetForce (playerPhysicsObject.body, playerDroid.velocity);

	playerDroid.worldPos = cpBodyGetPosition (playerPhysicsObject.body);

	playerDroid.middlePosition.x = playerDroid.worldPos.x + (DROID_SIZE * 0.5);
	playerDroid.middlePosition.y = playerDroid.worldPos.y + (DROID_SIZE * 0.5);

	net_sendPositionUpdate (0);
}

//------------------------------------------------------------------------------
//
// Setup initial values for player
void gam_initialPlayerSetup ()
//------------------------------------------------------------------------------
{
	playerDroid.acceleration = 0.00008f;
	playerMass = 3000;

	sys_setPlayerMass (playerMass);
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
// Animate the player sprite - called from timer callback
// Runs at tick rate set by 'playerAnimateInterval' - derived from current health
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
void gam_renderPlayerSprite ()
//------------------------------------------------------------------------------
{
    gl_renderSprite ("001", glm::vec2{winWidth / 2, winHeight / 2}, playerDroid.currentFrame, glm::vec3{1.0, 1.0, 1.0} );

	gl_renderSprite ("001", glm::vec2{playerDroid.serverWorldPos.x, playerDroid.serverWorldPos.y}, playerDroid.currentFrame, glm::vec3{1.0, 1.0, 0.0});
}

//------------------------------------------------------------------------------
//
// Get the tile type the player is currently over
int gam_getTileUnderPlayer ( string levelName, float posX, float posY )
//------------------------------------------------------------------------------
{
	return levelInfo.at(levelName).tiles[((int)(posY) * levelInfo.at(levelName).levelDimensions.x) + (int)posX];
}
