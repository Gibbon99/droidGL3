#pragma once

#include "hdr/system/sys_main.h"

#define NODE_UP            0x0
#define NODE_DOWN        0x1
#define NODE_LEFT        0x2
#define NODE_RIGHT        0x4

typedef struct
{
	bool walkable;
	cpVect tileLocation;
	int parent;
	int g_movementCost;
	int h_estMoveCost;
	int f_score;
} _pathNode;

typedef struct
{
	int whichDroid;        // Who owns this path
	string whichLevel;        // Which level is this droid on
	bool pathReady;
	bool wayPointsReady;
	bool inUse;
	cpVect startTile;
	cpVect destTile;
	int currentNodePtrClosedList;
	vector<_pathNode> openNodes;
	vector<_pathNode> closedNodes;
	vector<_pathNode> foundPath;
	vector<cpVect> wayPoints;
} _nodeList;

extern vector<_nodeList>        path;
extern int                      numAStarPaths;        // Set from Script
extern int                     debugAStarIndex;

// Allocate new structure if required or reuse existing one
// Start thread process
int gam_AStarRequestNewPath ( cpVect start, cpVect destination, int whichDroid, const string whichLevel );

// Check if the current thread pathfinding is finished
bool gam_AStarIsPathReady ( int whichPath );

// Clear coord memory and flag ready for reuse
void gam_AStarRemovePath ( int whichPath );

// Return how many waypoints there are
int gam_AStarGetNumWaypoints ( int whichPath );

// show waypoint lines
void gam_AStarDebugWayPoints ( int whichPath );

// Show path structure information
void gam_AStarDebugPaths ();

// Is a tile considered 'solid' or not
bool gam_AStarIsTileSolid ( int tileIndex, int whichPath );

// Remove any running threads
void gam_resetAllPaths ();

// Process all the currently active paths
void gam_AStarProcessPaths ( std::string levelName );

