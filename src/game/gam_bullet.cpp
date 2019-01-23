#include <hdr/system/sys_maths.h>
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_bullet.h"

float bulletMass;
float bulletTravelSpeed;

//-----------------------------------------------------------------------------
//
// Init bullet array for passed in level
void bul_initArray (const string levelName)
//-----------------------------------------------------------------------------
{
  _bullet tempBullet;

  for (int i = 0; i != 16; i++)
    {
      tempBullet.isAlive = false;
      levelInfo.at (levelName).bullet.push_back (tempBullet);
    }
}

//-----------------------------------------------------------------------------
//
// Remove a bullet from the world
void bul_removeBullet (int whichDeck, int whichBullet_1, int whichBullet_2)
//-----------------------------------------------------------------------------
{
  string levelName;

  levelName = lvl_returnLevelNameFromDeck (whichDeck);

  if (!levelInfo.at (levelName).bullet[whichBullet_1].isAlive)
    return;

  if (whichBullet_2 > -1)
    if (!levelInfo.at (levelName).bullet[whichBullet_2].isAlive)
      return;

  if (cpTrue ==
      cpSpaceContainsShape (space, levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.shape))
    {
      cpSpaceRemoveShape (space, levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.shape);
      cpShapeFree (levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.shape);
    }
  else
    printf ("ERROR: Attempted to remove non existent shape - bullet [ %i ]\n", whichBullet_1);

  if (cpTrue ==
      cpSpaceContainsBody (space, levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.body))
    {
      cpSpaceRemoveBody (space, levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.body);
      cpBodyFree (levelInfo.at (levelName).bullet[whichBullet_1].bulletPhysicsObject.body);
    }
  else
    printf ("ERROR: Attempted to remove non existent body - bullet [ %i ]\n", whichBullet_1);

  cpSpaceReindexStatic (space);

  levelInfo.at (levelName).bullet[whichBullet_1].isAlive = false;

//	par_removeEmitter ( bulletIndex );

  if (-1 == whichBullet_2)
    {
//		par_addEmitter ( levelInfo.at (levelName).bullet[whichBullet_1].worldPos, PARTICLE_TYPE_SPARK, -1 );
      return;
    }

  if (whichBullet_2 > -1)
    {
      // This is the index of the second bullet to remove
//		par_addEmitter ( levelName).bullet[whichBullet_2].worldPos, PARTICLE_TYPE_SPARK, -1 );

      if (cpTrue ==
          cpSpaceContainsShape (space, levelInfo.at (levelName).bullet[whichBullet_2].bulletPhysicsObject.shape))
        {
          cpSpaceRemoveShape (space, levelInfo.at (levelName).bullet[whichBullet_2].bulletPhysicsObject.shape);
          cpSpaceRemoveBody (space, levelInfo.at (levelName).bullet[whichBullet_2].bulletPhysicsObject.body);

          cpShapeFree (levelInfo.at (levelName).bullet[whichBullet_2].bulletPhysicsObject.shape);
          cpBodyFree (levelInfo.at (levelName).bullet[whichBullet_2].bulletPhysicsObject.body);
        }

      levelInfo.at (levelName).bullet[whichBullet_2].isAlive = false;

//		bullet[controlParam].particleIndex = -1;

//		par_removeEmitter ( controlParam );
    }
}

//-----------------------------------------------------------------------------
//
// Get the direction for a bullet to travel in - not the same as the current
// direction vector
cpVect bul_getPlayerBulletDirection ()
//-----------------------------------------------------------------------------
{
  cpVect bulletDirection;

  bulletDirection.x = 0;
  bulletDirection.y = 0;

// TODO rotate angle of sprite
/*
	if ( true == inputAction[gameLeft].currentlyDown )
		bulletDirection.x = -1;

	if ( true == inputAction[gameRight].currentlyDown )
		bulletDirection.x = 1;

	if ( true == inputAction[gameDown].currentlyDown )
		bulletDirection.y = 1;

	if ( true == inputAction[gameUp].currentlyDown )
		bulletDirection.y = -1;

//	return bulletDirection;
*/
  bulletDirection.x = playerDroid.velocity.x;
  bulletDirection.y = playerDroid.velocity.y;

  return bulletDirection;
}

//-----------------------------------------------------------------------------
//
// Pass in bullet type and get index into image array
string bul_getBulletImageByType (int whichType)
//-----------------------------------------------------------------------------
{
  switch (whichType)
    {
      case BULLET_TYPE_NORMAL: return "bullet_001";
      break;

      case BULLET_TYPE_SINGLE: return "bullet_476";
      break;

      case BULLET_TYPE_DOUBLE: return "bullet_821";
      break;

      default: return "Bullet type invalid";
      break;
    }
}

//-----------------------------------------------------------------------------
//
// Get the starting position of a newly fired bullet
// based on current velocity of the droid
cpVect bul_getStartingPosition (cpVect currentPos, int bulletType, cpVect direction, cpVect size)
//-----------------------------------------------------------------------------
{
  cpVect bulletStart;
  cpVect bulletStartOffset;
  cpVect bulletReturnValue;

  bulletStart = currentPos;

  if ((direction.x < 0) && (direction.y < 0))   // Up/Left
    {
      bulletStart.x -= size.x + SPRITE_SIZE;
      bulletStart.y -= size.y + SPRITE_SIZE;
      return bulletStart;
    }

  if (direction.x < 0)
    {
      bulletStart.x -= size.x + SPRITE_SIZE;
      bulletStart.y -= SPRITE_SIZE / 2;
    }

  if (direction.x > 0)
    {
      bulletStart.x += size.x;
      bulletStart.y -= SPRITE_SIZE / 2;
    }

  if (direction.y < 0)
    {
      bulletStart.y -= size.y + SPRITE_SIZE;
      bulletStart.x -= SPRITE_SIZE / 2;
    }

  if (direction.y > 0)
    {
      bulletStart.y += size.y;
      bulletStart.x -= SPRITE_SIZE / 2;
    }

  return bulletStart;



//  bulletStart.x -= size.x;
//  bulletStart.y -= size.y;    // Center bullet

  bulletStartOffset = direction * 24;

  printf ("BulletStart [ %3.3f %3.3f ]\n", currentPos.x, currentPos.y);
  printf ("bulletStartOffset [ %3.3f %3.3f ]\n", bulletStartOffset.x, bulletStartOffset.y);

  bulletReturnValue.x = bulletStart.x + bulletStartOffset.x;
  bulletReturnValue.y = bulletStart.y - bulletReturnValue.y;

  printf ("bulletReturnValue [ %3.3f %3.3f ]\n", bulletReturnValue.x, bulletReturnValue.y);

//	bulletStart.y -= TILE_SIZE / 2;

  return bulletReturnValue;
}

//-----------------------------------------------------------------------------
//
// Create a new bullet
void bul_newBullet (cpVect sourcePos, cpVect destPos, int type, int sourceDroid, const string levelName)
//-----------------------------------------------------------------------------
{
  _bullet tempBullet;
  cpShapeFilter bulletShapeFilter;
  std::bitset<32> bulletBitset;

  if (-1 == sourceDroid)
    {
      tempBullet.travelDirection = cpvnormalize (bul_getPlayerBulletDirection ());
      tempBullet.sourceDroid = 127;
    }
  else
    {
      tempBullet.travelDirection = cpvsub (destPos, sourcePos);
      tempBullet.sourceDroid = sourceDroid;
      printf ("New bullet added from Droid [ %i ]\n", sourceDroid);
    }

  tempBullet.isAlive = true;


  if (BULLET_TYPE_DISRUPTER != type) // if bullet is not a disrupter
    {
      if (type == -1)   // Fix for when non bullet transferred droid shoots
        type = BULLET_TYPE_NORMAL; // Default to normal 001 bullet type

      tempBullet.currentAnimFrame = 0;
      tempBullet.speed = 0.009f; //bulletTravelSpeed;

      tempBullet.angle = cpvtoangle (tempBullet.travelDirection);

      tempBullet.bitmapName = bul_getBulletImageByType (type);
      tempBullet.size.x = sprites.at (tempBullet.bitmapName).frameWidth;
      tempBullet.size.y = sprites.at (tempBullet.bitmapName).frameHeight;

      tempBullet.worldPos = bul_getStartingPosition (sourcePos, type, tempBullet.travelDirection, tempBullet.size);
      //
      // Create collision information
      //
      tempBullet.bulletPhysicsObject.body = cpSpaceAddBody (space, cpBodyNew (bulletMass, cpMomentForBox (bulletMass, tempBullet.size.x, tempBullet.size.y)));

      cpBodySetMass (tempBullet.bulletPhysicsObject.body, bulletMass);

      tempBullet.bulletPhysicsObject.shape = cpSpaceAddShape (space, cpBoxShapeNew (tempBullet.bulletPhysicsObject.body, tempBullet.size.x, tempBullet.size.y, 2));

      cpShapeSetCollisionType (tempBullet.bulletPhysicsObject.shape, PHYSIC_TYPE_BULLET);

      cpBodySetPosition (tempBullet.bulletPhysicsObject.body, tempBullet.worldPos);
      //
      // Set bitsets for shape filtering
      bulletBitset.reset ();
      bulletBitset = levelInfo.at (lvl_getCurrentLevelName ()).deckCategory;        // Set category to this current level
      bulletShapeFilter.categories = static_cast<cpBitmask>(bulletBitset.to_ulong ());

      bulletBitset.reset ();

      bulletBitset = levelInfo.at (lvl_getCurrentLevelName ()).deckCategory;        // Collide with everything in this category ( droids, walls )

      bulletShapeFilter.mask = static_cast<cpBitmask>(bulletBitset.to_ulong ());
      bulletShapeFilter.group = CP_NO_GROUP;

      cpShapeSetFilter (tempBullet.bulletPhysicsObject.shape, bulletShapeFilter);

      evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_LASER, 0, 0, glm::vec2 (), glm::vec2 (), "");

// TODO		par_addEmitter ( tempBullet.worldPos, PARTICLE_TYPE_TRAIL, i );
    }
  else
    {
      evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DISRUPTOR, 0, 0, glm::vec2 (), glm::vec2 (), "");
      tempBullet.type = BULLET_TYPE_DISRUPTER;
      tempBullet.damageDone = false;
    }

  char bulletCount = 0;

//	for ( auto bulletItr : levelInfo.at ( levelName ).bullet )
  for (int i = 0; i != levelInfo.at (levelName).bullet.size (); i++)
    {
      if (!levelInfo.at (levelName).bullet[i].isAlive)
        {
          //
          // Pack in deck number
          // Array index number
          int bulletIndex = sys_pack4Bytes ((char) lvl_getDeckNumber (levelName), bulletCount, 0, 0);

          cpShapeSetUserData (tempBullet.bulletPhysicsObject.shape, (cpDataPointer) bulletIndex);    // Passed into collision routine

          levelInfo.at (levelName).bullet[i] = tempBullet;
          return;
        }
      bulletCount++;
    }

  levelInfo.at (levelName).bullet.push_back (tempBullet);
  int bulletIndex = sys_pack4Bytes ((char) lvl_getDeckNumber (levelName), static_cast<char>(levelInfo.at (levelName).bullet.size ()), static_cast<char>(sourceDroid), 0);

  printf ("New bullet index [ %i ]\n", static_cast<int>( levelInfo.at (levelName).bullet.size ()));

  cpShapeSetUserData (tempBullet.bulletPhysicsObject.shape, (cpDataPointer) bulletIndex);    // Passed into collision routine
}

//-----------------------------------------------------------------------------
//
// Move all the alive bullets
void bul_moveBullet ()
//-----------------------------------------------------------------------------
{
  cpVect tempVelocity;

  for (auto levelItr : levelInfo)         // For each level
    {
      for (int i = 0; i != levelItr.second.bullet.size (); i++)
//		for ( auto &bulletItr : levelItr.second.bullet )       // For each bullet
        {
          if (levelItr.second.bullet[i].isAlive)
            {
              if (levelItr.second.bullet[i].type != BULLET_TYPE_DISRUPTER)
                //			if ( bulletItr.type != BULLET_TYPE_DISRUPTER )
                {
                  //				tempVelocity = cpvmult ( bulletItr.travelDirection, bulletItr.speed );
                  tempVelocity = cpvmult (levelItr.second.bullet[i].travelDirection, levelItr.second.bullet[i].speed);

                  cpBodySetForce (levelItr.second.bullet[i].bulletPhysicsObject.body, tempVelocity);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// Draw all the alive bullets for this level
void bul_renderBullet (const string levelName)
//-----------------------------------------------------------------------------
{
  for (int i = 0; i != levelInfo.at (levelName).bullet.size (); i++)
    {
      if (levelInfo.at (levelName).bullet[i].type != BULLET_TYPE_DISRUPTER)
        {
          if (levelInfo.at (levelName).bullet[i].isAlive)
            {
              levelInfo.at (levelName).bullet[i].worldPos = cpBodyGetPosition (levelInfo.at (levelName).bullet[i].bulletPhysicsObject.body);

              gl_renderSprite (levelInfo.at (levelName).bullet[i].bitmapName, glm::vec2{
                                levelInfo.at (levelName).bullet[i].worldPos.x,
                                levelInfo.at (levelName).bullet[i].worldPos.y}, 0.0f, levelInfo.at (levelName).bullet[i].currentAnimFrame, glm::vec3{
                                -1, 0, 0});
            }
        }
    }
}
