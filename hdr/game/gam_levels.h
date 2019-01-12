#pragma once

#include <unordered_map>
#include <string>
#include "hdr/libGL/chipmunk/chipmunk.h"
#include "hdr/game/gam_droids.h"
#include "hdr/system/sys_main.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/game/gam_lifts.h"
#include "hdr/game/gam_healing.h"

#define MAP_VERSION             115

#define CHECK_LEVEL_NAME    	if (!lvl_isLevelValid(levelName))    \
								{ con_print(CON_ERROR, true, "No valid level name [ %s ] - [ %s ]", levelName.c_str(), __func__); \
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

typedef struct
{
	char            *memPointer;
	int             levelLength;
	std::string     levelName;
	int             deckNumber;
} _levelMemory;

//-----------------------------------------------------------------------------
//
// Loaded from disk per level
//
//-----------------------------------------------------------------------------

typedef struct
{
	cpVect start;
	cpVect finish;
} _lineSegment;

//-----------------------------------------------------------------------------
//
// information needed for one lift
//
//-----------------------------------------------------------------------------

typedef struct
{
	int tunnel;
	int posX;
	int posY;
} _liftBasic;

//-----------------------------------------------------------------------------
//
// structure to hold information for healing tiles
//
//-----------------------------------------------------------------------------

typedef struct
{
	int         pos;                // position in array
	int         currentFrame;       // which frame are we on
	float       frameDelay;         // animation counter
	cpVect      worldPos;           // Position in world coords
} _basicHealing;

typedef struct _droid
{
	bool 			isAlive;
	int 			droidType;
	int 			currentHealth;
	int 			wayPointIndex;
	int 			wayPointDirection;
	std::string 	spriteName;
	int 			currentFrame;
	float 			frameDelay;
	glm::vec2   	renderOffset;

	float 			currentSpeed;
	float           acceleration;

	cpVect 			worldPos;
	cpVect          serverWorldPos;             // Where the server thinks we are
	cpVect          viewWorldPos;
	cpVect          middlePosition;             // TODO set middle position for droids

	cpVect          destinationCoords;          // This is the line segment end point
	cpVect          destDirection;              // Which way is the droid heading
	cpVect          velocity;                   // Current speed
	cpVect          serverVelocity;             // Current speed according to the server

	int             overTile;                   // which tile is the droid on
	int             currentMode;                // What is the droid doing; transfer, healing, terminal etc

	cpBody          *body;                      // Used for physics and collisions
	cpShape         *shape;
	float           mass;                       // Used for collision response

	bool            ignoreCollisions;           // Ignoring collisions for the time period
	bool            isExploding;

	int             targetIndex;                // Which droid shot this droid
	bool            beenShotByPlayer;
	float           beenShotCountdown;

	int             collisionCount;             // how many collision have occured to ignore them
	bool            hasCollided;
	int             collidedWith;               // Who did the droid hit
	float ignoreCollisionsCounter;

	int             playerDroidTypeDBIndex;     // What sort of droid is the player
	int             droidTransferedIntoIndex;

	bool            inTransferMode;

	float chanceToShoot;

	bool visibleToPlayer;

	//
// Weapon
	bool weaponCanFire;
	float weaponDelay;

	bool witnessShooting;
	bool witnessTransfer;

	float witnessShootingCountDown;
	float witnessTransferCountDown;

	// AI variables
	int ai_currentState;
	int ai_moveMode;

	// Pathfinding
	int aStarPathIndex;            // Index into which path to use
	int numberPathNodes;
	int currentAStarIndex;         // Index into aStarWaypoints array
	bool aStarDestinationFound;
	bool aStarInitDone;
	cpVect previousWaypoints;

	bool onFleeTile;
	bool foundFleeTile;

	bool isNotPatrolling;        // Used to enter resume branch
	bool onResumeDestTile;
	bool destSet;                // Does the droid have a destination

	bool onHealingTile;
	bool foundHealingTile;
	/*
cpVect acceleration;

cpVect screenPos;


//
// States
bool isStopped;
bool isAlive;

// Animation
float currentFrameDelay;
int currentFrame;



// AI variables
int ai_currentState;
int ai_nextState;
float ai_noActionCounter;
float ai_noActionCount;

cpVect originPosition;     // Remember this to return to
bool foundOriginPath;
bool returnToOrigin;
bool foundOriginPosition;

float healingCount;
bool foundWPTile;        // Go here after healing

cpVect destinationTile;        // Where is the droid heading
bool resumeDestFound;        // Set from running thread - aStar found destination
bool resumeThreadStarted;    // Is the thread started and running

*/

} _droid;

typedef struct _levelStruct{
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
	int                     deckNumber;         // Use this to reference by a number
	int                     numEnemiesAlive;
	bool                    containsClient;
	vector<_liftBasic>      lifts;
	vector<_droid>          droid;
	vector<_basicHealing>   healing;
	bool                    wallPhysicsCreated = false;
	bool                    droidPhysicsCreated = false;
	vector<_physicObject>   solidWalls;
	cpShapeFilter           deckShapeFilter;
	cpBitmask               deckCategory;
	cpBitmask               deckMask;
} _levelStruct;

//extern int                  currentLevel;
//extern string               currentLevelName;
extern unordered_map<string, _levelStruct>::const_iterator currentLevelItr;

extern int      numTotalLevelsToLoad;
extern bool     allLevelsLoaded;
extern int      currentAlertLevel;
extern string   currentLevelName;

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

void lvl_loadLevelFromFile ( const string fileName, int deckNumber );

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
void lvl_changeToLevel ( const string levelName, bool startOnLift, int whichLift );

// Return the string name of the current level
string lvl_getCurrentLevelName ();

// Check if the level name is valid before accessing the map
inline bool lvl_isLevelValid ( string levelName );

// Get a starting level - randomly chosen
std::string lvl_getStartingLevel ();

// Return the deckNumber for the passed in level string
int lvl_getDeckNumber ( std::string levelName );

// Return the levelName from the passed in deckNumber
std::string lvl_returnLevelNameFromDeck(int deckNumber);
