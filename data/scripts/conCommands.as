#include "data/scripts/commonDefines.h"

const int CON_COMMAND = 1;
const int CON_USAGE = 2;
const int CON_FUNC = 3;
const int CON_PARAM = 4;

//-----------------------------------------------------------------------------
//
// Set variables used in the game
void as_setGameVariables()
//-----------------------------------------------------------------------------
{
    winWidth = 800;
    winHeight = 600;
    //
    // Change window size and set new global variables
    lib_resizeWindow(winWidth, winHeight);

    sdf_addFontInfo(0, "Digital.ttf", 24);
    sdf_addFontInfo(1, "oldDigital.ttf", 28);
    sdf_addFontInfo(2, "President.ttf", 38);

    sdf_initFontSystem();
    //
    // Use the sound system or not
    as_useSound = true;
    //
    // Set volume level - 0 to 100
    aud_setAudioGain(100);
    //
    // Number of samples to play at once
    as_numAudioSamples = 16;
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