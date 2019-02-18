#include "hdr/game/gam_database.h"
#include "hdr/system/sys_maths.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/gam_droidAI.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_player.h"
#include "hdr/system/sys_utils.h"
#include "hdr/game/gam_droidAIPatrol.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_droids.h"
#include "hdr/gui/gui_terminal.h"

//------------------------------------------------------------------------------
//
// Animate the droid
void gam_animateThisLevel ( const string levelName )
//------------------------------------------------------------------------------
{
	if ( levelName.empty ())
	{
		con_print ( CON_ERROR, true, "Invalid or empty levelName passed to function [ %s ]", __LINE__ );
		return;
	}

	for ( int index = 0; index != levelInfo.at ( levelName ).numDroids; index++ )
	{
		if ( (levelInfo.at ( levelName ).droid[index].currentMode == DROID_MODE_NORMAL ) ||
				(levelInfo.at ( levelName ).droid[index].currentMode == DROID_MODE_EXPLODING ))
		{
            switch ( levelInfo.at ( levelName ).droid[index].currentMode )
              {
                case DROID_MODE_NORMAL:
                  levelInfo.at ( levelName ).droid[index].frameDelay += 1.0f / 3.0f;
                  break;

                case DROID_MODE_EXPLODING:
                  levelInfo.at ( levelName ).droid[index].frameDelay += 1.0f / 1.0f;
                  break;

                default:
                  break;
              }


			if ( levelInfo.at ( levelName ).droid[index].frameDelay > 1.0f )
			{
				levelInfo.at ( levelName ).droid[index].frameDelay = 0.0f;
				levelInfo.at ( levelName ).droid[index].currentFrame++;

				switch ( levelInfo.at ( levelName ).droid[index].currentMode )
				{
					case DROID_MODE_NORMAL:
						if ( levelInfo.at ( levelName ).droid[index].currentFrame ==
						     sprites.at ( levelInfo.at ( levelName ).droid[index].spriteName ).numberOfFrames )
							levelInfo.at ( levelName ).droid[index].currentFrame = 0;
						break;

					case DROID_MODE_EXPLODING:
						if ( levelInfo.at ( levelName ).droid[index].currentFrame == sprites.at ( "explosion" ).numberOfFrames )
                          {
                            levelInfo.at (levelName).droid[index].currentMode = DROID_MODE_DEAD;
                            evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_REMOVE_DROID_PHYSICS, index, 0, 0, glm::vec2{}, glm::vec2{}, levelName );
                          }
						break;

					default:
						break;
				}

			}
		}
	}
}

//------------------------------------------------------------------------------
//
// Render the droids for this level
void gam_renderDroids (const string levelName, float interpolate)
//------------------------------------------------------------------------------
{
	cpVect drawPosition;

	if ( levelInfo.at ( levelName ).droid.empty ())
		return;     // Nothing to render

	for ( int index = 0; index != levelInfo.at ( levelName ).numDroids; index++ )
	{
	  if (levelInfo.at ( levelName ).droid[index].currentMode != DROID_MODE_DEAD)
        {

          if (levelInfo.at (levelName).droid[index].visibleToPlayer)
//	    if (sys_visibleOnScreen(levelInfo.at(levelName).droid[index].worldPos, 32))
            {

              drawPosition = levelInfo.at (levelName).droid[index].worldPos;

              drawPosition.y = (int) drawPosition.y;   // Remove the fraction portion to stop blurring in Y direction

              switch (levelInfo.at (levelName).droid[index].currentMode)
                {
                  case DROID_MODE_NORMAL:
                    gl_renderSprite (levelInfo.at (levelName).droid[index].spriteName, SPRITE_TYPE_DROID, glm::vec2{drawPosition.x,drawPosition.y}, 0,
                        levelInfo.at (levelName).droid[index].currentFrame, glm::vec3{ 1.0f, 1.0f, 0.0f});
                  break;

                  case DROID_MODE_EXPLODING:
                    gl_renderSprite ("explosion", SPRITE_TYPE_DROID, glm::vec2{drawPosition.x, drawPosition.y}, 0,
                        levelInfo.at (levelName).droid[index].currentFrame, glm::vec3{ 1.0f, 1.0f, 0.0f});
                  break;

                  default: break;
                }
            }
        }
	}
}

//-----------------------------------------------------------------------------
//
//Setup initial values for enemy droids
void gam_initDroidValues ( const string levelName )
//-----------------------------------------------------------------------------
{
	_droid tempDroid;
	int wayPointCount = 1;

	// TODO - clear this out on game over and reset vector to 0
	levelInfo.at ( levelName ).droid.reserve ( static_cast<unsigned long>(levelInfo.at ( levelName ).numDroids));

	for ( int i = 0; i != levelInfo.at ( levelName ).numDroids; i++ )
	{

//		tempDroid.isAlive = true;
		tempDroid.droidType = levelInfo.at ( levelName ).droidTypes[i];
		tempDroid.currentHealth = dataBaseEntry[tempDroid.droidType].maxHealth;
		tempDroid.wayPointIndex = wayPointCount++; //rand() % (levelInfo.at(levelName).numWaypoints - 1);
		tempDroid.wayPointDirection = WAYPOINT_DOWN;
		tempDroid.spriteName = gl_getSpriteName ( tempDroid.droidType );
		tempDroid.currentFrame = 0;
		if ( sprites.empty ())
			tempDroid.numberOfFrames = 9;
		else
			tempDroid.numberOfFrames = sprites.at ( "001" ).numberOfFrames;

		tempDroid.frameDelay = 0.0f;
		tempDroid.currentSpeed = 0.0f;
		tempDroid.acceleration = dataBaseEntry[tempDroid.droidType].accelerate;
		tempDroid.worldPos = levelInfo.at ( levelName ).wayPoints[i];

		tempDroid.serverWorldPos = levelInfo.at ( levelName ).wayPoints[i];
		tempDroid.viewWorldPos = levelInfo.at ( levelName ).wayPoints[i];
		tempDroid.middlePosition = levelInfo.at ( levelName ).wayPoints[i];        // TODO - do maths for this

		tempDroid.destinationCoords = tempDroid.worldPos; //levelInfo.at(levelName).wayPoints[tempDroid.wayPointIndex];
//		ai_getNextWaypoint ( const string &levelName, int whichDroid )
		tempDroid.destDirection = sys_getDirection ( tempDroid.destinationCoords, tempDroid.worldPos );
		tempDroid.velocity = {0.0, 0.0};
		tempDroid.serverVelocity = {0.0, 0.0};

		tempDroid.overTile = 0;
		tempDroid.currentMode = DROID_MODE_NORMAL;      // TODO - set when the modes are done

		tempDroid.mass = static_cast<int>(strtol ( dataBaseEntry[tempDroid.droidType].weight.c_str (), nullptr, 10 ));  // This value plus the players base value
		tempDroid.mass += static_cast<int>(strtol ( dataBaseEntry[0].weight.c_str (), nullptr, 10 ));

		tempDroid.ignoreCollisions = false;
//		tempDroid.isExploding = false;

		tempDroid.targetIndex = -1;                // Which droid shot this droid
		tempDroid.beenShotByPlayer = false;
		tempDroid.beenShotCountdown = 0;

		tempDroid.collisionCount = 0;             // how many collision have occured to ignore them
		tempDroid.hasCollided = false;
		tempDroid.collidedWith = -1;               // Who did the droid hit


		tempDroid.ignoreCollisionsCounter = IGNORE_COLLISION_TIME;

		tempDroid.playerDroidTypeDBIndex = "001";     // What sort of droid is the player
//		tempDroid.droidTransferedIntoIndex = 0;

		tempDroid.currentMode = DROID_MODE_NORMAL;

		tempDroid.chanceToShoot = 0.0f;

		tempDroid.visibleToPlayer = true;

		//
// Weapon
		tempDroid.weaponCanFire = false;
		tempDroid.weaponDelay = 0.0f;

		tempDroid.witnessShooting = false;
		tempDroid.witnessTransfer = false;

		tempDroid.witnessShootingCountDown = 0.0f;
		tempDroid.witnessTransferCountDown = 0.0f;

		// AI variables
//		int ai_currentState;
		tempDroid.ai_moveMode = AI_MODE_WAYPOINT;

		// Pathfinding
		tempDroid.aStarPathIndex = -1;            // Index into which path to use
		tempDroid.numberPathNodes = -1;
		tempDroid.currentAStarIndex = -1;         // Index into aStarWaypoints array
		tempDroid.aStarDestinationFound = false;
		tempDroid.aStarInitDone = false;
		tempDroid.previousWaypoints = {};

		tempDroid.onFleeTile = false;
		tempDroid.foundFleeTile = false;

		tempDroid.isNotPatrolling = false;        // Used to enter resume branch
		tempDroid.onResumeDestTile = false;
		tempDroid.destSet = false;                // Does the droid have a destination

		tempDroid.onHealingTile = false;
		tempDroid.foundHealingTile = false;


		levelInfo.at ( levelName ).droid.push_back ( tempDroid );
	}
}


//-----------------------------------------------------------------------------
//
// Kill an enemy droid
// TODO Remove enemy physcis objects  - may need to be done in post-callback collision - Use event in mainLoop
// like for player and droid routine
void gam_destroyDroid ( int whichLevel, int whichDroid )
//-----------------------------------------------------------------------------
{
	switch ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentMode )
	{
		case DROID_MODE_EXPLODING:
			return;
			break;

		case DROID_MODE_NORMAL:

			if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].droidType < 6 )
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_EXPLODE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
			else
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_EXPLODE_2, 0, 0, glm::vec2 (), glm::vec2 (), "" );

			levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentFrame = 0;
			levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentMode = DROID_MODE_EXPLODING;
			levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].numberOfFrames = sprites.at ( "explosion" ).numberOfFrames;

			if (levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].aStarPathIndex > -1 )
              gam_AStarRemovePath ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].aStarPathIndex );
//			levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].isExploding = true;

//	levelInfo.at( lvl_returnLevelNameFromDeck ( whichLevel)).droid[whichDroid].currentFrameDelay = 0.0f;
//	levelInfo.at( lvl_returnLevelNameFromDeck ( whichLevel)).droid[whichDroid].isStopped = true;
			// TODO gam_addToScore ( dataBaseEntry[levelInfo.at( lvl_returnLevelNameFromDeck ( whichLevel)).droid[whichDroid].droidType].score );

			/* TODO

			par_addEmitter ( levelInfo.at( lvl_returnLevelNameFromDeck ( whichLevel)).droid[whichDroid].worldPos, PARTICLE_TYPE_EXPLOSION, -1 );
		*/

//
// TODO: Need extra checks here
//
			levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).numEnemiesAlive = levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).numDroids;

			for ( int i = 0; i != levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).numDroids; i++ )
			{
				if ( !levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[i].currentMode == DROID_MODE_NORMAL ||
				     levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[i].currentMode == DROID_MODE_EXPLODING )
				{
					levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).numEnemiesAlive--;
				}
			}

			if ( 0 == levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).numEnemiesAlive )
			{
// TODO 		gam_powerDownLevel ( whichLevel, true );
				con_print ( CON_INFO, false, "No enemies left on deck. [ %i ]", whichLevel );
			}

			return;
			break;

		default:


			return;
	}

}

//---------------------------------------------------------------
//
// Do damage to a droid
//
// damageSource can be either a bullet, explosion or collision
void gam_damageToDroid ( int whichLevel, int whichDroid, int damageSource, int sourceDroid )
//---------------------------------------------------------------
{
	switch ( damageSource )
	{
		case DAMAGE_BULLET:
			if ( -1 == sourceDroid ) // Player bullet
			{

			  printf("Droid [ %i ] hit by player bullet.\n", whichDroid);


				if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentMode == DROID_MODE_EXPLODING )
					return;

				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].targetIndex = sourceDroid;    // Set player as the target
				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].beenShotByPlayer = true;
				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].beenShotCountdown = droidBeenShotValue;

				//
				// Need to work out bullet damage when using non firing droid
				//
				if ( dataBaseEntry[gui_get3DTextureIndex (playerDroid.playerDroidTypeDBIndex)].canShoot )
					levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth -= dataBaseEntry[gui_get3DTextureIndex (playerDroid.playerDroidTypeDBIndex)].bulletDamage;
				else
					levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth -= dataBaseEntry[0].bulletDamage;

				if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth <= 0 )
				{
					gam_destroyDroid ( whichLevel, whichDroid );
				}
				else
				{
					evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
				}
			}
			else // hit by another droid bullet
			{
				if ( sourceDroid == whichDroid )
					return;

				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].targetIndex = sourceDroid;    // Set this droid as the target
				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].beenShotByPlayer = false;
				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth -= dataBaseEntry[levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[sourceDroid].droidType].bulletDamage;
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );

				if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth <= 0 )
				{
					gam_destroyDroid ( whichLevel, whichDroid );
				}
			}

			break;

		case DAMAGE_EXPLOSION:

			if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentMode == DROID_MODE_EXPLODING )
				return;

			if ( -1 == sourceDroid )
			{
				//
				// Player is colliding with exploding sprite
				gam_doDamageToPlayer ( DAMAGE_EXPLOSION, whichDroid );
			}
			else
			{
				//
				// Enemy Droid is colliding with another one exploding
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth -= collisionExplosionDamage;

				if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth <= 0 )
				{
					gam_destroyDroid ( whichLevel, whichDroid );
				}
			}
			break;

		case DAMAGE_COLLISION:
			if ( -1 == sourceDroid )
			{
				gam_doDamageToPlayer ( DAMAGE_COLLISION, whichDroid );

				printf ( "Droid health [ %i ]\n", levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth );

				levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth -= collisionDamageInflicted;
				if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( whichLevel )).droid[whichDroid].currentHealth <= 0 )
				{
					gam_destroyDroid ( whichLevel, whichDroid );
				}
			}
			break;
	}
}


//-----------------------------------------------------------------------------
//
// process ignore collosions
void gam_processIgnoreCollisions ( const string whichLevel, int whichDroid )
//-----------------------------------------------------------------------------
{
    if ( levelInfo.at ( whichLevel).droid[whichDroid].currentMode == DROID_MODE_DEAD)
      return;

	if ( levelInfo.at ( whichLevel ).droid[whichDroid].collisionCount < (rand () % 5) + 3 )
	{
		return;
	}

	levelInfo.at ( whichLevel ).droid[whichDroid].ignoreCollisions = true;

	levelInfo.at ( whichLevel ).droid[whichDroid].ignoreCollisionsCounter -= 1.0f * (1.0f / 30.0f);

	if ( levelInfo.at ( whichLevel ).droid[whichDroid].ignoreCollisionsCounter < 0.0f )
	{
		levelInfo.at ( whichLevel ).droid[whichDroid].ignoreCollisionsCounter = IGNORE_COLLISION_TIME;
		levelInfo.at ( whichLevel ).droid[whichDroid].ignoreCollisions = false;
		levelInfo.at ( whichLevel ).droid[whichDroid].collisionCount = 0;
		levelInfo.at ( whichLevel ).droid[whichDroid].hasCollided = false;
	}
}

//-----------------------------------------------------------------------------
//
// Process weapon timings
void gam_droidWeaponCharge ( int whichDroid, const string levelName )
//-----------------------------------------------------------------------------
{
  if (levelInfo.at (levelName ).droid[whichDroid].weaponCanFire)
    return;

  levelInfo.at ( levelName ).droid[whichDroid].weaponDelay += dataBaseEntry[levelInfo.at ( levelName ).droid[whichDroid].droidType].rechargeTime * (1.0f * (1.0f / 30.0f));

  if ( levelInfo.at ( levelName ).droid[whichDroid].weaponDelay > 1.0f )
    {
      levelInfo.at ( levelName ).droid[whichDroid].weaponDelay = 0.0f;
      levelInfo.at ( levelName ).droid[whichDroid].weaponCanFire = true;
    }
}
