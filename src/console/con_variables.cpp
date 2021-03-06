#include <hdr/gui/gui_terminal.h>
#include "hdr/game/gam_lineOfSight.h"
#include "hdr/game/gam_pathFind.h"
#include "hdr/game/gam_droidAI.h"
#include "hdr/game/gam_game.h"
#include "hdr/gui/gui_scrollBox.h"
#include "hdr/gui/gui_button.h"
#include "hdr/io/io_mouse.h"
#include "hdr/gui/gui_language.h"
#include "hdr/game/gam_lightCaster.h"
#include "hdr/game/gam_render.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_doors.h"
#include "hdr/game/gam_physics.h"
#include "hdr/gui/gui_main.h"
#include "hdr/game/gam_renderDebug.h"
#include "hdr/console/con_console.h"
#include "hdr/system/sys_audio.h"
#include "hdr/opengl/gl_opengl.h"
#include "hdr/game/gam_hud.h"
#include "hdr/network/net_server.h"
#include "hdr/game/gam_physicsPlayer.h"

cpVect vect1;

_hostScriptFunctions hostVariables[] =
{
	{"bool quitProgram",			        &quitProgram},
	{"int winWidth",                        &winWidth},
	{"int winHeight",                       &winHeight},
	{"int currentMode",                     &currentMode},
	{"int as_audioVolume",                  &as_audioVolume},
	{"int as_numAudioSamples",              &as_numAudioSamples},
	{"float nearPlane",                     &nearPlane},
	{"float farPlane",                      &farPlane},
	{"float cameraDistance",                &cameraDistance},
	{"int g_playFieldSize",                 &g_playFieldSize},
	{"cpVect vect1",                        &vect1},
	{"int healingAnimateInterval",          &healingAnimateInterval},
    {"int doorAnimateInterval",             &doorAnimateInterval},
	{"float shipDamping",                   &shipDamping},
	{"float collisionSlop",                 &collisionSlop},
	{"float wallFriction",                  &wallFriction},
	{"float wallRadius",                    &wallRadius},
	{"float playerRadius",                  &playerRadius},
	{"float playerFriction",                &playerFriction},
	{"float playerElastic",                 &playerElastic},
	{"float gravity",                       &gravity},
	{"int maxNumClients",                   &maxNumClients},
	{"int currentGUIScreen",                &currentGUIScreen},
	{"int currentObjectSelected",           &currentObjectSelected},
	{"int focusAnimateIntervalValue",       &focusAnimateIntervalValue},
	{"int getMousePositionInterval",        &getMousePositionInterval},
	{"int currentLanguage",                 &currentLanguage},

	{"int introStartX",                     &introStartX},
	{"int introStartY",                     &introStartY},
	{"int introWidth",                      &introWidth},
	{"int introHeight",                     &introHeight},
	{"int introBackRed",                    &introBackRed},
	{"int introBackGreen",                  &introBackGreen},
	{"int introBackBlue",                   &introBackBlue},
	{"int introBackAlpha",                  &introBackAlpha},
	{"int introFontRed",                    &introFontRed},
	{"int introFontGreen",                  &introFontGreen},
	{"int introFontBlue",                   &introFontBlue},
	{"int introFontAlpha",                  &introFontAlpha},

    {"int dbAnimateSpeed",                  &dbAnimateSpeed},

	{"float droidBeenShotValue",            &droidBeenShotValue},
	{"int collisionExplosionDamage",        &collisionExplosionDamage},
	{"int collisionDamageInflicted",        &collisionDamageInflicted},

	{"float witnessShootValue",             &witnessShootValue},
	{"float witnessTransferValue",          &witnessTransferValue},
	{"float ai_beenShot",                   &ai_beenShot},
	{"float ai_healthAmount",               &ai_healthAmount},
	{"float ai_playerVisible",              &ai_playerVisible},
	{"float ai_witnessShoot",               &ai_witnessShoot},
	{"float ai_witnessTransfer",            &ai_witnessTransfer},
	{"float ai_greenFactor",                &ai_greenFactor},
	{"float ai_yellowFactor",               &ai_yellowFactor},
	{"float ai_redFactor",                  &ai_redFactor},

	{"int numAStarPaths",                   &numAStarPaths},
	{"float wayPointDestinationSize",       &wayPointDestinationSize},
	{"float badHealthFactor",               &badHealthFactor},

	{"float g_gamma",                       &g_gamma},
	{"bool g_debugShowWaypoints",           &g_debugShowWaypoints},
	{"bool g_debugShowPhysicsLines",        &g_debugShowPhysicsLines},
	{"bool g_debugShowHullCircle",          &g_debugShowHullCircle},
	{"bool g_debugShowHullLines",           &g_debugShowHullLines},
	{"bool g_showDoorTriggers",             &g_showDoorTriggers},
	{"bool drawHUD",                        &drawHUD},
	{"float g_scaleViewBy",                 &g_scaleViewBy},
	{"bool allDroidsVisible",               &allDroidsVisible},
	{"float g_LineOfSightStep",             &g_LineOfSightStep},
	{"float bulletMass",                    &bulletMass},
	{"float bulletTravelSpeed",             &bulletTravelSpeed},
    {"int bulletAnimateInterval",         &bulletAnimateInterval},

    {"float aStarPathInterval",             &aStarPathInterval},

	{"float baseGameSpeed",                 &baseGameSpeed},

    {"int dbStartX",                        &dbStartX},
    {"int dbStartY",                        &dbStartY},
    {"int dbWidth",                         &dbWidth},
    {"int dbHeight",                        &dbHeight},
    {"int dbBackRed",                       &dbBackRed},
    {"int dbBackGreen",                     &dbBackGreen},
    {"int dbBackBlue",                      &dbBackBlue},
    {"int dbBackAlpha",                     &dbBackAlpha},
    {"int dbFontRed",                       &dbFontRed},
    {"int dbFontGreen",                     &dbFontGreen},
    {"int dbFontBlue",                      &dbFontBlue},
    {"int dbFontAlpha",                     &dbFontAlpha},
    {"int introScrollSpeed",                &introScrollSpeed},
    {"int introPauseTime",                  &introPauseTime},
    {"int dbScrollSpeed",                   &dbScrollSpeed},
    {"float dbImagePositionX",              &dbImagePositionX},
    {"float dbImagePositionY",                &dbImagePositionY},

	{"",                                    nullptr},
};

void sys_testPrintValue()
{
	printf("vect1 [ %3.3f %3.3f ]\n", vect1.x, vect1.y);
}

//-----------------------------------------------------------------------------
//
// Helper function to assign a num value to a var type string
int con_getVarType ( char *whichType )
//-----------------------------------------------------------------------------
{
	if ( strlen ( whichType ) == 0 )
		return VAR_TYPE_INT;

	if ( 0 == strcmp ( whichType, "int" ) )
		return VAR_TYPE_INT;

	if ( 0 == strcmp ( whichType, "float" ) )
		return VAR_TYPE_VEC3;

	if ( 0 == strcmp ( whichType, "bool" ) )
		return VAR_TYPE_BOOL;

	return -1;
}

//-----------------------------------------------------------
//
// Search for a variable name within all the global names
// return the index if found
// otherwise return -1
int con_getVariableIndex ( string whichVar )
//-----------------------------------------------------------
{
	const char 	*name;
	int 		typeID;
	string 		variableName;

	for ( uint i = 0; i < ( asUINT ) scriptEngine->GetGlobalPropertyCount(); i++ )
		{
			scriptEngine->GetGlobalPropertyByIndex ( i, &name, 0, &typeID );
			variableName = name;

			if ( variableName == whichVar )
				return i;	// Found a match - return index
		}

	return -1;	// Not found
}

//-----------------------------------------------------------
//
// Set the value of a global script variable
void con_setVariableValue ( string whichVar, string newValue )
//-----------------------------------------------------------
{
	int		variableIndex;

	variableIndex = con_getVariableIndex (whichVar);

	if ( -1 == variableIndex )
		{
			con_print ( CON_INFO, false, "ERROR: Could not locate variable [ %s ]", whichVar.c_str() );
			return;
		}

	const char 	*name;
	int 		typeID;
	bool 		isConst;
	void 		*varPointer;
	string 		varType;

	//
	// Get the Global properties
	scriptEngine->GetGlobalPropertyByIndex ( variableIndex, &name, 0, &typeID, &isConst, 0, &varPointer );

	varType = scriptEngine->GetTypeDeclaration ( typeID );

	if ( varType == "int" )
		{
			con_print ( CON_INFO, false, "Set variable to value [ %i ]", atoi ( newValue.c_str() ) );
			* ( int * ) varPointer = atoi ( newValue.c_str() );
		}

	else if ( varType == "float" )
		{
			con_print ( CON_INFO, false, "Set variable to value [ %f ]", atof ( newValue.c_str() ) );
			* ( float * ) varPointer = (float)atof ( newValue.c_str() );
		}

	else if ( varType == "bool" )
		{
			if ( newValue == "true" || newValue == "1" )
				* ( bool * ) varPointer = true;
			else
				* ( bool * ) varPointer = false;

			con_print ( CON_INFO, false, "Set variable to value [ %s ]", newValue.c_str() );
		}
}

//-----------------------------------------------------------
//
// Get the value of a global script variable
void con_getVariableValue ( string whichVar )
//-----------------------------------------------------------
{
	int		variableIndex;

	variableIndex = con_getVariableIndex (whichVar);

	if ( -1 == variableIndex )
		{
			con_print ( CON_INFO, false, "ERROR: Could not locate variable [ %s ]", whichVar.c_str() );
			return;
		}

	const char 	*name;
	int 		typeID;
	bool 		isConst;
	void 		*varPointer;
	string 		varType;

	//
	// Get the Global properties
	scriptEngine->GetGlobalPropertyByIndex ( variableIndex, &name, 0, &typeID, &isConst, 0, &varPointer );

	varType = scriptEngine->GetTypeDeclaration ( typeID );

	if ( varType == "int" )
		{
			con_print ( CON_INFO, false, "Value of %s %s is %i", varType.c_str(), whichVar.c_str(), * ( int * ) varPointer );
			return;

		}

	else if ( varType == "float" )
		{
			con_print ( CON_INFO, false, "Value of %s %s is %5.5f", varType.c_str(), whichVar.c_str(), * ( float * ) varPointer );
			return;

		}

	else if ( varType == "bool" )
		{
			con_print ( CON_INFO, false, "Value of %s %s is %s", varType.c_str(), whichVar.c_str(), * ( bool * ) varPointer ? "true" : "false" );
			return;

		}

	else if ( varType == "string" )
		{
			string printString;

			printString = * ( string * ) varPointer;
			con_print ( CON_INFO, false, "Value of %s %s is %s", varType.c_str(), whichVar.c_str(), printString.c_str() );

		}

	else
		con_print ( CON_INFO, false, "Unknown type [ %s ] for [ %s ]", varType.c_str(), whichVar.c_str() );
}

//-----------------------------------------------------------
//
// List all the variables registered to the script module
// TODO: Pass in module name and select variables from module name
void con_listVariables ()
//-----------------------------------------------------------
{
	asUINT n;

	// List the application registered variables
	con_print ( CON_INFO, false, "Application variables" );

	for ( n = 0; n < ( asUINT ) scriptEngine->GetGlobalPropertyCount(); n++ )
		{
			const char *name;
			int typeId;
			bool isConst;
			scriptEngine->GetGlobalPropertyByIndex ( n, &name, 0, &typeId, &isConst );
			string decl = isConst ? " const " : " ";
			decl += scriptEngine->GetTypeDeclaration ( typeId );
			decl += " ";
			decl += name;
			con_print ( CON_INFO, false, "[ %s ]", decl.c_str() );
		}

	// List the user variables in the module
	asIScriptModule *mod = scriptEngine->GetModule ( "startup" );

	if ( mod )
		{
			//cout << endl;
			con_print ( CON_INFO, false, "User variables" );

			for ( n = 0; n < ( asUINT ) mod->GetGlobalVarCount(); n++ )
				{
					con_print ( CON_INFO, false, "[ %s ]", mod->GetGlobalVarDeclaration ( n ) );
				}
		}
}

//-----------------------------------------------------------
//
// List all the functions registed with the script engine
// TODO: Make module name a paramter to list functions from it
void con_listFunctions ()
//-----------------------------------------------------------
{
	asUINT n;

	// List the application registered functions
	con_print ( CON_INFO, false, "Application functions" );

	for ( n = 0; n < ( asUINT ) scriptEngine->GetGlobalFunctionCount(); n++ )
		{
			asIScriptFunction *func = scriptEngine->GetGlobalFunctionByIndex ( n );

			// Skip the functions that start with _ as these are not meant to be called explicitly by the user
			if ( func->GetName() [0] != '_' )
				con_print ( CON_INFO, false, "[ %s ]", func->GetDeclaration() );
		}

	// List the user functions in the module
	asIScriptModule *mod = scriptEngine->GetModule ( "startup" );

	if ( mod )
		{
			con_print ( CON_INFO, false, "User functions" );

			for ( n = 0; n < ( asUINT ) mod->GetFunctionCount(); n++ )
				{
					asIScriptFunction *func = mod->GetFunctionByIndex ( n );
					con_print ( CON_INFO, false, "[ %s ]", func->GetDeclaration() );
				}
		}
}
