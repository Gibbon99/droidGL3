#include <hdr/system/sys_audio.h>
#include <hdr/game/gam_transfer.h>
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/system/sys_maths.h"
#include "hdr/game/gam_physicsCollisions.h"

cpCollisionHandler *handlerPlayerBullet;
cpCollisionHandler *handlerEnemyEnemy;
cpCollisionHandler *handlerEnemyPlayer;
cpCollisionHandler *handlerWallBullet;
cpCollisionHandler *handlerDoorBullet;
cpCollisionHandler *handlerEnemyBullet;


//-------------------------------------------------------------------
//
// Post-Step callback for hitting droid - needed in post-step so when its
// destroyed the shape and body are removed from the world
static void handleDamageDroidCollision ( cpSpace *space, cpShape *shape, int *passedInValues )
//-------------------------------------------------------------------
{
  unsigned char values[4];

  int valuesPassed = 0; // = new int ();

  // deference pointer to data
  valuesPassed = *(static_cast<int*>(passedInValues));

  values[0] = static_cast<unsigned char>((valuesPassed >> 24) & 0xff);        // whichlevel
  values[1] = static_cast<unsigned char>((valuesPassed >> 16) & 0xff);        // which enemy droid index
  values[2] = static_cast<unsigned char>((valuesPassed >> 8) & 0xff);         // Damage type
  values[3] = static_cast<unsigned char>(valuesPassed & 0xff);                // sourceDroid

  printf("Inside damage [ %i %i %i %i ]\n", values[0], values[1], values[2], values[3]);


  if ( 127 == values[3] )   // Player is the source
    gam_damageToDroid ( values[0], values[1], values[2], -1 );
  else
    gam_damageToDroid ( values[0], values[1], values[2], values[3] );

//	delete valuesPassed;
}

//-------------------------------------------------------------------
//
// Handle bullet hitting enemy droid - could be another enemy or player bullet
bool handleCollisionEnemyBullet ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
  // Get the cpShapes involved in the collision
  //
  // The order is A = ENEMY and B = BULLET
  //
  // A is the ENEMY Droid
  // B is the bullet
  //
  cpShape *a, *b;
  cpDataPointer dataPointer_A, dataPointer_B;
  unsigned char valuesPassedDroid_A[4], valuesPassedDroid_B[4];

  cpArbiterGetShapes ( arb, &a, &b );

  dataPointer_A = cpShapeGetUserData ( a );
  dataPointer_B = cpShapeGetUserData ( b );

  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

  int *passValueBullet = new int ();        // Memory leak?? using delete passValue causes bad values to be passed
  int packedValue = 0;

  char sourceDroid;

  sourceDroid = static_cast<char>(levelInfo.at (lvl_returnLevelNameFromDeck (valuesPassedDroid_B[BYTE_ZERO])).bullet[valuesPassedDroid_B[BYTE_ONE]].sourceDroid);

  printf ("bullet hit droid [ %i ]  - level [ %i ] source [ %i ]\n", valuesPassedDroid_A[BYTE_ONE], valuesPassedDroid_A[BYTE_ZERO], sourceDroid);

  levelInfo.at( lvl_returnLevelNameFromDeck (valuesPassedDroid_A[BYTE_ZERO]) ).droid[valuesPassedDroid_A[BYTE_ONE]].beenShotByPlayer = true;  // check right index
  levelInfo.at( lvl_returnLevelNameFromDeck (valuesPassedDroid_A[BYTE_ZERO]) ).droid[valuesPassedDroid_A[BYTE_ONE]].targetIndex = sourceDroid;

  packedValue = sys_pack4Bytes (valuesPassedDroid_A[BYTE_ZERO], valuesPassedDroid_A[BYTE_ONE], DAMAGE_BULLET, sourceDroid );
  *passValueBullet = packedValue;

  cpSpaceAddPostStepCallback ( space, (cpPostStepFunc) handleDamageDroidCollision, a, passValueBullet );

    //
  // Remove the bullet after the collision
  evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_REMOVE_BULLET, valuesPassedDroid_B[BYTE_ZERO], valuesPassedDroid_B[BYTE_ONE], -1, glm::vec2{}, glm::vec2{}, "");

  return cpFalse;	// should this be true or false
}

//-------------------------------------------------------------------
//
// Remove bullet if collided with wall
bool handleCollisionWallBullet ( cpArbiter *arb, cpSpace *space, int *unused)
//-------------------------------------------------------------------
{
	// A is the wall
	// B is the bullet
	cpShape *a, *b;
	cpDataPointer dataPointer_A, dataPointer_B;
	unsigned char valuesPassedDroid_A[4], valuesPassedDroid_B[4];

	cpArbiterGetShapes ( arb, &a, &b );

	dataPointer_A = cpShapeGetUserData ( a );
	dataPointer_B = cpShapeGetUserData ( b );

	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

	evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_REMOVE_BULLET, valuesPassedDroid_B[BYTE_ZERO], valuesPassedDroid_B[BYTE_ONE], -1, glm::vec2{}, glm::vec2{}, "");

	return cpTrue;
}

//-------------------------------------------------------------------
//
// Check before collision needs to be handled if it should continue
bool handleCollisionDroidCheck ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
	// Get the cpShapes involved in the collision
	//
	// The order is A = ENEMY and B = PLAYER if userData == -1
	//
	//  Or A = ENEMY and B = ENEMY
	cpShape             *a, *b;
	cpDataPointer       dataPointer_A,          dataPointer_B;
	unsigned char       valuesPassedDroid_A[4], valuesPassedDroid_B[4];

	cpArbiterGetShapes ( arb, &a, &b );

	dataPointer_A = cpShapeGetUserData ( a );
	dataPointer_B = cpShapeGetUserData ( b );

	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

	if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].ignoreCollisions ||
	     levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).droid[valuesPassedDroid_B[BYTE_ONE]].ignoreCollisions )
		return cpFalse;

	return cpTrue;
}

//-------------------------------------------------------------------
//
// Handle ENEMY hitting ENEMY droid or PLAYER to ENEMY
void handleCollisionDroidToDroid ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
	// Get the cpShapes involved in the collision
	//
	// The order is A = ENEMY and B = PLAYER if userData == -1
	//
	//  Or A = ENEMY and B = ENEMY
	cpShape             *a, *b;
	cpDataPointer       dataPointer_A,          dataPointer_B;
	unsigned char       valuesPassedDroid_A[4], valuesPassedDroid_B[4];

	cpArbiterGetShapes ( arb, &a, &b );

	dataPointer_A = cpShapeGetUserData ( a );
	dataPointer_B = cpShapeGetUserData ( b );

	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

	int *passValue = new int ();        // Memory leak?? using delete passValue causes bad values to be passed
	int packedValue = 0;

	if ( 1 == valuesPassedDroid_B[BYTE_TWO] )    // Is B the player
	{
		//
		// Process player vs enemy collision
		if ( !levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].currentMode == DROID_MODE_EXPLODING )
		{
			packedValue = sys_pack4Bytes (valuesPassedDroid_A[BYTE_ZERO], valuesPassedDroid_A[BYTE_ONE], DAMAGE_COLLISION, 127 );
			*passValue = packedValue;

			cpSpaceAddPostStepCallback ( space, (cpPostStepFunc) handleDamageDroidCollision, a, passValue );

// TODO fix up			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else    // Player collided with exploding sprite - take damage
		{
// TODO			if ( valuesPassedDroid_A[BYTE_ONE] != playerDroid.droidTransferedIntoIndex )
				// Ignore explosion if colliding with recently transferred droid
			{
				packedValue = sys_pack4Bytes (valuesPassedDroid_A[BYTE_ZERO], DAMAGE_EXPLOSION, 0, 0 );
				*passValue = packedValue;

				cpSpaceAddPostStepCallback ( space, (cpPostStepFunc) handleDamageDroidCollision, a, passValue );
				evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
			}
		}
	}
	else
	{
		//
		// Droid collided with another droid
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].collisionCount++;
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).droid[valuesPassedDroid_B[BYTE_ONE]].collisionCount++;

		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].hasCollided = true;
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).droid[valuesPassedDroid_B[BYTE_ONE]].hasCollided = true;

		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].collidedWith = valuesPassedDroid_B[BYTE_ONE];
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).droid[valuesPassedDroid_B[BYTE_ONE]].collidedWith = valuesPassedDroid_A[BYTE_ONE];

		if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].currentMode == DROID_MODE_EXPLODING )
		{
			gam_damageToDroid ( valuesPassedDroid_B[BYTE_ZERO], valuesPassedDroid_B[BYTE_ONE], DAMAGE_EXPLOSION, valuesPassedDroid_A[BYTE_ONE] );
// TODO fix up						evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else
		{
			gam_damageToDroid ( valuesPassedDroid_B[BYTE_ZERO], valuesPassedDroid_B[BYTE_ONE], DAMAGE_COLLISION, valuesPassedDroid_A[BYTE_ONE] );
// TODO fix up						evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}

		if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).droid[valuesPassedDroid_B[BYTE_ONE]].currentMode == DROID_MODE_EXPLODING )
		{
			gam_damageToDroid ( valuesPassedDroid_A[BYTE_ZERO], valuesPassedDroid_A[BYTE_ONE], DAMAGE_EXPLOSION, valuesPassedDroid_B[BYTE_ONE] );
// TODO fix up						evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else
		{
			gam_damageToDroid ( valuesPassedDroid_A[BYTE_ZERO], valuesPassedDroid_A[BYTE_ONE], DAMAGE_COLLISION, valuesPassedDroid_B[BYTE_ONE] );
// TODO fix up						evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
	}

//	delete passValue;
}

//-------------------------------------------------------------------
//
// Collision between PLAYER and BULLET
// Ignore if it is a player bullet
bool handleCollisionPlayerBulletCheck ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
  // Get the cpShapes involved in the collision
  //
  // The order is A = PLAYER and B = BULLET
  //
  cpShape             *a, *b;
  cpDataPointer       dataPointer_A, dataPointer_B;
  unsigned char       valuesPassedDroid_A[4], valuesPassedDroid_B[4];

  cpArbiterGetShapes ( arb, &a, &b );

  dataPointer_A = cpShapeGetUserData ( a );
  dataPointer_B = cpShapeGetUserData ( b );

  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

  if (levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_ZERO] )).bullet[valuesPassedDroid_B[BYTE_TWO]].sourceDroid == -1)
    {
      return cpFalse;   // don't proceed with collision
    }

    return cpTrue;    // continue processing - hit enemy droid
}

//-------------------------------------------------------------------
//
// Collision between PLAYER and DROID - check transfer status
// Continue to post solve if TRANSFER == FALSE
bool handleCollisionTransferCheck ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
	// Get the cpShapes involved in the collision
	//
	// The order is A = ENEMY and B = PLAYER if userData == -1
	//
	//  Or A = ENEMY and B = ENEMY
	cpShape             *a, *b;
	cpDataPointer       dataPointer_A, dataPointer_B;
	unsigned char       valuesPassedDroid_A[4], valuesPassedDroid_B[4];

	cpArbiterGetShapes ( arb, &a, &b );

	dataPointer_A = cpShapeGetUserData ( a );
	dataPointer_B = cpShapeGetUserData ( b );

	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
	sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

	if ( 1 == valuesPassedDroid_B[BYTE_TWO] )    // Is B the player
	{
		if ( playerDroid.currentMode == DROID_MODE_TRANSFER )
          {
            printf ("collision - start transfer\n");
            gam_initTransfer(valuesPassedDroid_A[BYTE_ONE]);    // Pass in the droid to transfer with
            cpBodySetVelocity ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).droid[valuesPassedDroid_A[BYTE_ONE]].body, cpVect{ 0, 0} );
            return cpFalse;    // Don't continue processing collision
          }

	}
	return cpTrue;    // Continue processing collision
}


//-------------------------------------------------------------------
//
// Handle bullet hitting door sensor
bool handleDoorBullet ( cpArbiter *arb, cpSpace *space, int *unused )
//-------------------------------------------------------------------
{
  // Get the cpShapes involved in the collision
  //
  // The order is A = DOOR and B = BULLET
  //
  cpShape             *a, *b;
  cpDataPointer       dataPointer_A, dataPointer_B;
  unsigned char       valuesPassedDroid_A[4], valuesPassedDroid_B[4];

  cpArbiterGetShapes ( arb, &a, &b );

  dataPointer_A = cpShapeGetUserData ( a );
  dataPointer_B = cpShapeGetUserData ( b );

  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_A), valuesPassedDroid_A );
  sys_getPackedBytes ( static_cast<int>((intptr_t) dataPointer_B), valuesPassedDroid_B );

  switch ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_ZERO] )).doorTrigger[valuesPassedDroid_A[BYTE_ONE]].currentFrame )
    {
      case DOOR_ACROSS_OPENED:
      case DOOR_UP_OPENED:
        return cpFalse;
      break;

      default:
        evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_REMOVE_BULLET, valuesPassedDroid_B[BYTE_ZERO], valuesPassedDroid_B[BYTE_ONE], -1, glm::vec2{}, glm::vec2{}, "");
      return cpTrue;
      break;
    }
}

//-------------------------------------------------------------------
//
// Setup all the collision handlers
void sys_setupCollisionHandlers ()
//-------------------------------------------------------------------
{
    //
    // Handle collision between PLAYER and BULLET
    //
    handlerPlayerBullet = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_PLAYER, PHYSIC_TYPE_BULLET );
    handlerPlayerBullet->beginFunc = (cpCollisionBeginFunc) handleCollisionPlayerBulletCheck;
	//
	// Handle collision between ENEMY and ENEMY
	//
	handlerEnemyEnemy = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_ENEMY, PHYSIC_TYPE_ENEMY );
	handlerEnemyEnemy->beginFunc = (cpCollisionBeginFunc) handleCollisionDroidCheck;
	handlerEnemyEnemy->postSolveFunc = (cpCollisionPostSolveFunc) handleCollisionDroidToDroid;
	//
	// Handle collision between ENEMY and PLAYER
	//
	handlerEnemyPlayer = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_ENEMY, PHYSIC_TYPE_PLAYER );
	handlerEnemyPlayer->beginFunc = (cpCollisionBeginFunc) handleCollisionTransferCheck;
	handlerEnemyPlayer->postSolveFunc = (cpCollisionPostSolveFunc) handleCollisionDroidToDroid;
	//
	// Handle collision between wall and bullet
	//
	handlerWallBullet = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_WALL, PHYSIC_TYPE_BULLET );
	handlerWallBullet->beginFunc = (cpCollisionBeginFunc) handleCollisionWallBullet;
    //
    // Handle collision between door and bullet
    //
    handlerDoorBullet = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_DOOR, PHYSIC_TYPE_BULLET );
    handlerDoorBullet->beginFunc = (cpCollisionBeginFunc) handleDoorBullet;
    //
    // Handle BULLET hitting an ENEMY
    //
    handlerEnemyBullet = cpSpaceAddCollisionHandler ( space, PHYSIC_TYPE_ENEMY, PHYSIC_TYPE_BULLET );
    handlerEnemyBullet->postSolveFunc = (cpCollisionPostSolveFunc)handleCollisionEnemyBullet;
}
