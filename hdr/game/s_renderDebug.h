#pragma once

#include <string>

extern bool        g_debugShowWaypoints;
extern bool         g_debugShowPhysicsLines;

// Render the collision line segments
void gam_showLineSegments(std::string levelName);

// Render the waypoint segments
void gam_showWayPoints(std::string levelName);
