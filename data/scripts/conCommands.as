#include "data/scripts/commonDefines.h"

const int CON_COMMAND = 1;
const int CON_USAGE = 2;
const int CON_FUNC = 3;
const int CON_PARAM = 4;

cpVect as_vectTest(cpVect cpParam)
{
	cpVect testReturn;

	testReturn.x = 333;
	testReturn.y = 999;


	if (cpParam.x == 987)
		printCon_AS("Got 987 value in x", "");


	cpParam.x = 400;
	cpParam.y = 500;

	return testReturn;
}

//-----------------------------------------------------------------------------
//
// Set variables used in the game
void as_setGameVariables()
//-----------------------------------------------------------------------------
{
    drawHUD = true;

    nearPlane = 0.0;
    farPlane = 400.0;
    cameraDistance = 350.0;

    g_scaleViewBy = 3.1;
    g_playFieldSize = 256;  // Tiles across / down in multiples of 32

    g_debugShowWaypoints = false;
    g_debugShowPhysicsLines = false;

    g_debugShowHullLines = false;
    g_debugShowHullCircle = false;

    g_showDoorTriggers = false;

	baseGameSpeed = 1.0f;

//    sdf_addFontInfo(0, "Digital.ttf", 24);
  //  sdf_addFontInfo(1, "oldDigital.ttf", 28);
//    sdf_addFontInfo(2, "President.ttf", 38);

//    sdf_initFontSystem();
    //
    // Number of samples to play at once
    as_numAudioSamples = 16;
    //
    // Speed to animate healing tiles in milliseconds
    healingAnimateInterval = 150;
    //
    // Speed to animate the moving doors in milliseconds
    doorAnimateInterval = 100;
    //
    // Variables for physics engine
    shipDamping = 0.2;
    collisionSlop = 0.9;
    wallFriction = 0.8;
    wallRadius = 2.0;

    playerMass = 0.6;
    playerRadius = 12.0;
    playerFriction = 0.5;
    playerElastic = 0.8;
    //
    // Slowdown factor
    gravity = 0.0002;
    //
    // Max number of clients allowed on server
    maxNumClients = 4;
}

//-----------------------------------------------------------------------------
//
// Script console command - quit program
bool as_conCommandQuit()
//-----------------------------------------------------------------------------
{
    sys_changeMode(MODE_SHUTDOWN);
    //quitProgram = true;

    return true;
}

//-----------------------------------------------------------------------------
//
// Add all the script commands to console commands
void as_addAllScriptCommands()
//-----------------------------------------------------------------------------
{
    sys_addScriptCommand("quit", "Quit the program", "bool as_conCommandQuit()", false);
    sys_addScriptCommand("exit", "Quit the program", "bool as_conCommandQuit()", false);
}
