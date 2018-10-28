#include <hdr/game/s_levels.h>
#include "hdr/opengl/gl_renderSprite.h"
#include "s_droids.h"

//------------------------------------------------------------------------------
//
// Render the droids for this level
void drd_renderThisLevel( const string levelName)
//------------------------------------------------------------------------------
{
	if (levelInfo.at(levelName).droid.empty ())
		return;     // Nothing to render

	for (int index = 0; index != levelInfo.at(levelName).numDroids; index++)
	{
		gl_renderSprite(
				levelInfo.at(levelName).droid[index].spriteName,
				glm::vec2{levelInfo.at(levelName).droid[index].worldPos.x, levelInfo.at(levelName).droid[index].worldPos.y},
				glm::vec3{1,1,1});
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

		levelInfo.at(levelName).droid.push_back(tempDroid);
	}
}