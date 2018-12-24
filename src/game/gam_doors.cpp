#include <hdr/game/gam_levels.h>
#include <hdr/game/gam_player.h>
#include <hdr/system/sys_audio.h>
#include "hdr/game/gam_doors.h"

_doorTrigger		doorTrigger[MAX_NUM_OF_DOORS];		// pointer to memory to hold door trigger information
int				    numDoorsOnLevel;					// how many doors are on this level
float				doorFrameDelay;
bool                g_showDoorTriggers;

bool                        doDoorAnimate = true;
SDL_TimerID                 timerDoorAnimate;
Uint32                      doorAnimateInterval;      // From script

// ----------------------------------------------------------------------------
//
// Play a door sound - pan based on door and player location
void gam_playDoorSound(int whichDoor)
// ----------------------------------------------------------------------------
{
  float angle;

  angle = static_cast<float>(atan2 (doorTrigger[whichDoor].topLeft.y - playerDroid.worldPos.y, doorTrigger[whichDoor].topLeft.x - playerDroid.worldPos.x) * 180 / 3.1415);

  evt_sendEvent (USER_EVENT_AUDIO, AUDIO_PLAY_SAMPLE, SND_DOOR, static_cast<int>(abs(playerDroid.worldPos.x - doorTrigger[whichDoor].topLeft.x)), false, glm::vec2{angle,0}, glm::vec2(), "");
}

// ----------------------------------------------------------------------------
//
// Process all the doors that are currently inUse
Uint32 gam_animateDoorCallback(Uint32 interval, void *param)
// ----------------------------------------------------------------------------
{
  int i;

  if (!doDoorAnimate)
    return interval;

  for (i = 0; i < numDoorsOnLevel; i++)
    {
      if (doorTrigger[i].inUse)
        {
          switch (doorTrigger[i].currentFrame)
            {
              case DOOR_ACROSS:doorTrigger[i].currentFrame = DOOR_ACROSS_OPEN_1;
                  gam_playDoorSound(i);
              break;

              case DOOR_ACROSS_OPEN_1:doorTrigger[i].currentFrame = DOOR_ACROSS_OPEN_2;
              break;

              case DOOR_ACROSS_OPEN_2:doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_OPENED:doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_CLOSING_1:doorTrigger[i].currentFrame = DOOR_ACROSS_OPENED;
              break;

              case DOOR_ACROSS_CLOSING_2:doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_1;
              break;

              case DOOR_UP:doorTrigger[i].currentFrame = DOOR_UP_OPEN_1;
                  gam_playDoorSound(i);
              break;

              case DOOR_UP_OPEN_1:doorTrigger[i].currentFrame = DOOR_UP_OPEN_2;
              break;

              case DOOR_UP_OPEN_2:doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_OPENED:doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_CLOSING_1:doorTrigger[i].currentFrame = DOOR_UP_OPENED;
              break;

              case DOOR_UP_CLOSING_2:doorTrigger[i].currentFrame = DOOR_UP_CLOSING_1;
              break;

            }    // end of switch statement
        }    // end of inUse is true text
      else
        {    // trigger is not in use
          switch (doorTrigger[i].currentFrame)
            {
              case DOOR_ACROSS_OPENED:
                    gam_playDoorSound(i);
                doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_1;
              break;

              case DOOR_ACROSS_CLOSING_1:doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSING_2;
              break;

              case DOOR_ACROSS_CLOSING_2:doorTrigger[i].currentFrame = DOOR_ACROSS_CLOSED;
              break;

              case DOOR_UP_OPENED:
                    gam_playDoorSound(i);
                doorTrigger[i].currentFrame = DOOR_UP_CLOSING_1;
              break;

              case DOOR_UP_CLOSING_1:doorTrigger[i].currentFrame = DOOR_UP_CLOSING_2;
              break;

              case DOOR_UP_CLOSING_2:doorTrigger[i].currentFrame = DOOR_UP_CLOSED;
              break;
            }
        }
      levelInfo.at (lvl_getCurrentLevelName ()).tiles[doorTrigger[i].mapPos] = doorTrigger[i].currentFrame;
    }
  return interval;
    }	// end of for each door loop



// ----------------------------------------------------------------------------
//
// Set the state of the healing tile timer
void gam_setDoorAnimateState(bool newState)
// ----------------------------------------------------------------------------
{
  doDoorAnimate = newState;
}

// ----------------------------------------------------------------------------
//
// Initiate the timer to animate the door tiles
//
// Pass in time in milliseconds
void gam_initDoorAnimateTimer(Uint32 interval)
// ----------------------------------------------------------------------------
{
	timerDoorAnimate = evt_registerTimer(interval, gam_animateDoorCallback, "Door animation");
}

// ----------------------------------------------------------------------------
//
// Check door trigger areas against sprite positions
void gam_doorCheckTriggerAreas(string levelName)
// ----------------------------------------------------------------------------
{
  int i = 0;
  int j = 0;

  if (0 == numDoorsOnLevel)
    return;		// no doors on this level to draw

  for (i = 0; i != numDoorsOnLevel; i++)
    {
      doorTrigger[i].inUse = (playerDroid.middlePosition.x > doorTrigger[i].topLeft.x)
                             && (playerDroid.middlePosition.y  > doorTrigger[i].topLeft.y)
                             && (playerDroid.middlePosition.x  < doorTrigger[i].botRight.x)
                             && (playerDroid.middlePosition.y  < doorTrigger[i].botRight.y);
    }
  //
  // now check all the enemy sprites against the doors
  //
  for (i = 0; i != numDoorsOnLevel; i++)
    {
      for (j = 0; j != levelInfo.at(levelName).numDroids; j++)
        {
          if (true == levelInfo.at(levelName).droid[j].isAlive)
            {
              if ((levelInfo.at(levelName).droid[j].worldPos.x + (SPRITE_SIZE / 2) > doorTrigger[i].topLeft.x) &&
                  (levelInfo.at(levelName).droid[j].worldPos.y + (SPRITE_SIZE / 2) > doorTrigger[i].topLeft.y) &&
                  (levelInfo.at(levelName).droid[j].worldPos.x + (SPRITE_SIZE / 2) < doorTrigger[i].botRight.x) &&
                  (levelInfo.at(levelName).droid[j].worldPos.y + (SPRITE_SIZE / 2) < doorTrigger[i].botRight.y))
                {
                  doorTrigger[i].inUse = true;
                }
            }
        }
    }
}


// ----------------------------------------------------------------------------
//
// Draw a debug quad for the door triggers
void gam_debugDoorTriggers()
//------------------------------------------------------------------------
{
  glm::vec2           textureSize;
  glm::vec2           scaleBy;
  static auto         haveSize = false;
  static glm::vec2    renderPosition;
  float texCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};

  if (!g_showDoorTriggers)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA);

  for (int i = 0; i < numDoorsOnLevel; i++)
    {

      textureSize = io_getTextureSize("whiteSquare");

      textureSize.x = (float)(doorTrigger[i].topLeft.x - doorTrigger[i].topRight.x);
      textureSize.y = (float)(doorTrigger[i].topLeft.y - doorTrigger[i].botLeft.y);

      renderPosition.x = (float)doorTrigger[i].topLeft.x;
      renderPosition.y = (float)doorTrigger[i].topLeft.y;

      if ((doorTrigger[i].topRight.x - doorTrigger[i].topLeft.x) > (doorTrigger[i].botLeft.y - doorTrigger[i].topLeft.y))
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

      if (doorTrigger[i].inUse)
          gl_draw2DQuad (renderPosition, textureSize, "quad3d", io_getTextureID ("redSquare"), glm::vec3{0, 0, 0}, glm::vec3{1.0, 0.0, 0.0}, texCoords);
      else
          gl_draw2DQuad (renderPosition, textureSize, "quad3d", io_getTextureID ("whiteSquare"), glm::vec3{0, 0, 0}, glm::vec3{1.0, 0.0, 0.0}, texCoords);
    }

    glDisable(GL_BLEND);
}


// ----------------------------------------------------------------------------
//
// Setup each door doorTrigger for this level
void gam_doorTriggerSetup()
// ----------------------------------------------------------------------------
{
  int 	    i;
  float 	sourceX = 0;
  float 	sourceY = 0;
  int 	    destX = 0;
  int 	    destY = 0;
  int 	    currentTile = 0;
  int 	    doorCounter = 0;
  cpVect	center;
  cpFloat	width, height;

  numDoorsOnLevel = 0;

  for (i = 0; i != levelInfo.at(lvl_getCurrentLevelName()).levelDimensions.x * levelInfo.at(lvl_getCurrentLevelName()).levelDimensions.y; i++)
    {
      currentTile = levelInfo.at(lvl_getCurrentLevelName()).tiles[(((int)sourceY * (levelInfo.at(lvl_getCurrentLevelName()).levelDimensions.x)) + (int)sourceX)];
      if (currentTile < 0)
        {
          con_print(CON_ERROR, true, "Tile found in gam_doorTriggerSetup is below 0. loop [ %i ]", i);
        }


      if	(
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
          doorTrigger[doorCounter].frameDelay = doorFrameDelay;
          doorTrigger[doorCounter].nextFrame = 0.0f;
          doorTrigger[doorCounter].mapPos = i;
          doorTrigger[doorCounter].mapX = destX;
          doorTrigger[doorCounter].mapY = destY;

          if ((DOOR_UP == currentTile) || (DOOR_UP_OPEN_1 == currentTile) || (DOOR_UP_OPEN_2 == currentTile) || (DOOR_UP_OPENED == currentTile) || (DOOR_UP_CLOSING_1 == currentTile) || (DOOR_UP_CLOSING_2 == currentTile) || (DOOR_UP_CLOSED == currentTile))
            {
              doorTrigger[doorCounter].currentFrame = DOOR_UP_CLOSED;	// reset to default frame

              doorTrigger[doorCounter].topLeft.x = (sourceX * TILE_SIZE) - (TILE_SIZE / 2); // - TILE_SIZE;
              doorTrigger[doorCounter].topLeft.y = (sourceY * TILE_SIZE); // - (TILE_SIZE );

              doorTrigger[doorCounter].topRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);
              doorTrigger[doorCounter].topRight.y = (sourceY * TILE_SIZE); // - (TILE_SIZE );

              doorTrigger[doorCounter].botLeft.x = (sourceX * TILE_SIZE) - (TILE_SIZE / 2); // - TILE_SIZE;
              doorTrigger[doorCounter].botLeft.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2);

              doorTrigger[doorCounter].botRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);
              doorTrigger[doorCounter].botRight.y = (sourceY * TILE_SIZE) + (TILE_SIZE) * 2;

              center.x = (sourceX * TILE_SIZE) + (TILE_SIZE / 2);
              center.y = (sourceY * TILE_SIZE) + (TILE_SIZE / 2);
              width = TILE_SIZE / 3;
              height = TILE_SIZE;
            }
          else
            {
              doorTrigger[doorCounter].currentFrame = DOOR_ACROSS;		// reset to default frame TODO Check frame type ( closed ?? )

              doorTrigger[doorCounter].topLeft.x = (sourceX * TILE_SIZE); // - (TILE_SIZE / 2);
              doorTrigger[doorCounter].topLeft.y = (sourceY * TILE_SIZE) - (TILE_SIZE / 2);

              doorTrigger[doorCounter].topRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2);
              doorTrigger[doorCounter].topRight.y = (sourceY * TILE_SIZE) - (TILE_SIZE / 2);

              doorTrigger[doorCounter].botLeft.x = (sourceX * TILE_SIZE); // - (TILE_SIZE / 2);
              doorTrigger[doorCounter].botLeft.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);

              doorTrigger[doorCounter].botRight.x = (sourceX * TILE_SIZE) + (TILE_SIZE * 2);
              doorTrigger[doorCounter].botRight.y = (sourceY * TILE_SIZE) + (TILE_SIZE * 2) + (TILE_SIZE / 2);

              center.x = (sourceX * TILE_SIZE) + (TILE_SIZE / 2);
              center.y = (sourceY * TILE_SIZE);
              width = TILE_SIZE;
              height = TILE_SIZE / 3;
            }
//          sys_createDoorSensor(doorCounter, width, height, center);
          doorCounter++;
        }	// end of if statement

      destX++;
      sourceX++;
      if (destX == (levelInfo.at(lvl_getCurrentLevelName ()).levelDimensions.x))
        {
          destX = 0;
          sourceX = 0;
          destY++;
          sourceY++;
        }
    }
  numDoorsOnLevel = doorCounter;
}

