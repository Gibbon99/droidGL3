#include "data/scripts/commonDefines.h"

//------------------------------------------------------------
//
// Change to a new GUI screen
void as_changeGUIScreen ( int newScreen )
//------------------------------------------------------------
{
	currentGUIScreen = newScreen;
}

//------------------------------------------------------------
//
// This function is called when a object is clicked
void as_guiHandleActionEvent ( string &in objectID )
//------------------------------------------------------------
{
	//
	// Main option screen
	//
	if ( currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrMainMenu" ))
	{
		if ( objectID == "buttonStartGame" )
		{
			gam_startNewGame(NET_GAME_SINGLE);
			return;
		}

		if ( objectID == "buttonJoinGame" )
		{
			gam_startNewGame(NET_GAME_JOIN_NETWORK);
			return;
		}

		if ( objectID == "buttonStartTutorial" )
		{
			return;
		}

		if ( objectID == "buttonOptions" )
		{
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrOptions" ) );
			as_guiSetObjectFocus ( "checkBoxTest" );
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
	else if ( currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrOptions" ))
	{
		if ( objectID == "checkBoxTest" )
		{
			drawHUD = !drawHUD;
			as_updateCheckedStatus  ("checkBoxTest", drawHUD);
			return;
		}

		if ( objectID == "buttonOptionsBack" )
		{
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrMainMenu" ) );
			as_guiSetObjectFocus("buttonStartGame");
			return;
		}
	}
	else if ( currentGUIScreen == as_guiFindIndex (GUI_OBJECT_SCREEN, "scrTerminal" ))
	{
		if ( objectID == "buttonTermLogOff" )
		{
			as_changeGameMode ( MODE_GAME );
			return;
		}

		if (objectID == "buttonTermInfo" )
		{
            as_changeGameMode ( MODE_DATABASE );
            as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrDataBaseView" ) );
            as_guiSetObjectFocus ( "buttonDataBaseReturn" );
            as_setDatabaseAnimateState ( true );
            as_enterTerminalMode();
			return;
		}

		if (objectID == "buttonTermDeck" )
		{
			as_changeGameMode ( MODE_DECK_VIEW );
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrDeckView" ) );
			as_guiSetObjectFocus ( "buttonDeckReturn" );
			return;
		}

		if (objectID == "buttonTermShip" )
		{
			as_changeGameMode ( MODE_SIDE_VIEW );
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrSideView" ) );
			as_guiSetObjectFocus ( "buttonSideReturn" );
			return;
		}
	}
	else if (currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrSideView" ))
	{
		if (objectID == "buttonSideReturn")
		{
			as_changeGameMode ( MODE_GUI );
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrTerminal" ) );
			as_guiSetObjectFocus ( "buttonTermLogOff" );
			return;
		}
	}
	else if (currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrDeckView" ) )
	{
		if (objectID == "buttonDeckReturn")
		{
			as_changeGameMode ( MODE_GUI );
			as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrTerminal" ) );
			as_guiSetObjectFocus ( "buttonTermLogOff" );
			return;
		}
	}
    else if (currentGUIScreen == as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrDataBaseView" ) )
    {
        if (objectID == "buttonDataBaseReturn")
        {
            as_exitTerminalMode();
            as_setDatabaseAnimateState ( false );
            as_changeGameMode ( MODE_GUI );
            as_changeGUIScreen ( as_guiFindIndex ( GUI_OBJECT_SCREEN, "scrTerminal" ) );
            as_guiSetObjectFocus ( "buttonTermLogOff" );
            return;
        }

        if (objectID == "buttonDataBasePrevious")
        {
            as_getPreviousDataBaseRecord();
            return;
        }

        if (objectID == "buttonDataBaseNext")
        {
            as_getNextDataBaseRecord();
            return;
        }
    }
}

//------------------------------------------------------------
//
// Setup the elements for the terminal of the ship GUI screen
void as_setupDataBaseView()
//------------------------------------------------------------
{
    as_guiCreateNewScreen ("scrDataBaseView");

    as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonDataBasePrevious" );
    as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonDataBasePrevious", GUI_COORD_TYPE_PERCENT, 15, 80, 25, 10 );
    as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonDataBasePrevious", GUI_LABEL_CENTER, gui_getString ( "buttonPrevious" ));
    as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonDataBasePrevious", "scr_guiHandleActionEvent");
    as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonDataBasePrevious", "scrDataBaseView" );

    as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonDataBaseReturn" );
    as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonDataBaseReturn", GUI_COORD_TYPE_PERCENT, 50, 80, 25, 10 );
    as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonDataBaseReturn", GUI_LABEL_CENTER, gui_getString ( "buttonOptionsBack" ));
    as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonDataBaseReturn", "scr_guiHandleActionEvent");
    as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonDataBaseReturn", "scrDataBaseView" );

    as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonDataBaseNext" );
    as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonDataBaseNext", GUI_COORD_TYPE_PERCENT, 85, 80, 25, 10 );
    as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonDataBaseNext", GUI_LABEL_CENTER, gui_getString ( "buttonNext" ));
    as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonDataBaseNext", "scr_guiHandleActionEvent");
    as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonDataBaseNext", "scrDataBaseView" );
}

//------------------------------------------------------------
//
// Setup the elements for the sideview of the ship GUI screen
void as_setupSideView()
//------------------------------------------------------------
{
	as_guiCreateNewScreen ("scrSideView");

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonSideReturn" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonSideReturn", GUI_COORD_TYPE_PERCENT, 50, 80, 30, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonSideReturn", GUI_LABEL_CENTER, gui_getString ( "buttonOptionsBack" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonSideReturn", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonSideReturn", "scrSideView" );
}

//------------------------------------------------------------
//
// Setup the elements for the sideview of the ship GUI screen
void as_setupDeckView()
//------------------------------------------------------------
{
	as_guiCreateNewScreen ("scrDeckView");

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonDeckReturn" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonDeckReturn", GUI_COORD_TYPE_PERCENT, 50, 80, 30, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonDeckReturn", GUI_LABEL_CENTER, gui_getString ( "buttonOptionsBack" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonDeckReturn", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonDeckReturn", "scrDeckView" );
}

//------------------------------------------------------------
//
// Setup the elements for the terminal screen
void as_setupGUITerminal ()
//------------------------------------------------------------
{
	as_guiCreateNewScreen ( "scrTerminal" );

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonTermLogOff" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonTermLogOff", GUI_COORD_TYPE_PERCENT, 50, 10, 35, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonTermLogOff", GUI_LABEL_CENTER, gui_getString ( "logoffTerminal" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonTermLogOff", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonTermLogOff", "scrTerminal" );

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonTermInfo" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonTermInfo", GUI_COORD_TYPE_PERCENT, 50, 25, 35, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonTermInfo", GUI_LABEL_CENTER, gui_getString ( "droidInfo" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonTermInfo", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonTermInfo", "scrTerminal" );

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonTermDeck" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonTermDeck", GUI_COORD_TYPE_PERCENT, 50, 40, 35, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonTermDeck", GUI_LABEL_CENTER, gui_getString ( "deckMap" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonTermDeck", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonTermDeck", "scrTerminal" );

	as_guiCreateObject           ( GUI_OBJECT_BUTTON, "buttonTermShip" );
	as_guiSetObjectPosition      ( GUI_OBJECT_BUTTON, "buttonTermShip", GUI_COORD_TYPE_PERCENT, 50, 55, 35, 10 );
	as_guiSetObjectLabel         ( GUI_OBJECT_BUTTON, "buttonTermShip", GUI_LABEL_CENTER, gui_getString ( "shipView" ));
	as_guiSetObjectFunctions     ( GUI_OBJECT_BUTTON, "buttonTermShip", "scr_guiHandleActionEvent");
	as_guiAddObjectToScreen      ( GUI_OBJECT_BUTTON, "buttonTermShip", "scrTerminal" );
}

//-----------------------------------------------------------------------------
//
// Create the display other droid information screen
void as_createIntro_2()
//-----------------------------------------------------------------------------
{
    as_guiCreateNewScreen ( "scrTransferIntro2" );

    as_guiCreateObject             ( GUI_OBJECT_LABEL, "scrTransferIntro2Heading");
    as_guiAddObjectToScreen        ( GUI_OBJECT_LABEL, "scrTransferIntro2Heading", "scrTransferIntro2" );
    as_guiSetObjectPosition        ( GUI_OBJECT_LABEL, "scrTransferIntro2Heading", GUI_COORD_TYPE_PERCENT,      10, 20, 10, 10 );
    as_guiSetObjectLabel           ( GUI_OBJECT_LABEL, "scrTransferIntro2Heading", GUI_LABEL_CENTER, "This is the droid you wish to control." );
}

//-----------------------------------------------------------------------------
//
// Create the display player information screen
void as_createIntro_1()
//-----------------------------------------------------------------------------
{
    as_guiCreateNewScreen ( "scrTransferIntro1" );

    as_guiCreateObject             ( GUI_OBJECT_LABEL, "scrTransferIntro1Heading");
    as_guiAddObjectToScreen        ( GUI_OBJECT_LABEL, "scrTransferIntro1Heading", "scrTransferIntro1" );
    as_guiSetObjectPosition        ( GUI_OBJECT_LABEL, "scrTransferIntro1Heading", GUI_COORD_TYPE_PERCENT,      10, 20, 10, 10 );
    as_guiSetObjectLabel           ( GUI_OBJECT_LABEL, "scrTransferIntro1Heading", GUI_LABEL_CENTER, "This is the droid you currently control." );
}

//-----------------------------------------------------------------------------
//
// Create the transfer choose side screen
void as_createTransferChooseSide()
//-----------------------------------------------------------------------------
{
	as_guiCreateNewScreen ( "scrTransferStart" );

	as_guiCreateObject             ( GUI_OBJECT_LABEL, "scrTransferChooseHeading");
	as_guiAddObjectToScreen        ( GUI_OBJECT_LABEL, "scrTransferChooseHeading", "scrTransferStart" );
	as_guiSetObjectPosition        ( GUI_OBJECT_LABEL, "scrTransferChooseHeading", GUI_COORD_TYPE_PERCENT,      10, 20, 10, 10 );
	as_guiSetObjectLabel           ( GUI_OBJECT_LABEL, "scrTransferChooseHeading", GUI_LABEL_CENTER, "Choose the side." );
}


//-----------------------------------------------------------------------------
//
// Create the display player information screen
void as_createIntro_0()
//-----------------------------------------------------------------------------
{
    as_guiCreateNewScreen ( "scrTransferIntro0" );

    as_guiCreateObject             ( GUI_OBJECT_LABEL, "scrTransferIntro0Heading");
    as_guiAddObjectToScreen        ( GUI_OBJECT_LABEL, "scrTransferIntro0Heading", "scrTransferIntro0" );
    as_guiSetObjectPosition        ( GUI_OBJECT_LABEL, "scrTransferIntro0Heading", GUI_COORD_TYPE_PERCENT,      10, 20, 10, 10 );
    as_guiSetObjectLabel           ( GUI_OBJECT_LABEL, "scrTransferIntro0Heading", GUI_LABEL_CENTER, "Commencing transfer process...." );
}

//-----------------------------------------------------------------------------
//
// Setup the transfer screens
void as_createTransferScreens()
//-----------------------------------------------------------------------------
{
    as_createIntro_0();
    as_createIntro_1();
    as_createIntro_2();
	as_createTransferChooseSide();
}

//-----------------------------------------------------------------------------
//
// Create the options screen
void as_createOptionsScreen()
//-----------------------------------------------------------------------------
{
	as_guiCreateNewScreen ( "scrOptions" );

	as_guiCreateObject             ( GUI_OBJECT_LABEL, "scrOptionsHeading");
	as_guiAddObjectToScreen        ( GUI_OBJECT_LABEL, "scrOptionsHeading", "scrOptions" );
	as_guiSetObjectPosition        ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_COORD_TYPE_PERCENT,      50, 10, 10, 10 );
	as_guiSetObjectLabel           ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_LABEL_CENTER, "OPTIONS" );

	as_guiCreateObject             ( GUI_OBJECT_CHECKBOX, "checkBoxTest" );
	as_guiAddObjectToScreen        ( GUI_OBJECT_CHECKBOX, "checkBoxTest", "scrOptions" );
	as_guiSetObjectPosition        ( GUI_OBJECT_CHECKBOX, "checkBoxTest", GUI_COORD_TYPE_PIXEL,              20, 200, 44, 44 );
	as_guiSetObjectLabel           ( GUI_OBJECT_CHECKBOX, "checkBoxTest", GUI_LABEL_CENTER, "Show HUD graphic" );
	as_guiSetObjectFunctions       ( GUI_OBJECT_CHECKBOX, "checkBoxTest",          "scr_guiHandleActionEvent");

	as_guiCreateObject             ( GUI_OBJECT_TEXTBOX, "textBoxTest" );
	as_guiAddObjectToScreen        ( GUI_OBJECT_TEXTBOX, "textBoxTest", "scrOptions" );
	as_guiSetObjectPosition        ( GUI_OBJECT_TEXTBOX, "textBoxTest", GUI_COORD_TYPE_PIXEL,              230, 555, 380, 44 );
	as_guiSetObjectLabel           ( GUI_OBJECT_TEXTBOX, "textBoxTest", GUI_LABEL_CENTER, "Server address" );
	as_guiSetObjectFunctions       ( GUI_OBJECT_TEXTBOX, "textBoxTest",          "scr_guiHandleActionEvent");

	as_guiCreateObject              ( GUI_OBJECT_BUTTON, "buttonOptionsBack");
	as_guiAddObjectToScreen         ( GUI_OBJECT_BUTTON, "buttonOptionsBack", "scrOptions");
	as_guiSetObjectPosition         ( GUI_OBJECT_BUTTON, "buttonOptionsBack",     GUI_COORD_TYPE_PERCENT,      50, 10, 30, 10 );
	as_guiSetObjectLabel            ( GUI_OBJECT_BUTTON, "buttonOptionsBack",     GUI_LABEL_CENTER, gui_getString ( "buttonOptionsBack" ) );
	as_guiSetObjectFunctions        ( GUI_OBJECT_BUTTON, "buttonOptionsBack",     "scr_guiHandleActionEvent");
}

//-----------------------------------------------------------------------------
//
// Setup colors and variables for db scrollbox
void as_setdbValues ()
//-----------------------------------------------------------------------------
{
	// Position of the droid rotating graphic
	dbImagePositionX = 650;
	dbImagePositionY = 300;

	// Position of the scrollbox in the droid database screen
	dbStartX = 20;
	dbStartY = winHeight - 150;
	dbWidth = 450;
	dbHeight = 380;
	dbBackRed = 0;
	dbBackGreen = 0;
	dbBackBlue = 255;
	dbBackAlpha = 255;
	dbFontRed = 110;
	dbFontGreen = 190;
	dbFontBlue = 190;
	dbFontAlpha = 200;
	dbScrollSpeed = 20;     // Scroll speed in milliseconds
}

//-----------------------------------------------------------------------------
//
// Setup colors and variables for intro scrollbox
void as_setIntroValues()
//-----------------------------------------------------------------------------
{
	int gutterSize = 20;

	introStartX = gutterSize;
	introStartY = winHeight - 100;
	introWidth = (winWidth - gutterSize) - introStartX;
	introHeight = 500;      // Get size of HUD image??
	introBackRed = 0;
	introBackGreen = 0;
	introBackBlue = 255;
	introBackAlpha = 255;
	introFontRed = 110;
	introFontGreen = 190;
	introFontBlue = 190;
	introFontAlpha = 200;
	introScrollSpeed = 10;     // Scroll speed in milliseconds
	introPauseTime = 3000;      // Pause for 3 seconds
}

//-----------------------------------------------------------------------------
//
// Create colors for the sideview graphic
void as_setSideviewColors()
//-----------------------------------------------------------------------------
{
	as_createSideViewColor  (SIDEVIEW_SHIP_COLOR,        142, 182, 12, 255);
	as_createSideViewColor  (SIDEVIEW_ACTIVE_DECK_COLOR, 71, 199, 232, 255);
	as_createSideViewColor  (SIDEVIEW_ENGINE_COLOR,      34, 34, 34, 255);
	as_createSideViewColor  (SIDEVIEW_LIFT_COLOR,        18, 76, 88, 255);
	as_createSideViewColor  (SIDEVIEW_ACTIVE_LIFT_COLOR, 38, 221, 188, 255);
}

//-----------------------------------------------------------------------------
//
// Setup the GUI screens and elements - called from Host
void as_setupGUI()
//-----------------------------------------------------------------------------
{
	currentLanguage = LANG_ENGLISH;
	as_setLanguageStrings();

	focusAnimateIntervalValue = 7;      // Color animation in milliseconds
	getMousePositionInterval = 50;      // How often to get the mouse position in milliseconds - to fast is too many events

    dbAnimateSpeed = 75;

	as_setIntroValues();
    as_setdbValues ();

	as_guiCreateNewScreen ( "scrMainMenu" );

	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonStartGame" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonJoinGame" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonOptions" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonStartTutorial" );
	as_guiCreateObject ( GUI_OBJECT_BUTTON, "buttonQuit" );

	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonStartGame", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonJoinGame", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonOptions", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonStartTutorial", "scrMainMenu" );
	as_guiAddObjectToScreen ( GUI_OBJECT_BUTTON, "buttonQuit", "scrMainMenu" );

	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonStartGame", GUI_COORD_TYPE_PERCENT,      50, 13, 30, 7 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonJoinGame", GUI_COORD_TYPE_PERCENT,       50, 28, 30, 8 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonOptions", GUI_COORD_TYPE_PERCENT,        50, 43, 30, 9 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonStartTutorial", GUI_COORD_TYPE_PERCENT,  50, 58, 30, 10 );
	as_guiSetObjectPosition ( GUI_OBJECT_BUTTON, "buttonQuit", GUI_COORD_TYPE_PERCENT,           50, 73, 30, 11 );

	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonStartGame",     GUI_LABEL_CENTER, gui_getString ( "startGame" ) );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonJoinGame",      GUI_LABEL_CENTER, gui_getString ( "joinGame" ) );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonOptions",       GUI_LABEL_CENTER, gui_getString ( "options" ) );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonStartTutorial", GUI_LABEL_CENTER, gui_getString ( "tutorial" ) );
	as_guiSetObjectLabel ( GUI_OBJECT_BUTTON, "buttonQuit",          GUI_LABEL_CENTER, gui_getString ( "exit" ) );

	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonStartGame",     "scr_guiHandleActionEvent");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonJoinGame",      "scr_guiHandleActionEvent");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonOptions",       "scr_guiHandleActionEvent");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonStartTutorial", "scr_guiHandleActionEvent");
	as_guiSetObjectFunctions ( GUI_OBJECT_BUTTON, "buttonQuit",          "scr_guiHandleActionEvent");

	as_createOptionsScreen ();
	as_setupGUITerminal ();
	as_setupSideView ();
	as_setupDeckView ();
    as_setupDataBaseView ();
    as_createTransferScreens ();

	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_ACTIVE_COL, 255, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_INACTIVE_COL, 155, 155, 155, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_ACTIVE_CORNER_COL, 255, 255, 0, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_INACTIVE_CORNER_COL, 55, 55, 55, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_ACTIVE_LABEL_COL, 0, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_BUTTON, "ALL", GUI_INACTIVE_LABEL_COL, 55, 55, 55, 255 );

	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_ACTIVE_COL, 255, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_INACTIVE_COL, 155, 155, 155, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_ACTIVE_CORNER_COL, 255, 255, 0, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_INACTIVE_CORNER_COL, 55, 55, 55, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_ACTIVE_LABEL_COL, 0, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_CHECKBOX, "ALL", GUI_INACTIVE_LABEL_COL, 55, 55, 55, 255 );

	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_ACTIVE_COL, 255, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_INACTIVE_COL, 155, 155, 155, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_ACTIVE_CORNER_COL, 255, 255, 0, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_INACTIVE_CORNER_COL, 55, 55, 55, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_ACTIVE_LABEL_COL, 0, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_TEXTBOX, "ALL", GUI_INACTIVE_LABEL_COL, 55, 55, 55, 255 );

    as_guiSetObjectColor ( GUI_OBJECT_LABEL, "ALL", GUI_ACTIVE_LABEL_COL, 255, 255, 255, 255 );

	as_guiSetObjectColor ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_ACTIVE_COL, 255, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_INACTIVE_COL, 155, 155, 155, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_ACTIVE_LABEL_COL, 0, 255, 255, 255 );
	as_guiSetObjectColor ( GUI_OBJECT_LABEL, "scrOptionsHeading", GUI_INACTIVE_LABEL_COL, 55, 55, 55, 255 );

	as_setSideviewColors();
}
