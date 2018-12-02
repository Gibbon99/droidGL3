#pragma once

#include <unordered_map>
#include <hdr/game/gam_droids.h>
#include "hdr/system/sys_main.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/game/gam_lifts.h"

#define MAP_VERSION             115

#define CHECK_LEVEL_NAME    	if (!lvl_isLevelValid(levelName))    \
								{ con_print(CON_ERROR, true, "No valid level name [ %s ] - [ %s ]", levelName, __func__); \
								}

#define MAX_NUM_SPRITES         20   // maximum number of sprites per level
#define MAX_NUM_OF_WAYPOINTS    100
#define NUM_OF_LEVELS           21
#define MAX_NUM_OF_LIFTS        3

#define TILE_SIZE               32
#define DROID_SIZE              24

#define NO_PASS_TILE            40
#define LIFT_TILE               (NO_PASS_TILE + 1)
#define HEALING_TILE            (LIFT_TILE + 4)

#define ALERT_GREEN_TILE        4
#define ALERT_YELLOW_TILE       5
#define ALERT_RED_TILE          6

#define TERMINAL_TOP            51
#define TERMINAL_BOTTOM         52
#define TERMINAL_RIGHT          53
#define TERMINAL_LEFT           54

#define LEVEL_LOAD_ERROR_NOT_FOUND    0x01
#define LEVEL_LOAD_ERROR_FILESYSTEM   0x02
#define LEVEL_LOAD_MEMORY_ERROR       0x03
#define LEVEL_LOAD_MALLOC_ERROR       0x04

typedef struct
{
	char    *memPointer;
	int     levelLength;
	string  levelName;
} _levelMemory;

//-----------------------------------------------------------------------------
//
// Loaded from disk per level
//
//-----------------------------------------------------------------------------

typedef struct {
	cpVect start;
	cpVect finish;
} _lineSegment;


struct _levelStruct {
	int                     mapVersion;
	int                     numLineSegments;
	int                     numWaypoints;
	int                     numDroids;
	int                     numLifts;
	cpVect                  levelDimensions;
//	vector<_lineSegment>    lineSegments;
	vector<cpVect>          lineSegments;
	vector<cpVect>          wayPoints;
	vector<int>             tiles;
	vector<int>             droidTypes;
	char                    levelName[20];
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	// Above here is loaded from file
	//
	int                     numEnemiesAlive;
	vector<_liftBasic>      lifts;
	vector<_droid>          droid;
};

//extern int                  currentLevel;
//extern string               currentLevelName;
extern unordered_map<string, _levelStruct>::const_iterator currentLevelItr;

extern bool     allLevelsLoaded;
extern int      currentAlertLevel;

extern unordered_map <string, _levelStruct> levelInfo;

//-----------------------------------------------------------------------------
//
// Function declarations
//
//-----------------------------------------------------------------------------

// Return the iterator for a level name
unordered_map<string, _levelStruct>::const_iterator lvl_getLevelIndex ( const string fileName );


// Handle a level file error event
void lvl_handleLevelFileError ( const int errorCode, const string fileName );

// Add an entry to the levelMemoryPointers list
long lvl_addLevelInfo ( char *memPointer, const int levelLength, const string fileName );

void lvl_loadLevelFromFile ( const string fileName );

// Load the level into structure
bool lvl_loadLevel ( intptr_t levelMemoryIndex );

// Load all the levels into memory
bool lvl_loadAllLevels ();

bool lvl_checkLoad ( string levelName );

// Convert current tile information into padded array
void lvl_addPaddingToLevel(string levelName);

// List all the levels loaded
void lvl_showLevelsLoaded ();

// Change to a new level
void lvl_changeToLevel ( const string levelName );

// Return the string name of the current level
string lvl_getCurrentLevelName ();

// Check if the level name is valid before accessing the map
inline bool lvl_isLevelValid ( string levelName );