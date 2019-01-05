#include "hdr/gui/gui_language.h"
#include "hdr/game/gam_database.h"
#include "hdr/game/gam_game.h"
#include "hdr/io/io_fileSystem.h"

vector<_dataBaseEntry> dataBaseEntry;

//-----------------------------------------------------------------------------------------------------
//
// Load a droid database file.
// Load into memory and then parse using LoadData with ini file
bool io_getDBDroidInfo ( const std::string &fileName )
//-----------------------------------------------------------------------------------------------------
{
	CSimpleIniA         iniFile;
	SI_Error            returnCode;
	PHYSFS_sint64       fileLength;
	_dataBaseEntry      tempDataBaseEntry;

	char *sourceText = nullptr;

	fileLength = io_getFileSize ((char *) fileName.c_str ());
	if ( fileLength == -1 )
	{
		con_print(CON_ERROR, true, "Unable to find database config file for [ %s ]", fileName.c_str());
		return false;
	}

	sourceText = (char *) malloc ( sizeof ( char ) * (fileLength + 1));
	if ( nullptr == sourceText )
	{
		con_print(CON_ERROR, true, "Memory error [ %s ]", fileName.c_str());
		return false;
	}

	if ( -1 == io_getFileIntoMemory ((char *) fileName.c_str (), sourceText ))
	{
		con_print(CON_ERROR, true, "Error loading file into memory [ %s ]", fileName.c_str());
		free ( sourceText );
		return false;
	}
	//
	// Make sure it's null terminated   // Need fileLength + 1 ??
	sourceText[fileLength] = '\0';

	iniFile.SetUnicode ();
	returnCode = iniFile.LoadData ( sourceText, static_cast<size_t>(fileLength));
	if ( returnCode != SI_OK )
	{
		con_print(CON_ERROR, true, "Error loading config file [ %s ]", fileName.c_str());
		free ( sourceText );
		return false;
	}

	tempDataBaseEntry.maxHealth = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "max_health", "50" ), nullptr, 10 ));

	if ( dataBaseEntry.empty ())    // 001 base droid stats
	{
		tempDataBaseEntry.maxSpeed = strtof ( iniFile.GetValue ( "mainSection", "max_speed", "3" ), nullptr );
		tempDataBaseEntry.maxSpeed += baseGameSpeed;

		tempDataBaseEntry.accelerate = strtof ( iniFile.GetValue ( "mainSection", "accelerate", "1.5" ), nullptr );
		tempDataBaseEntry.accelerate += baseGameSpeed;
	}
	else
	{
		//
		// All other droid speeds and acceleration are based on player droid values
		// Easy to change overall speed of the game
		//
		tempDataBaseEntry.maxSpeed = dataBaseEntry[0].maxSpeed + strtof ( iniFile.GetValue ( "mainSection", "max_speed", "1.5" ), nullptr );
		tempDataBaseEntry.accelerate = dataBaseEntry[0].accelerate + strtof ( iniFile.GetValue ( "mainSection", "accelerate", "1.5" ), nullptr );
	}

	tempDataBaseEntry.score = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "score", "50" ), nullptr, 10 ));
	tempDataBaseEntry.bounceDamage = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "bounce_damage", "50" ), nullptr, 10 ));
	tempDataBaseEntry.canShoot = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "can_shoot", "50" ), nullptr, 10 ));
	tempDataBaseEntry.rechargeTime = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "rechargeTime", "50" ), nullptr, 10 ));
	tempDataBaseEntry.bulletType = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "bullet_type", "50" ), nullptr, 10 ));
	//
	// Map the type of bullet to the sprite image for it
	//
	// Set to -1 if can_shoot == 0
	switch ( tempDataBaseEntry.bulletType )
	{
		case 0: // small double laser
			tempDataBaseEntry.bulletType = BULLET_TYPE_NORMAL;
			break;

		case 1: // Large single laser
			tempDataBaseEntry.bulletType = BULLET_TYPE_SINGLE;
			break;

		case 2: // Large double laser
			tempDataBaseEntry.bulletType = BULLET_TYPE_DOUBLE;
			break;

		case 3: // Disrupter
			tempDataBaseEntry.bulletType = BULLET_TYPE_DISRUPTER;
			break;
	}

	tempDataBaseEntry.chanceToShoot = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "chance_to_shoot", "50" ), nullptr, 10 ));
	tempDataBaseEntry.bulletDamage = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "bullet_damage", "50" ), nullptr, 10 ));
	tempDataBaseEntry.disrupterImmune = static_cast<bool>(static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "disrupter_immune", "0" ), nullptr, 10 )));
	tempDataBaseEntry.tokenCount = static_cast<int>(strtol ( iniFile.GetValue ( "mainSection", "token_count", "5" ), nullptr, 10 ));
	tempDataBaseEntry.height = iniFile.GetValue ( "mainSection", "height", "" );
	tempDataBaseEntry.weight = iniFile.GetValue ( "mainSection", "weight", "" );
	tempDataBaseEntry.dbImageFileName = iniFile.GetValue ( "mainSection", "dbImageFileName", "" );

	//
	// Read in key for text language
	//
	tempDataBaseEntry.description =     iniFile.GetValue ( "mainSection", "description", "" );
	tempDataBaseEntry.className =       iniFile.GetValue ( "mainSection", "className", "" );
	tempDataBaseEntry.drive =           iniFile.GetValue ( "mainSection", "drive", "" );
	tempDataBaseEntry.brain =           iniFile.GetValue ( "mainSection", "brain", "" );
	tempDataBaseEntry.weapon =          iniFile.GetValue ( "mainSection", "weapon", "" );
	tempDataBaseEntry.sensor1 =         iniFile.GetValue ( "mainSection", "sensor1", "" );
	tempDataBaseEntry.sensor2 =         iniFile.GetValue ( "mainSection", "sensor2", "" );
	tempDataBaseEntry.sensor3 =         iniFile.GetValue ( "mainSection", "sensor3", "" );
	tempDataBaseEntry.notes =           iniFile.GetValue ( "mainSection", "notes", "" );

	//
	// Now get the text string using that text key for the current language
	//
	tempDataBaseEntry.description =     gui_getString ( tempDataBaseEntry.description );
	tempDataBaseEntry.className =       gui_getString ( tempDataBaseEntry.className );
	tempDataBaseEntry.drive =           gui_getString ( tempDataBaseEntry.drive );
	tempDataBaseEntry.brain =           gui_getString ( tempDataBaseEntry.brain );
	tempDataBaseEntry.weapon =          gui_getString ( tempDataBaseEntry.weapon );
	tempDataBaseEntry.sensor1 =         gui_getString ( tempDataBaseEntry.sensor1 );
	tempDataBaseEntry.sensor2 =         gui_getString ( tempDataBaseEntry.sensor2 );
	tempDataBaseEntry.sensor3 =         gui_getString ( tempDataBaseEntry.sensor3 );
	tempDataBaseEntry.notes =           gui_getString ( tempDataBaseEntry.notes );
	//
	// Add in index pointers
//	tempDataBaseEntry.imageIndex = imageIndexCounter; // Start at first image and increment
//	imageIndexCounter++;

	//
	// Close the file and free resources
	free ( sourceText );

	dataBaseEntry.push_back(tempDataBaseEntry);

	return true;
}


//------------------------------------------------------------
//
// Get and build up information for droid database files
bool gam_getDBInformation()
//------------------------------------------------------------
{
	if (!io_getDBDroidInfo("001.txt"))
		return false;

	if (!io_getDBDroidInfo("123.txt"))
		return false;

	if (!io_getDBDroidInfo("139.txt"))
		return false;

	if (!io_getDBDroidInfo("247.txt"))
		return false;

	if (!io_getDBDroidInfo("249.txt"))
		return false;

	if (!io_getDBDroidInfo("296.txt"))
		return false;

	if (!io_getDBDroidInfo("302.txt"))
		return false;

	if (!io_getDBDroidInfo("329.txt"))
		return false;

	if (!io_getDBDroidInfo("420.txt"))
		return false;

	if (!io_getDBDroidInfo("476.txt"))
		return false;

	if (!io_getDBDroidInfo("493.txt"))
		return false;

	if (!io_getDBDroidInfo("516.txt"))
		return false;

	if (!io_getDBDroidInfo("571.txt"))
		return false;

	if (!io_getDBDroidInfo("598.txt"))
		return false;

	if (!io_getDBDroidInfo("614.txt"))
		return false;

	if (!io_getDBDroidInfo("615.txt"))
		return false;

	if (!io_getDBDroidInfo("629.txt"))
		return false;

	if (!io_getDBDroidInfo("711.txt"))
		return false;

	if (!io_getDBDroidInfo("742.txt"))
		return false;

	if (!io_getDBDroidInfo("751.txt"))
		return false;

	if (!io_getDBDroidInfo("821.txt"))
		return false;

	if (!io_getDBDroidInfo("834.txt"))
		return false;

	if (!io_getDBDroidInfo("883.txt"))
		return false;

	if (!io_getDBDroidInfo("999.txt"))
		return false;

	return true;
}


// evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_LOAD, 0, 0, 0, vec2(), vec2(), "hud.tga");
