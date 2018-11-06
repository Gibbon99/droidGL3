#include <hdr/game/s_levels.h>
#include <hdr/game/s_player.h>
#include <hdr/system/sys_utils.h>
#include <hdr/game/s_droidAIPatrol.h>
#include "hdr/opengl/gl_renderSprite.h"
#include "s_droids.h"

//------------------------------------------------------------------------------
//
// Animate the droid
void drd_animateThisLevel(const string levelName)
//------------------------------------------------------------------------------
{
	for (int index = 0; index != levelInfo.at(levelName).numDroids; index++)
	{
		levelInfo.at(levelName).droid[index].frameDelay += 1.0f / 3.0f;

		if ( levelInfo.at (levelName).droid[index].frameDelay > 1.0f)
		{
			levelInfo.at (levelName).droid[index].frameDelay = 0.0f;
			levelInfo.at (levelName).droid[index].currentFrame++;
			if ( levelInfo.at (levelName).droid[index].currentFrame == sprites.at(levelInfo.at (levelName).droid[index].spriteName).numberOfFrames)
				levelInfo.at (levelName).droid[index].currentFrame = 0;
		}
	}
}
//------------------------------------------------------------------------------
//
// Render the droids for this level
void drd_renderThisLevel( const string levelName, float interpolate)
//------------------------------------------------------------------------------
{
    cpVect       drawPosition;
//    float       drawPositionY;

	if (levelInfo.at(levelName).droid.empty ())
		return;     // Nothing to render

	for (int index = 0; index != levelInfo.at(levelName).numDroids; index++)
	{

//	    if (sys_visibleOnScreen(levelInfo.at(levelName).droid[index].worldPos, 32))
	    {

			drawPosition = cpvadd(levelInfo.at (levelName).droid[index].worldPos, cpvmult(levelInfo.at (levelName).droid[index].velocity, interpolate));

			drawPosition.y = (int)drawPosition.y;   // Remove the fraction portion to stop blurring in Y direction

            gl_renderSprite(
                    levelInfo.at(levelName).droid[index].spriteName,
                    glm::vec2{drawPosition.x, drawPosition.y},
                    levelInfo.at(levelName).droid[index].currentFrame,
                    glm::vec3{1, 1, 0});
        }
	}
}

//------------------------------------------------------------------------------
//
// Setup droid information for this level
void drd_setupLevel(string levelName)
//------------------------------------------------------------------------------
{
	_droid      tempDroid;

	if (!levelInfo.at(levelName).droid.empty())
		return;

	levelInfo.at(levelName).droid.reserve(static_cast<unsigned long>(levelInfo.at (levelName).numDroids));

	for (int i = 0; i != levelInfo.at(levelName).numDroids; i++)
	{
		tempDroid.isAlive = true;
		tempDroid.droidType = levelInfo.at(levelName).droidTypes[i];
		tempDroid.wayPointIndex = rand() % (levelInfo.at(levelName).numWaypoints - 1);

		tempDroid.worldPos = levelInfo.at(levelName).wayPoints[i];

		tempDroid.spriteName = gl_getSpriteName(tempDroid.droidType);
		tempDroid.currentFrame = 0;
		tempDroid.frameDelay = 0.0f;
		tempDroid.wayPointDirection = WAYPOINT_UP;
		tempDroid.destinationCoords = levelInfo.at(levelName).wayPoints[tempDroid.wayPointIndex];
		tempDroid.currentSpeed = 1.2f;

		levelInfo.at(levelName).droid.push_back(tempDroid);
	}
}