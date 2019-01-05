#include "hdr/game/gam_render.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_doors.h"
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_lifts.h"
#include "hdr/network/net_client.h"

vector<_levelMemory> levelMemoryPointers;
unordered_map<string, _levelStruct> levelInfo;
//unordered_map<string, _levelStruct>::const_iterator currentLevelItr;

int numTotalLevelsToLoad = 0;
bool allLevelsLoaded = false;

//----------------------------------------------------------------------------
//
// Structure holding all the level information
//
//-----------------------------------------------------------------------------
int currentLevel = -1;
vec2 drawOffset;
string currentLevelName;
int currentAlertLevel = ALERT_GREEN_TILE;

//-----------------------------------------------------------------------------------------------------
//
// Check if the level name is valid before accessing the map
inline bool lvl_isLevelValid ( string levelName )
//-----------------------------------------------------------------------------------------------------
{
	return levelInfo.count ( levelName ) > 0;
}

//-----------------------------------------------------------------------------------------------------
//
// Get a starting level - randomly chosen
std::string lvl_getStartingLevel ()
//-----------------------------------------------------------------------------------------------------
{
	return "Bridge";       // TODO: Make a random choice
}

//-----------------------------------------------------------------------------------------------------
//
// Return the string name of the current level
string lvl_getCurrentLevelName ()
//-----------------------------------------------------------------------------------------------------
{
	return currentLevelName;
}

//-----------------------------------------------------------------------------------------------------
//
// Add an entry to the levelMemoryPointers list
long lvl_addLevelInfo ( char *memPointer, const int levelLength, const string fileName, int deckNumber )
//-----------------------------------------------------------------------------------------------------
{
	_levelMemory tempMemory;
	long index;

	tempMemory.memPointer = memPointer;
	tempMemory.levelName = fileName;
	tempMemory.levelLength = levelLength;
	tempMemory.deckNumber = deckNumber;

	if ( SDL_LockMutex ( gameMutex ) == 0 )   // Lock it in case main thread is reading it
	{
		levelMemoryPointers.push_back ( tempMemory );
		index = levelMemoryPointers.size () - 1;
		SDL_UnlockMutex ( gameMutex );
		return index;
	}
	return -1;  // TODO: check if this happens if mutex fails
}


//-----------------------------------------------------------------------------------------------------
//
// Load a game level from a file
//
// Is called from the GAME thread
// Puts the level information into a queue of memory pointers
void lvl_loadLevelFromFile ( const string fileName, int deckNumber )
//-----------------------------------------------------------------------------------------------------
{
	char *levelBuffer = nullptr;
	int levelLength;
	long levelMemoryIndex;

//	con_print(CON_INFO, true, "Step 1 - load level file [ %s ]", fileName.c_str());

	levelLength = (int) io_getFileSize ( fileName.c_str ());

	if ( levelLength < 0 )
	{
		evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, LEVEL_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), fileName );
		return;
	}

//	con_print (CON_INFO, true, "Level size [ %i ]", levelLength);

	levelBuffer = (char *) malloc ( sizeof ( char ) * levelLength );

	if ( nullptr == levelBuffer )
	{
		evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, LEVEL_LOAD_MALLOC_ERROR, 0, 0, vec2 (), vec2 (), fileName );
		return;
	}

	if ( -1 == io_getFileIntoMemory ( fileName.c_str (), levelBuffer ))
	{
		evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, LEVEL_LOAD_MEMORY_ERROR, 0, 0, vec2 (), vec2 (), fileName );
		free ( levelBuffer );
		levelBuffer = nullptr;
		return;
	}

//	con_print (CON_INFO, true, "File is loaded into memory [ %i ]", levelBuffer);

	levelMemoryIndex = lvl_addLevelInfo ( levelBuffer, levelLength, fileName, deckNumber );

//	con_print (CON_INFO, true, "Index into array for [ %s ] is [ %i ]", fileName.c_str (), levelMemoryIndex);

//	con_print (CON_INFO, true, "Send event to main thread to load the level into memory structure.");

	evt_sendSDLEvent ( EVENT_TYPE_DO_LEVEL_LOAD, (int) levelMemoryIndex, 0 );
}

//-----------------------------------------------------------------------------------------
//
// Load the level into structure
// Called from main thread on user event
// Load level from memory pointed to by levelMemoryIndex - loaded in Game Thread
bool lvl_loadLevel ( intptr_t levelMemoryIndex )
//-----------------------------------------------------------------------------------------
{
	int checkVersion;
	SDL_RWops *fp;
	_levelStruct tempLevel = _levelStruct ();

	tempLevel.lineSegments.clear ();
	tempLevel.wayPoints.clear ();
	tempLevel.droidTypes.clear ();

	fp = SDL_RWFromMem ( levelMemoryPointers[(size_t) levelMemoryIndex].memPointer, levelMemoryPointers[(size_t) levelMemoryIndex].levelLength );

	//
	// Get a handle to the memory file
	if ( nullptr == fp )
	{
		evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, LEVEL_LOAD_ERROR_NOT_FOUND, 0, 0, vec2 (), vec2 (), levelMemoryPointers[(size_t) levelMemoryIndex].levelName );
		return false;
	}
	//
	// Check this version is ok to use
	//
	size_t returnCode = SDL_RWread ( fp, &checkVersion, sizeof ( checkVersion ), 1 );
	if ( 0 == returnCode )
	{
		evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_ERROR, LEVEL_LOAD_ERROR_FILESYSTEM, 0, 0, vec2 (), vec2 (), levelMemoryPointers[(size_t) levelMemoryIndex].levelName );
		SDL_RWclose ( fp );
		return false;
	}

	tempLevel.deckNumber = levelMemoryPointers[(size_t) levelMemoryIndex].deckNumber;

	if ( checkVersion != MAP_VERSION )
	{
		con_print ( CON_ERROR, true, "MAP_VERSION wrong. Reading incorrect level file." );
		return false;
	}
	tempLevel.mapVersion = 0;
	//
	// Read number variables
	//
	SDL_RWread ( fp, (void *) &tempLevel.numLineSegments, sizeof ( tempLevel.numLineSegments ), 1 );
	SDL_RWread ( fp, (void *) &tempLevel.numWaypoints, sizeof ( tempLevel.numWaypoints ), 1 );
	SDL_RWread ( fp, (void *) &tempLevel.numDroids, sizeof ( tempLevel.numDroids ), 1 );
	SDL_RWread ( fp, (void *) &tempLevel.numLifts, sizeof ( tempLevel.numLifts ), 1 );
	SDL_RWread ( fp, (void *) &tempLevel.levelDimensions, sizeof ( tempLevel.levelDimensions ), 1 );

	//
	// Line segments for physics collisions
	//
	_lineSegment tempSegment;
	size_t returnCodeLine;
	cpVect lineStart;
	cpVect lineFinish;

	for ( int i = 0; i != tempLevel.numLineSegments; i++ )
	{
		returnCodeLine = SDL_RWread ( fp, &tempSegment, sizeof ( _lineSegment ), 1 );
		if ( returnCodeLine != 1 )
			printf ( "Error: Reading line segment [ %i ]\n", i );

		lineStart = tempSegment.start;
		lineFinish = tempSegment.finish;

		lineStart.x += (drawOffset.x * 0.5) * TILE_SIZE;
		lineStart.y += (drawOffset.y * 0.5) * TILE_SIZE;

		lineStart.x -= (TILE_SIZE * 0.5);

		lineFinish.x += (drawOffset.x * 0.5) * TILE_SIZE;
		lineFinish.y += (drawOffset.y * 0.5) * TILE_SIZE;

		lineFinish.x -= (TILE_SIZE * 0.5);

		tempLevel.lineSegments.push_back ( lineStart );
		tempLevel.lineSegments.push_back ( lineFinish );
	}

	//
	// Waypoints for Droid patrol
	//
	cpVect tempWaypoint;

	for ( int i = 0; i != tempLevel.numWaypoints; i++ )
	{
		SDL_RWread ( fp, &tempWaypoint, sizeof ( tempWaypoint ), 1 );

		//
		// Why is this needed?  Pixel offset from screen res?
		//
		tempWaypoint.x += (drawOffset.x * 0.5) * TILE_SIZE;
		tempWaypoint.y += (drawOffset.y * 0.5) * TILE_SIZE;

		tempWaypoint.x -= (TILE_SIZE * 0.5);

		tempLevel.wayPoints.push_back ( tempWaypoint );
	}
	//
	// Load each droid type on the current level
	//
	int tempDroidType;
	for ( int i = 0; i != tempLevel.numDroids; i++ )
	{
		SDL_RWread ( fp, &tempDroidType, sizeof ( tempDroidType ), 1 );
		tempLevel.droidTypes.push_back ( tempDroidType );
	}
	//
	// Array holding tile types
	//
	int tempTile;
	for ( int i = 0; i != tempLevel.levelDimensions.x * tempLevel.levelDimensions.y; i++ )
	{
		SDL_RWread ( fp, &tempTile, sizeof ( tempTile ), 1 );
		tempLevel.tiles.push_back ( tempTile );
	}
	SDL_RWread ( fp, &tempLevel.levelName, sizeof ( tempLevel.levelName ), 1 );

	//
	// Finished - close the file
	//
	SDL_RWclose ( fp );

//	printf("Finished loading file from memory [ %s ]\n", tempLevel.levelName);

	if ( SDL_LockMutex ( levelMutex ) == 0 )
	{
		levelInfo.insert ( std::pair<string, _levelStruct> ( tempLevel.levelName, tempLevel ));
		SDL_UnlockMutex ( levelMutex );
	}
//	printf("Unable to lock levelMutex\n");

	free ( levelMemoryPointers[levelMemoryIndex].memPointer );

	evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_LEVEL_EXTRAS, 0, 0, 0, vec2 (), vec2 (), tempLevel.levelName );
	return true;
}

//---------------------------------------------------------
//
// Convert current tile information into padded array
void lvl_addPaddingToLevel ( const string levelName )
//---------------------------------------------------------
{
	std::vector<int> tempLevel;
	vec2 tempDimensions;
	int countY, countX, whichTile;
	int destX, destY;
	int tileIndexPtr;

	CHECK_LEVEL_NAME

	destX = static_cast<int>(drawOffset.x / 2);
	destY = static_cast<int>(drawOffset.y / 2);

	tempDimensions.x = static_cast<float>(levelInfo.at ( levelName ).levelDimensions.x);
	tempDimensions.y = static_cast<float>(levelInfo.at ( levelName ).levelDimensions.y);

	tempLevel.reserve ( static_cast<unsigned long>((levelInfo.at ( levelName ).levelDimensions.x + drawOffset.x) *
	                                               (levelInfo.at ( levelName ).levelDimensions.y + drawOffset.y)));

	tempLevel.assign ( static_cast<unsigned long>((levelInfo.at ( levelName ).levelDimensions.x + drawOffset.x) *
	                                              (levelInfo.at ( levelName ).levelDimensions.y + drawOffset.y)), 0 );

	for ( countY = 0; countY != levelInfo.at ( levelName ).levelDimensions.y; countY++ )
	{
		destX = static_cast<int>(drawOffset.x / 2);
		for ( countX = 0; countX != levelInfo.at ( levelName ).levelDimensions.x; countX++ )
		{
			tileIndexPtr = static_cast<int>((countY * levelInfo.at ( levelName ).levelDimensions.x) + countX);
			whichTile = levelInfo.at ( levelName ).tiles[tileIndexPtr];
			tempLevel[(destY * (tempDimensions.x + drawOffset.x)) + destX] = whichTile;
			destX++;
		}
		destY++;
	}

	tempDimensions.x += drawOffset.x;
	tempDimensions.y += drawOffset.y;

	levelInfo.at ( levelName ).levelDimensions.x = tempDimensions.x;
	levelInfo.at ( levelName ).levelDimensions.y = tempDimensions.y;

	levelInfo.at ( levelName ).tiles.clear ();

	levelInfo.at ( levelName ).tiles.reserve ( static_cast<unsigned long>(tempDimensions.x * tempDimensions.y));

	levelInfo.at ( levelName ).tiles.assign ( static_cast<unsigned long>(tempDimensions.x * tempDimensions.y), 0 );

	for ( int i = 0; i != (int) tempDimensions.x * (int) tempDimensions.y; i++ )
	{
		levelInfo.at ( levelName ).tiles[i] = tempLevel[i];
	}
	tempLevel.clear ();
}

//---------------------------------------------------------
//
// Load all the levels into memory
bool lvl_loadAllLevels ()
//---------------------------------------------------------
{
	string levelName;

	currentLevel = 0;

	drawOffset.x = winWidth / TILE_SIZE; // Padding to make tilePosX always positive
	drawOffset.y = winHeight / TILE_SIZE; // Padding to make tilePosY always positive

	for ( int i = 0; i < NUM_OF_LEVELS; i++ )
	{
		if ( 7 == i )
			currentLevel++;
		else
		{
			levelName = "newDeck" + std::to_string ( i ) + ".dat";

			evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_GAME_LOAD_LEVEL, i, 0, 0, vec2 (), vec2 (), levelName );

//			gam_initDroidValues ( currentLevel );

//			shipLevel[currentLevel].numEnemiesAlive = shipLevel[currentLevel].numDroids;

			currentLevel++;
		}
	}

	numTotalLevelsToLoad = currentLevel - 1;    // -1 to skip engine level

//	gam_setupLifts();

//	sys_createSolidWalls();
//	sys_setupPlayerPhysics();

	return true;
}

//-----------------------------------------------------------------------------------------------------
//
// Set the level slot in the map as an error
void lvl_setLevelError ( const string fileName )
//-----------------------------------------------------------------------------------------------------
{
	_levelStruct tempSet;

	tempSet.mapVersion = -1;
	levelInfo.insert ( std::pair<string, _levelStruct> ( fileName, tempSet ));
}

//-----------------------------------------------------------------------------------------------------
//
// Handle a level file error event
void lvl_handleLevelFileError ( const int errorCode, const string fileName )
//-----------------------------------------------------------------------------------------------------
{
	switch ( errorCode )
	{
		case LEVEL_LOAD_ERROR_NOT_FOUND:
			con_print ( CON_ERROR, true, "Could not find level file [ %s ]", fileName.c_str ());
			lvl_setLevelError ( fileName );
			break;

		case LEVEL_LOAD_ERROR_FILESYSTEM:
			con_print ( CON_ERROR, true, "Could not load level file from memory [ %s ]", fileName.c_str ());
			lvl_setLevelError ( fileName );
			break;

		case LEVEL_LOAD_MEMORY_ERROR:
			con_print ( CON_ERROR, true, "Memory error loading level [ %s ]", fileName.c_str ());
			lvl_setLevelError ( fileName );
			break;

		case LEVEL_LOAD_MALLOC_ERROR:
			con_print ( CON_ERROR, true, "MALLOC error loading level [ %s ]", fileName.c_str ());
			lvl_setLevelError ( fileName );
			break;

		case SIDEVIEW_FILE_NOT_FOUND:
			con_print ( CON_ERROR, true, "Could not find sideview data file [ %s ]", fileName.c_str ());
			break;

		case SIDEVIEW_MALLOC_ERROR:
			con_print ( CON_ERROR, true, "Memory error loading datafile [ %s ]", fileName.c_str ());
			break;

		case SIDEVIEW_LOAD_MEMORY_ERROR:
			con_print ( CON_ERROR, true, "Could not load datafile into memory [ %s ]", fileName.c_str ());
			break;

		case SIDEVIEW_LOAD_ERROR_FILESYSTEM:
			con_print ( CON_ERROR, true, "Error reading from file [ %s ]", fileName.c_str ());
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Pass in levelName to find
// Return Index into memory vector or .end() if not loaded
//
// Return the iterator for a level name
unordered_map<string, _levelStruct>::const_iterator lvl_getLevelIndex ( const string levelName )
//-----------------------------------------------------------------------------------------------------
{
	unordered_map<string, _levelStruct>::const_iterator levelItr;

	CHECK_LEVEL_NAME

	levelItr = levelInfo.find ( levelName );

	if ( levelItr != levelInfo.end ())
	{
		if ( levelItr->second.mapVersion == -1 )  // Not loaded properly
		{
			con_print ( CON_ERROR, true, "Trying to access invalid level [ %s ]", levelName.c_str ());
			return levelInfo.end ();
		}
		return levelItr;
	}
	con_print ( CON_ERROR, true, "Unable to find levelName [ %s ] - [ %s ]", levelName.c_str (), __func__ );

	return levelInfo.end ();   // Not found
}

//-----------------------------------------------------------------------------------------------------
//
// Return the levelName from the passed in deckNumber
std::string lvl_returnLevelNameFromDeck ( int deckNumber )
//-----------------------------------------------------------------------------------------------------
{
	for ( auto levelItr : levelInfo )
	{
		if ( deckNumber == levelItr.second.deckNumber )
		{
			return levelItr.second.levelName;
		}
	}
	return "Not Found";
}

//-----------------------------------------------------------------------------------------------------
//
// Debug - check that the level has loaded ok - display some data
bool lvl_checkLoad ( const string &levelName, int levelNumber )
//-----------------------------------------------------------------------------------------------------
{
	unordered_map<string, _levelStruct>::const_iterator levelItr;

	levelItr = lvl_getLevelIndex ( levelName );
	if ( levelItr == levelInfo.end ())
	{
		con_print ( CON_ERROR, true, "Error finding index for [ %s ]", levelName.c_str ());
		return false;
	}

	if ( levelItr->second.mapVersion != -1 )
	{
		con_print ( CON_INFO, true, "Level name [ %i ] is [ %s ]", levelNumber, levelItr->second.levelName );
		con_print ( CON_INFO, true, "Num Droids [ %i ]", levelItr->second.numDroids );
		con_print ( CON_INFO, true, "num lifts [ %i ]", levelInfo.at ( levelName ).numLifts );
		return true;
	}
	else
		con_print ( CON_ERROR, true, "Level data is invalid or not loaded [ %s ]", levelName.c_str ());

	return false;
}

//-----------------------------------------------------------------------------------------
//
// Return the deckNumber for the passed in level string
int lvl_getDeckNumber ( const string levelName )
//-----------------------------------------------------------------------------------------
{
	unordered_map<string, _levelStruct>::const_iterator levelItr;

	CHECK_LEVEL_NAME

	levelItr = levelInfo.find ( levelName );

	if ( levelItr != levelInfo.end ())
	{
		if ( levelItr->second.mapVersion == -1 )  // Not loaded properly
		{
			con_print ( CON_ERROR, true, "Trying to access invalid level [ %s ]", levelName.c_str ());
			return -1;
		}
		return levelItr->second.deckNumber;
	}
	con_print ( CON_ERROR, true, "Unable to find levelName [ %s ] - [ %s ]", levelName.c_str (), __func__ );

	return 1;   // Not found
}

//-----------------------------------------------------------------------------------------
//
// List all the levels loaded
void lvl_showLevelsLoaded ()
//-----------------------------------------------------------------------------------------
{
	int levelCount = 0;
	for ( const auto &levelItr : levelInfo )
	{
//		con_print(CON_INFO, true, "Level index [ %i ]", levelCount++);
		lvl_checkLoad ( levelItr.second.levelName, levelCount++ );
		if ( levelCount == 7 )
			levelCount++;
	}
}

//-----------------------------------------------------------------------------------------------------
//
// Change to a new level
void lvl_changeToLevel ( const string levelName, bool startOnLift, int whichLift )
//-----------------------------------------------------------------------------------------------------
{
	string previousLevelName;

	previousLevelName = currentLevelName;

	currentLevelName = levelName;
	gam_findHealingTiles ( levelName );
	gam_getLiftPositions ( levelName );
	drd_setupLevel ( levelName );

	if ( startOnLift )
	{
		playerDroid.worldPos = gam_getLiftworldPosition ( levelName, whichLift );
		//
		// Center on the lift tile
		playerDroid.worldPos.x += TILE_SIZE * 0.5f;
		playerDroid.worldPos.y += TILE_SIZE * 0.5f;
		sys_setPlayerPhysicsPosition ( playerDroid.worldPos );
	}
	else
	{
		// Get random starting position from waypoints
	}

	sys_destroyPhysicObjects ( currentLevelName);
	sys_createSolidWalls ( levelName );
	gam_doorTriggerSetup();

	if (previousLevelName != currentLevelName)
		io_removeTextureFromMap( previousLevelName );

	gam_resetLevelInit();

//	net_sendCurrentLevel(levelName);
}

