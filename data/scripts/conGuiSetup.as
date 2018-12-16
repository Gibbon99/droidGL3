#include "data/scripts/commonDefines.h"

//------------------------------------------------------------
//
// This function is called when a object is clicked
void as_guiHandleButtonPress ( string &in objectID )
//------------------------------------------------------------
{
	//
	// Main option screen
	//
	if ( currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrMainMenu" ))
	{
		if ( objectID == "buttonStartGame" )
		{
			return;
		}

		if ( objectID == "buttonStartTutorial" )
		{
			return;
		}

		if ( objectID == "buttonOptions" )
		{
			return;
		}

		if ( objectID == "buttonHelp" )
		{
			return;
		}

		if ( objectID == "buttonQuit" )
		{
			quitProgram = true;
			return;
		}
	}
}

//-----------------------------------------------------------------------------
//
// Setup the GUI screens and elements - called from Host
void as_setupGUI()
//-----------------------------------------------------------------------------
{

	focusAnimateIntervalValue = 7;     // Color animation in milliseconds

	as_guiCreateNewScreen ( "scrMainMenu" );

	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonStartGame" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonOptions" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonStartTutorial" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonQuit" );

	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonStartGame", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonOptions", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonStartTutorial", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonQuit", "scrMainMenu" );

	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonStartGame", GUI_COORD_TYPE_PERCENT,      50, 10, 30, 10 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonOptions", GUI_COORD_TYPE_PERCENT,        50, 30, 30, 10 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonStartTutorial", GUI_COORD_TYPE_PERCENT,  50, 50, 30, 10 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonQuit", GUI_COORD_TYPE_PERCENT,           50, 70, 30, 10 );

	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonStartGame", GUI_LABEL_CENTER, "Start Game" );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonOptions", GUI_LABEL_CENTER, "Options" );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonStartTutorial", GUI_LABEL_CENTER, "Tutorial" );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonQuit", GUI_LABEL_CENTER, "Exit" );

	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonStartGame",     "scr_guiHandleButtonPress");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonOptions",       "scr_guiHandleButtonPress");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonStartTutorial", "scr_guiHandleButtonPress");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonQuit",          "scr_guiHandleButtonPress");

	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_ACTIVE_COL, 255, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_INACTIVE_COL, 155, 155, 155, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_ACTIVE_CORNER_COL, 255, 255, 0, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_INACTIVE_CORNER_COL, 55, 55, 55, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_ACTIVE_LABEL_COL, 0, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_BUTTON_INACTIVE_LABEL_COL, 55, 55, 55, 255 );
}
