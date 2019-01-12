#include "hdr/game/gam_droidAI.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/gam_database.h"
#include "hdr/game/gam_levels.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_render.h"
#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/io/io_keyboard.h"
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
	if ( eventMoveLeft )
	{
		playerDroid.velocity.x -= dataBaseEntry[0].accelerate;
		if ( playerDroid.velocity.x < -dataBaseEntry[0].maxSpeed )
			playerDroid.velocity.x = -dataBaseEntry[0].maxSpeed;
	}
	else if ( eventMoveRight )
	{
		playerDroid.velocity.x += dataBaseEntry[0].accelerate;
		if ( playerDroid.velocity.x > dataBaseEntry[0].maxSpeed )
			playerDroid.velocity.x = dataBaseEntry[0].maxSpeed;
	}

	if ( eventMoveUp )
	{
		playerDroid.velocity.y -= dataBaseEntry[0].accelerate;
		if (playerDroid.velocity.y < -dataBaseEntry[0].maxSpeed)
			playerDroid.velocity.y = -dataBaseEntry[0].maxSpeed;
	}
	else if ( eventMoveDown )
	{
		playerDroid.velocity.y += dataBaseEntry[0].accelerate;
		if (playerDroid.velocity.y > dataBaseEntry[0].maxSpeed)
			playerDroid.velocity.y = dataBaseEntry[0].maxSpeed;
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

	playerDroid.worldPos = cpBodyGetPosition (playerPhysicsObject.body);        // Call after physics processing?

	playerDroid.middlePosition.x = playerDroid.worldPos.x + (DROID_SIZE * 0.5);
	playerDroid.middlePosition.y = playerDroid.worldPos.y + (DROID_SIZE * 0.5);

	viewableScreenCoord.x = playerDroid.worldPos.x - ( winWidth / 2 );
	viewableScreenCoord.y = playerDroid.worldPos.y - ( winHeight / 2 );

	playerDroid.overTile = gam_getTileUnderPlayer (lvl_getCurrentLevelName (), playerDroid.middlePosition.x / TILE_SIZE, playerDroid.middlePosition.y / TILE_SIZE);
//	net_sendPositionUpdate (0);
}

//------------------------------------------------------------------------------
//
// Setup initial values for player
void gam_initialPlayerSetup ()
//------------------------------------------------------------------------------
{
	float   massWeight;

	playerDroid.mass = -1;
	playerDroid.inTransferMode = false;

	massWeight = static_cast<int>(strtof ( dataBaseEntry[0].weight.c_str(), nullptr ));

	if (!physicsStarted)
	{
		con_print(CON_ERROR, true, "Attempting to setup player physics with no engine ready.");
		return;
	}
	sys_setPlayerMass (massWeight);
	sys_setupPlayerPhysics ();
	sys_setPlayerMass (massWeight);
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
	timerPlayerAnimate = evt_registerTimer(interval, gam_playerAnimateTimerCallback, "Player animation");
}

//------------------------------------------------------------------------------
//
// Render the player sprite
void gam_renderPlayerSprite ()
//------------------------------------------------------------------------------
{
    gl_renderSprite ("001", glm::vec2{winWidth / 2, winHeight / 2}, playerDroid.currentFrame, glm::vec3{1.0, 1.0, 1.0} );

//	gl_renderSprite ("001", glm::vec2{playerDroid.serverWorldPos.x, playerDroid.serverWorldPos.y}, playerDroid.currentFrame, glm::vec3{1.0, 1.0, 0.0});
}

//------------------------------------------------------------------------------
//
// Get the tile type the player is currently over
int gam_getTileUnderPlayer ( string levelName, float posX, float posY )
//------------------------------------------------------------------------------
{
	return levelInfo.at(levelName).tiles[((int)(posY) * levelInfo.at(levelName).levelDimensions.x) + (int)posX];
}


//-----------------------------------------------------------------------------
//
// Do damage to player health
// Can either be from DAMAGE_BULLET, DAMAGE_COLLISION or DAMAGE_EXPLOSION
void gam_doDamageToPlayer ( int damageSource, int sourceDroid )
//-----------------------------------------------------------------------------
{
	switch ( damageSource )
	{
		case DAMAGE_BULLET:
			playerDroid.currentHealth -= dataBaseEntry[levelInfo.at(lvl_getCurrentLevelName ()).droid[sourceDroid].droidType].bulletDamage;
//				printf("Player health now [ %i ]\n", playerCurrentHealth);
			break;

		case DAMAGE_COLLISION:
			playerDroid.currentHealth -= collisionDamageInflicted;
			break;

		case DAMAGE_EXPLOSION:
			playerDroid.currentHealth -= collisionExplosionDamage;
			break;

		default:
			break;
	}

	evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );

	if ( playerDroid.currentHealth < 0 )
	{
		// End Game
		con_print ( CON_INFO, true, "PLAYER DEAD [ %i ]", playerDroid.currentHealth );
		/* TODO
		playerIsExploding = true;	// Draw player exploding
		playerCurrentFrame = 0;		// Start from beginning of explosion animation
		sys_stopAllSounds();
		sys_playSound(SND_EXPLODE_2, SND_PAN_LEFT, ALLEGRO_PLAYMODE_ONCE);
		sys_playSound(SND_EXPLODE_2, SND_PAN_RIGHT, ALLEGRO_PLAYMODE_ONCE);
		sys_changeMode ( MODE_PLAYER_EXPLODE, false );
		 */
	}
}
