#include <hdr/system/sys_audio.h>
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/system/sys_maths.h"
#include "hdr/game/gam_physicsCollisions.h"

cpCollisionHandler *handlerEnemyEnemy;
cpCollisionHandler *handlerEnemyPlayer;

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

	if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].ignoreCollisions ||
	     levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_LEVEL] )).droid[valuesPassedDroid_B[BYTE_ENEMY_INDEX]].ignoreCollisions )
		return cpFalse;

	return cpTrue;
}

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

	printf("Got passed in value [ %i ]\n", valuesPassed);

	values[0] = static_cast<unsigned char>((valuesPassed >> 24) & 0xff);        // whichlevel
	values[1] = static_cast<unsigned char>((valuesPassed >> 16) & 0xff);        // which enemy droid index
	values[2] = static_cast<unsigned char>((valuesPassed >> 8) & 0xff);         // Damage type
	values[3] = static_cast<unsigned char>(valuesPassed & 0xff);                // sourceDroid

	if ( 127 == values[3] )   // Player is the source
		drd_damageToDroid ( values[0], values[1], values[2], -1 );
	else
		drd_damageToDroid ( values[0], values[1], values[2], values[3] );

//	delete valuesPassed;
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

	if ( 1 == valuesPassedDroid_B[BYTE_PLAYER_FLAG] )    // Is B the player
	{
		//
		// Process player vs enemy collision
		if ( !levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].isExploding )
		{
			packedValue = sys_pack4Bytes (valuesPassedDroid_A[BYTE_LEVEL], valuesPassedDroid_A[BYTE_ENEMY_INDEX], DAMAGE_COLLISION, 127 );
			*passValue = packedValue;

			cpSpaceAddPostStepCallback ( space, (cpPostStepFunc) handleDamageDroidCollision, a, passValue );

			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else    // Player collided with exploding sprite - take damage
		{
			if ( valuesPassedDroid_A[BYTE_ENEMY_INDEX] != playerDroid.droidTransferedIntoIndex )
				// Ignore explosion if colliding with recently transferred droid
			{
				packedValue = sys_pack4Bytes (valuesPassedDroid_A[BYTE_LEVEL], DAMAGE_EXPLOSION, 0, 0 );
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
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].collisionCount++;
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_LEVEL] )).droid[valuesPassedDroid_B[BYTE_ENEMY_INDEX]].collisionCount++;

		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].hasCollided = true;
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_LEVEL] )).droid[valuesPassedDroid_B[BYTE_ENEMY_INDEX]].hasCollided = true;

		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].collidedWith = valuesPassedDroid_B[BYTE_ENEMY_INDEX];
		levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_LEVEL] )).droid[valuesPassedDroid_B[BYTE_ENEMY_INDEX]].collidedWith = valuesPassedDroid_A[BYTE_ENEMY_INDEX];

		if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].isExploding )
		{
			drd_damageToDroid ( valuesPassedDroid_B[BYTE_LEVEL], valuesPassedDroid_B[BYTE_ENEMY_INDEX], DAMAGE_EXPLOSION, valuesPassedDroid_A[BYTE_ENEMY_INDEX] );
			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else
		{
			drd_damageToDroid ( valuesPassedDroid_B[BYTE_LEVEL], valuesPassedDroid_B[BYTE_ENEMY_INDEX], DAMAGE_COLLISION, valuesPassedDroid_A[BYTE_ENEMY_INDEX] );
			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}

		if ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_B[BYTE_LEVEL] )).droid[valuesPassedDroid_B[BYTE_ENEMY_INDEX]].isExploding )
		{
			drd_damageToDroid ( valuesPassedDroid_A[BYTE_LEVEL], valuesPassedDroid_A[BYTE_ENEMY_INDEX], DAMAGE_EXPLOSION, valuesPassedDroid_B[BYTE_ENEMY_INDEX] );
			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DAMAGE, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
		else
		{
			drd_damageToDroid ( valuesPassedDroid_A[BYTE_LEVEL], valuesPassedDroid_A[BYTE_ENEMY_INDEX], DAMAGE_COLLISION, valuesPassedDroid_B[BYTE_ENEMY_INDEX] );
			evt_sendEvent ( USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_COLLIDE_1, 0, 0, glm::vec2 (), glm::vec2 (), "" );
		}
	}

//	delete passValue;
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

	if ( 1 == valuesPassedDroid_B[BYTE_PLAYER_FLAG] )    // Is B the player
	{
		if ( !playerDroid.inTransferMode )
			return cpTrue;    // Continue processing collision
	}
	//
	// Ignore collision and start transfer process
	// Transfer into Droid
	//
	cpBodySetVelocity ( levelInfo.at ( lvl_returnLevelNameFromDeck ( valuesPassedDroid_A[BYTE_LEVEL] )).droid[valuesPassedDroid_A[BYTE_ENEMY_INDEX]].body, cpVect{ 0, 0} );
// TODO	trn_startTransferMode ( whichDroid_A );
//	sys_changeMode ( MODE_TRANSFER_INTRO, true );

	return cpFalse;    // Don't continue processing collision
}


//-------------------------------------------------------------------
//
// Setup all the collision handlers
void sys_setupCollisionHandlers ()
//-------------------------------------------------------------------
{
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
}
