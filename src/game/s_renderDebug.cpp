#include <hdr/game/s_levels.h>
#include "hdr/game/s_renderDebug.h"


bool        g_debugShowWaypoints = true;
bool         g_debugShowPhysicsLines = true;

//--------------------------------------------------------------------------
//
// Render the collision line segments
void gam_showLineSegments()
//--------------------------------------------------------------------------
{
	glm::vec3   lineStart;
	glm::vec3   lineFinish;
	cpVect      cp_lineStart;
	cpVect      cp_lineFinish;

	int         indexCount = 0;

	if (!g_debugShowPhysicsLines)
		return;

	for ( int i = 0; i != levelInfo.at(currentLevelName).lineSegments.size() / 2; i++ ) // line is from 2 points
	{
		cp_lineStart = levelInfo.at(currentLevelName).lineSegments[indexCount++];
		cp_lineFinish = levelInfo.at(currentLevelName).lineSegments[indexCount++];

		lineStart.x = static_cast<float>(cp_lineStart.x);
		lineStart.y = static_cast<float>(cp_lineStart.y);
		lineStart.z = 0.0f;

		lineFinish.x = static_cast<float>(cp_lineFinish.x);
		lineFinish.y = static_cast<float>(cp_lineFinish.y);
		lineFinish.z = 0.0f;

		gl_drawLine (lineStart, lineFinish, "colorLine", vec4 (1, 1, 1, 1));
	}
}

//--------------------------------------------------------------------------
//
// Render the waypoint segments
void gam_showWayPoints()
//--------------------------------------------------------------------------
{
	int                 indexCount;
	glm::vec3           lineStart;
	glm::vec3           lineFinish;
	_lineSegment		tempLine;

	if (!g_debugShowWaypoints)
		return;

	indexCount = 0;

	for ( auto it: levelInfo.at(currentLevelName).wayPoints)
//	for ( auto it = levelInfo.at(currentLevelName).wayPoints.begin(); it != levelInfo.at(currentLevelName).wayPoints.end(); ++it)
	{
		tempLine.start = levelInfo.at(currentLevelName).wayPoints[indexCount];

		if (indexCount + 1 < levelInfo.at(currentLevelName).numWaypoints)
			tempLine.finish = levelInfo.at(currentLevelName).wayPoints[indexCount + 1];
		else
			tempLine.finish = levelInfo.at(currentLevelName).wayPoints[0];

		lineStart.x = static_cast<float>(tempLine.start.x);
		lineStart.y = static_cast<float>(tempLine.start.y);
		lineStart.z = 0.0f;

		lineFinish.x = static_cast<float>(tempLine.finish.x);
		lineFinish.y = static_cast<float>(tempLine.finish.y);
		lineFinish.z = 0.0f;

		gl_drawLine (lineStart, lineFinish, "colorLine", vec4 (1, 1, 1, 1));

		indexCount++;
	}
}
