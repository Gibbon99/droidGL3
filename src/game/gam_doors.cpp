#include <hdr/game/gam_levels.h>
#include <hdr/game/gam_player.h>
#include <hdr/system/sys_audio.h>
#include <hdr/system/sys_maths.h>
#include "hdr/game/gam_doors.h"

float           doorFrameDelay;
bool            g_showDoorTriggers;

bool            doDoorAnimate = true;
SDL_TimerID     timerDoorAnimate;
Uint32          doorAnimateInterval;      // From script


//-----------------------------------------------------------------------------
//
// Create physics sensor shape for the passed in door index
//
// Sensor used to detect bullet collision
void sys_createDoorSensor ( int whichDoor, cpFloat width, cpFloat height, cpVect center, const string levelName )
//-----------------------------------------------------------------------------
{
  cpShapeFilter           doorShapeFilter;
  std::bitset<32>         doorBitset;
  int                     packedValue;

  levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.body = cpBodyNewStatic();
  cpBodySetCenterOfGravity ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.body, center );
  cpBodySetPosition ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.body, center );

  levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape = cpBoxShapeNew ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.body, width, height, 0 );
  cpSpaceAddShape ( space, levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape );
  cpShapeSetCollisionType ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape, PHYSIC_TYPE_DOOR );

  cpShapeSetUserData ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape, (cpDataPointer)(intptr_t)whichDoor );

  cpShapeSetSensor (levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape, static_cast<cpBool>(true));

  //
  // Setup collision shape filtering bitmasks
  //
  doorBitset.reset();                                         // Clear the bitset
  doorBitset = levelInfo.at ( levelName ).deckCategory;       // Each door on this level is in this category
  doorShapeFilter.categories = static_cast<cpBitmask>(doorBitset.to_ulong());     // Set the category

  doorBitset.reset();                                         // Clear the bitset

  doorBitset = levelInfo.at ( levelName ).deckCategory;       // Collide with everything in this category ( includes other droids on this level )
//		wallBitset[PHYSIC_TYPE_ENEMY] = true;                 // The other droids
  doorBitset[PHYSIC_TYPE_DOOR] = true;                        // and the door
  doorShapeFilter.mask = static_cast<cpBitmask>(doorBitset.to_ulong());

  doorShapeFilter.group = CP_NO_GROUP;

  cpShapeSetFilter ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape, doorShapeFilter );

  packedValue = sys_pack4Bytes( static_cast<char>(lvl_getDeckNumber ( levelName)), (char)whichDoor, 0, 0);
  cpShapeSetUserData ( levelInfo.at ( levelName ).doorTrigger[whichDoor].physics.shape, (cpDataPointer) packedValue);
}


// ----------------------------------------------------------------------------
//
// Play a door sound - pan based on door and player location
void gam_playDoorSound (int whichDoor, const string &levelName)
// ----------------------------------------------------------------------------
{
  float angle;

  angle = static_cast<float>(atan2 (
      levelInfo.at ( levelName ).doorTrigger[whichDoor].topLeft.y - playerDroid.worldPos.y,
      levelInfo.at ( levelName ).doorTrigger[whichDoor].topLeft.x - playerDroid.worldPos.x) * 180 / 3.1415);

  evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DOOR, static_cast<int>(abs (
      playerDroid.worldPos.x - levelInfo.at ( levelName ).doorTrigger[whichDoor].topLeft.x)), false, glm::vec2{angle, 0}, glm::vec2 (), "");
}

// ----------------------------------------------------------------------------
//
// Process all the doors that are currently inUse
Uint32 gam_animateDoorCallback (Uint32 interval, void *param)
// ----------------------------------------------------------------------------
{
  int i;

  if (!doDoorAnimate)
    return interval;

  for (i = 0; i < levelInfo.at ( lvl_getCurrentLevelName() ).doorTrigger.size(); i++)
    {
      if (levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].inUse)
        {
          switch (levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame)
            {
              case DOOR_ACROSS:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_OPEN_1;
                gam_playDoorSound (i, lvl_getCurrentLevelName ());
              break;

              case DOOR_ACROSS_OPEN_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_OPEN_2;
              break;

              case DOOR_ACROSS_OPEN_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_OPENED:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_CLOSING_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_CLOSING_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_1;
              break;

              case DOOR_UP:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_OPEN_1;
                gam_playDoorSound (i, lvl_getCurrentLevelName ());
              break;

              case DOOR_UP_OPEN_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_OPEN_2;
              break;

              case DOOR_UP_OPEN_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_OPENED:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_CLOSING_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_CLOSING_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_CLOSING_1;
              break;

            }    // end of switch statement
        }    // end of inUse is true text
      else
        {    // trigger is not in use
          switch (levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame)
            {
              case DOOR_ACROSS_OPENED:
                gam_playDoorSound (i, lvl_getCurrentLevelName ());
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_1;
              break;

              case DOOR_ACROSS_CLOSING_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_2;
              break;

              case DOOR_ACROSS_CLOSING_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSED;
              break;

              case DOOR_UP_OPENED:
                gam_playDoorSound (i, lvl_getCurrentLevelName ());
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_CLOSING_1;
              break;

              case DOOR_UP_CLOSING_1:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_CLOSING_2;
              break;

              case DOOR_UP_CLOSING_2:
                levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame = DOOR_UP_CLOSED;
              break;
            }
        }
      levelInfo.at (lvl_getCurrentLevelName ()).tiles[levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].mapPos] = levelInfo.at ( lvl_getCurrentLevelName () ).doorTrigger[i].currentFrame;
    }
  return interval;
}    // end of for each door loop



// ----------------------------------------------------------------------------
//
// Set the state of the healing tile timer
void gam_setDoorAnimateState (bool newState)
// ----------------------------------------------------------------------------
{
  doDoorAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the door tiles
//
// Pass in time in milliseconds
void gam_initDoorAnimateTimer (Uint32 interval)
// ----------------------------------------------------------------------------
{
  timerDoorAnimate = evt_registerTimer (interval, gam_animateDoorCallback, "Door animation");
}

// ----------------------------------------------------------------------------
//
// Check door trigger areas against sprite positions
void gam_doorCheckTriggerAreas (const string levelName)
// ----------------------------------------------------------------------------
{
  int i = 0;
  int j = 0;


  if (levelInfo.at ( levelName ).doorTrigger.empty())
    return;        // no doors on this level to draw

  for (i = 0; i != levelInfo.at ( levelName ).doorTrigger.size(); i++)
    {
      levelInfo.at ( levelName ).doorTrigger[i].inUse = (playerDroid.middlePosition.x > levelInfo.at ( levelName ).doorTrigger[i].topLeft.x)
                             && (playerDroid.middlePosition.y > levelInfo.at ( levelName ).doorTrigger[i].topLeft.y)
                             && (playerDroid.middlePosition.x < levelInfo.at ( levelName ).doorTrigger[i].botRight.x)
                             && (playerDroid.middlePosition.y < levelInfo.at ( levelName ).doorTrigger[i].botRight.y);
    }
  //
  // now check all the enemy sprites against the doors
  //
  for (i = 0; i != levelInfo.at ( levelName ).doorTrigger.size(); i++)
    {
      for (j = 0; j != levelInfo.at (levelName).numDroids; j++)
        {
          if (DROID_MODE_NORMAL == levelInfo.at (levelName).droid[j].currentMode)
            {
              if ((levelInfo.at (levelName).droid[j].worldPos.x + (SPRITE_SIZE / 2) > levelInfo.at ( levelName ).doorTrigger[i].topLeft.x) &&
                  (levelInfo.at (levelName).droid[j].worldPos.y + (SPRITE_SIZE / 2) > levelInfo.at ( levelName ).doorTrigger[i].topLeft.y) &&
                  (levelInfo.at (levelName).droid[j].worldPos.x + (SPRITE_SIZE / 2) < levelInfo.at ( levelName ).doorTrigger[i].botRight.x) &&
                  (levelInfo.at (levelName).droid[j].worldPos.y + (SPRITE_SIZE / 2) < levelInfo.at ( levelName ).doorTrigger[i].botRight.y))
                {
                  levelInfo.at ( levelName ).doorTrigger[i].inUse = true;
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------
//
// Draw a debug quad for the door triggers
void gam_debugDoorTriggers (const string levelName)
//------------------------------------------------------------------------
{
  glm::vec2             textureSize;
  glm::vec2             scaleBy;
  static auto           haveSize = false;
  static glm::vec2      renderPosition;
  float texCoords[] =   {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};

  if (!g_showDoorTriggers)
    return;

  glEnable (GL_BLEND);
  glBlendFunc (GL_DST_COLOR, GL_SRC_ALPHA);

  for (int i = 0; i < levelInfo.at ( levelName ).doorTrigger.size(); i++)
    {

      textureSize = io_getTextureSize ("whiteSquare");

      textureSize.x = (float) (levelInfo.at ( levelName ).doorTrigger[i].topLeft.x - levelInfo.at ( levelName ).doorTrigger[i].topRight.x);
      textureSize.y = (float) (levelInfo.at ( levelName ).doorTrigger[i].topLeft.y - levelInfo.at ( levelName ).doorTrigger[i].botLeft.y);

      renderPosition.x = (float) levelInfo.at ( levelName ).doorTrigger[i].topLeft.x;
      renderPosition.y = (float) levelInfo.at ( levelName ).doorTrigger[i].topLeft.y;

      if ((levelInfo.at ( levelName ).doorTrigger[i].topRight.x - levelInfo.at ( levelName ).doorTrigger[i].topLeft.x)
          > (levelInfo.at ( levelName ).doorTrigger[i].botLeft.y - levelInfo.at ( levelName ).doorTrigger[i].topLeft.y))
        {
          // Across
          renderPosition.x += (TILE_SIZE * 2) + (TILE_SIZE / 2);
          renderPosition.y += (TILE_SIZE * 2) - (TILE_SIZE / 2);
        }
      else
        {
          // Vertical
          renderPosition.x += (TILE_SIZE * 1) + (TILE_SIZE / 2);
          renderPosition.y += (TILE_SIZE * 2) + (TILE_SIZE / 2);
        }

      if (levelInfo.at ( levelName ).doorTrigger[i].inUse)
        gl_draw2DQuad (renderPosition, 0, textureSize, "quad3d", io_getTextureID ("redSquare"), glm::vec3{0, 0, 0}, glm::vec3{1.0, 0.0, 0.0}, texCoords);
      else
        gl_draw2DQuad (renderPosition, 0, textureSize, "quad3d", io_getTextureID ("whiteSquare"), glm::vec3{0, 0, 0}, glm::vec3{1.0, 0.0, 0.0}, texCoords);
    }

  glDisable (GL_BLEND);
}

// ----------------------------------------------------------------------------
//
// Setup each door doorTrigger for this level
void gam_doorTriggerSetup (const string levelName)
// ----------------------------------------------------------------------------
{
  int             i;
  float           sourceX = 0;
  float           sourceY = 0;
  int             destX = 0;
  int             destY = 0;
  int             currentTile = 0;
  cpVect          center;
  _doorTrigger    tempDoorTrigger;
  cpFloat         width, height;

  for (i = 0; i != levelInfo.at (levelName).levelDimensions.x * levelInfo.at (levelName).levelDimensions.y; i++)
    {
      currentTile = levelInfo.at (levelName).tiles[(((int) sourceY * (levelInfo.at (levelName).levelDimensions.x)) + (int) sourceX)];

      if (currentTile < 0)
        {
          con_print (CON_ERROR, true, "Tile found in gam_doorTriggerSetup is below 0. loop [ %i ]", i);
        }

      if (
          (DOOR_ACROSS == currentTile) ||
          (DOOR_ACROSS_OPEN_1 == currentTile) ||
          (DOOR_ACROSS_OPEN_2 == currentTile) ||
          (DOOR_ACROSS_OPENED == currentTile) ||
          (DOOR_ACROSS_CLOSING_1 == currentTile) ||
          (DOOR_ACROSS_CLOSING_2 == currentTile) ||
          (DOOR_ACROSS_CLOSED == currentTile) ||
          (DOOR_UP == currentTile) ||
          (DOOR_UP_OPEN_1 == currentTile) ||
          (DOOR_UP_OPEN_2 == currentTile) ||
          (DOOR_UP_OPENED == currentTile) ||
          (DOOR_UP_CLOSING_1 == currentTile) ||
          (DOOR_UP_CLOSING_2 == currentTile) ||
          (DOOR_UP_CLOSED == currentTile)
          )
        {
          tempDoorTrigger.frameDelay = doorFrameDelay;
          tempDoorTrigger.nextFrame = 0.0f;
          tempDoorTrigger.mapPos = i;
          tempDoorTrigger.mapX = destX;
          tempDoorTrigger.mapY = destY;

          if ((DOOR_UP == currentTile) || (DOOR_UP_OPEN_1 == currentTile) || (DOOR_UP_OPEN_2 == currentTile)
              || (DOOR_UP_OPENED == currentTile) || (DOOR_UP_CLOSING_1 == currentTile)
              || (DOOR_UP_CLOSING_2 == currentTile) || (DOOR_UP_CLOSED == currentTile))
            {
              tempDoorTrigger.currentFrame = DOOR_UP_CLOSED;    // reset to default frame

              tempDoorTrigger.topLeft.x = (sourceX * TILE_SIZE) - (TILE_SIZE / 2); // - TILE_SIZE;
              tempDoorTrigger.topLeft.y = (sourceY * TILE_SIZE); // - (TILE_SIZE );

              tempDoorTrigger.topRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);
              tempDoorTrigger.topRight.y = (sourceY * TILE_SIZE); // - (TILE_SIZE );

              tempDoorTrigger.botLeft.x = (sourceX * TILE_SIZE) - (TILE_SIZE / 2); // - TILE_SIZE;
              tempDoorTrigger.botLeft.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2);

              tempDoorTrigger.botRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);
              tempDoorTrigger.botRight.y = (sourceY * TILE_SIZE) + (TILE_SIZE) * 2;

              center.x = (sourceX * TILE_SIZE) + (TILE_SIZE / 2);
              center.y = (sourceY * TILE_SIZE) + (TILE_SIZE / 2);
              width = TILE_SIZE / 3;    // TODO: Check they are never used
              height = TILE_SIZE;
            }
          else
            {
              tempDoorTrigger.currentFrame = DOOR_ACROSS;        // reset to default frame TODO Check frame type ( closed ?? )

              tempDoorTrigger.topLeft.x = (sourceX * TILE_SIZE); // - (TILE_SIZE / 2);
              tempDoorTrigger.topLeft.y = (sourceY * TILE_SIZE) - (TILE_SIZE / 2);

              tempDoorTrigger.topRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2);
              tempDoorTrigger.topRight.y = (sourceY * TILE_SIZE) - (TILE_SIZE / 2);

              tempDoorTrigger.botLeft.x = (sourceX * TILE_SIZE); // - (TILE_SIZE / 2);
              tempDoorTrigger.botLeft.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);

              tempDoorTrigger.botRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2);
              tempDoorTrigger.botRight.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);

              center.x = (sourceX * TILE_SIZE) + (TILE_SIZE / 2);
              center.y = (sourceY * TILE_SIZE);
            }

            levelInfo.at ( levelName ).doorTrigger.push_back( tempDoorTrigger );
            sys_createDoorSensor(static_cast<int>(levelInfo.at (levelName ).doorTrigger.size() - 1), width, height, center, levelName);
        }    // end of if statement

      destX++;
      sourceX++;
      if (destX == (levelInfo.at (levelName).levelDimensions.x))
        {
          destX = 0;
          sourceX = 0;
          destY++;
          sourceY++;
        }
    }
}

