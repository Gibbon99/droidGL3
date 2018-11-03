#include <hdr/game/s_levels.h>
#include <hdr/game/s_player.h>
#include <hdr/system/sys_utils.h>
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
			if ( levelInfo.at (levelName).droid[index].currentFrame > 8)
				levelInfo.at (levelName).droid[index].currentFrame = 0;
		}
	}
}
//------------------------------------------------------------------------------
//
// Render the droids for this level
void drd_renderThisLevel( const string levelName)
//------------------------------------------------------------------------------
{
    float       drawPositionX;
    float       drawPositionY;

	if (levelInfo.at(levelName).droid.empty ())
		return;     // Nothing to render

	for (int index = 0; index != levelInfo.at(levelName).numDroids; index++)
	{

//	    if (sys_visibleOnScreen(levelInfo.at(levelName).droid[index].worldPos, 32))
	    {

            drawPositionX = levelInfo.at(levelName).droid[index].worldPos.x - playerDroid.worldPos.x;
            drawPositionY = levelInfo.at(levelName).droid[index].worldPos.y - playerDroid.worldPos.y;

            // 280 is the left visible edge - WIDTH 240
            // 520 is the right visible edge

            // 200 is the bottom visible edge - HEIGHT 200
            // 400 is the top visible edge

            drawPositionX += 240;
            drawPositionY += 200;
//            drawPositionY = levelInfo.at(levelName).levelDimensions.y - drawPositionY;
//drawPosition.y = -drawPosition.y;

            if (0 == index)
            {
                printf("Droid [ %i ] Player [ %3.3f %3.3f ] draw [ %3.3f %3.3f ] world [ %3.3f %3.3f ]\n", index,
                    playerDroid.worldPos.x, playerDroid.worldPos.y,
                    drawPositionX, drawPositionY,
                       levelInfo.at(levelName).droid[index].worldPos.x,
                       levelInfo.at(levelName).droid[index].worldPos.y);
            }


            gl_renderSprite(
                    levelInfo.at(levelName).droid[index].spriteName,
                    glm::vec2{drawPositionX, drawPositionY},
                    levelInfo.at(levelName).droid[index].currentFrame,
                    glm::vec3{1, 1, 0});

//		levelInfo.at(levelName).droid[index].worldPos.y -= 0.1f;
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

		levelInfo.at(levelName).droid.push_back(tempDroid);
	}
}