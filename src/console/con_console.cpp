#include <cstdarg>
#include <sstream>
#include <iostream>
#include <utility>

#include "hdr/console/con_console.h"
#include "hdr/io/io_logfile.h"
#include "hdr/system/sys_embedFont.h"

bool			conCursorIsOn;

_conLine		conLines[NUM_CON_LINES];
_conLine		conPrevCommands[NUM_MAX_CON_COMMANDS];

vector<_conCommand>			conCommands;
int							conCurrentNumCommands;

GLuint			conCurrentCharCount;

_glColor     	currentConLineColor;
GLuint			conHistoryPtr;			// Which history command are we at

_conLine		conCurrentLine;
_conLine		conCurrentPrompt;

int				conNumInHistory = 0;

//-----------------------------------------------------------------------------
//
// Add all the host based console commands here
void con_addConsoleCommands()
//-----------------------------------------------------------------------------
{
	con_addCommand ( "help",			"List out available commands",	( ExternFunc ) conHelp );
	con_addCommand ( "glInfo",			"Info about openGL",			( ExternFunc ) conOpenGLInfo );
	con_addCommand ( "listVars",		"List script variables",		( ExternFunc ) conListVariables );
	con_addCommand ( "listFunctions",	"List script functions",		( ExternFunc ) conListFunctions );
	con_addCommand ( "getVar",     		"Get the value of a variable",  ( ExternFunc ) conGetVariableValue );
	con_addCommand ( "setVar",     		"Set the value of a variable",  ( ExternFunc ) conSetVariableValue );
	con_addCommand ( "scShowFunc", 		"Show all script added commands", ( ExternFunc ) showScriptAddedCommands );
	con_addCommand ( "quit",            "Shutdown the game",            (ExternFunc) conQuit );

//	conAddCommand("scDo",		"Execute script function",		(ExternFunc)conScriptExecute);
}

//----------------------------------------------------------------
//
// Handle a console user event - called by thread
int con_processConsoleUserEvent ( void *ptr )
//----------------------------------------------------------------
{
	_myEventData tempEventData;

	while ( runThreads )
	{
		SDL_Delay(THREAD_DELAY_MS);

		if ( !consoleEventQueue.empty ())   // stuff in the queue to process
		{
			if ( SDL_LockMutex (consoleMutex) == 0 )
			{
				tempEventData = consoleEventQueue.front();
				consoleEventQueue.pop ();
				SDL_UnlockMutex (consoleMutex);
			}

			switch ( tempEventData.eventAction )
			{
				case CONSOLE_ADD_LINE:
					_glColor lineColor;
					lineColor.red = (float)tempEventData.data1;
					lineColor.green = (float)tempEventData.data2;
					lineColor.blue = (float)tempEventData.data3;
					lineColor.alpha = 1.0f;
					con_incLine (lineColor, tempEventData.eventString);
					break;

				default:
					break;
			}
		}
	}
	printf("CONSOLE thread stopped.\n");
	return 0;
}

//-----------------------------------------------------------------------------
//
// Show the console
void con_showConsole()
//-----------------------------------------------------------------------------
{
#define EMBEDDED_FONT_HEIGHT 17.0f

	float 			currentY;
	glm::vec4		lineColor;
	glm::vec2		linePosition;
	auto loopCount = (int)(winHeight / EMBEDDED_FONT_HEIGHT);
	_conLine		conTempLine;

	if (SDL_LockMutex (consoleMutex) == 0)  // Wait until thread is not updating console contents
	{
		currentY = EMBEDDED_FONT_HEIGHT;

		for ( int i = 0; i != loopCount; i++ )
		{
			lineColor.r = conLines[i].conLineColor.red;
			lineColor.g = conLines[i].conLineColor.green;
			lineColor.b = conLines[i].conLineColor.blue;
			lineColor.a = conLines[i].conLineColor.alpha;

			linePosition.x = 1.0f;
			linePosition.y = currentY;
			currentY = currentY + (EMBEDDED_FONT_HEIGHT);

			fnt_printText (linePosition, lineColor, "%s", conLines[i].conLine.c_str ());
		}

		conTempLine.conLine = conCurrentLine.conLine;

		if ( conCursorIsOn )
			conTempLine.conLine += "<";
		else
			conTempLine.conLine += " ";

		linePosition.x = 1.0f;
		linePosition.y = 0.0f;

		fnt_printText (linePosition, lineColor, "%s", conTempLine.conLine.c_str ());

		SDL_UnlockMutex (consoleMutex);
	}
	else
		return;     // Can't get lock
}

//-----------------------------------------------------------------------------
//
// Add a new line to the console - move all the others up
// 0 is the new line added
//
void con_incLine ( _glColor lineColor, string newLine )
//-----------------------------------------------------------------------------
{
	int i;

	for ( i = NUM_CON_LINES - 1; i != 0; i-- )
	{
		conLines[i].conLine = conLines[i - 1].conLine;
		conLines[i].conLineColor = conLines[i - 1].conLineColor;
	}

	conLines[0].conLine = std::move (newLine);
	conLines[0].conLineColor = lineColor;
}

//-----------------------------------------------------------------------------
//
// Init the console
void con_initConsole()
//-----------------------------------------------------------------------------
{
	int i;

	for ( i = 0; i < NUM_CON_LINES; i++ )
		{
			conLines[i].conLine = "";
			conLines[i].conLineColor.red = 1.0f;
			conLines[i].conLineColor.green = 1.0f;
			conLines[i].conLineColor.blue = 1.0f;
			conLines[i].conLineColor.alpha = 1.0f;
		}

	currentConLineColor.red = 1.0f;
	currentConLineColor.green = 1.0f;
	currentConLineColor.blue = 1.0f;
	currentConLineColor.alpha = 1.0f;

	for ( i = NUM_MAX_CON_COMMANDS - 1; i != 0; i-- )
		conPrevCommands[i].conLine = "";

	conHistoryPtr = 0;
	conNumInHistory = 0;

	conCurrentPrompt.conLine = "]_";
	conCurrentLine.conLine = "";

	conCurrentCharCount = 0;

	con_addConsoleCommands();
}

//-----------------------------------------------------------------------------
//
// Add a script function to the console commands
void con_addScriptCommand ( string command, string usage, string funcPtr, bool setParam )
//-----------------------------------------------------------------------------
{
	_conCommand			tempConCommand;

	tempConCommand.type =		CON_COMMAND_SCRIPT;
	tempConCommand.command = 	std::move(command);
	tempConCommand.usage =      std::move (usage);
	tempConCommand.scriptFunc = std::move(funcPtr);

	conCommands.push_back ( tempConCommand );
	sys_addScriptConsoleFunction ( conCommands.back().command, conCommands.back().scriptFunc, setParam );
	conCurrentNumCommands++;
}

//-----------------------------------------------------------------------------
//
// Push a command into the console for processing
void con_pushCommand ( char *param1 )
//-----------------------------------------------------------------------------
{
	con_processCommand ( param1 );
}

//-----------------------------------------------------------------------------
//
// Execute a console command from a script
void con_pushScriptCommand ( std::string command )
//-----------------------------------------------------------------------------
{
	con_processCommand ( ( char * ) command.c_str() );
}

//-----------------------------------------------------------------------------
//
// Autocompletion for console commands
void con_completeCommand ( string lookFor )
//-----------------------------------------------------------------------------
{
	string	lookForKeep;

	lookForKeep = lookFor;

	//
	// Check each of the commands
	for ( int i = 0; i != conCurrentNumCommands; i++ )
		{
			if ( conCommands[i].command.find ( lookFor, 0 ) != string::npos )
				{
					con_print ( false, true, "[ %s ]", conCommands[i].command.c_str() );
					conCurrentLine.conLine = "";
					conCurrentLine.conLine = conCommands[i].command;
					conCurrentCharCount = (int)conCommands[i].command.length();
				}
		}
}

//-----------------------------------------------------------------------------
//
// Process the cursor
void con_processCursor ( )
//-----------------------------------------------------------------------------
{
	conCursorIsOn =! conCursorIsOn;
}

//-----------------------------------------------------------------------------
//
// Display the current prompt
void con_processBackspaceKey ( float frameInterval )
//-----------------------------------------------------------------------------
{
	_conLine	conTempLine;

	if ( conCurrentCharCount > 0 )
		conCurrentCharCount--;

	conTempLine.conLine = conCurrentLine.conLine.substr ( 0, conCurrentCharCount  );
	conCurrentLine.conLine = conTempLine.conLine;
}

//-----------------------------------------------------------------------------
//
// Pop a command from the history buffer
void con_popHistoryCommand()
//-----------------------------------------------------------------------------
{
	conCurrentLine.conLine = conPrevCommands[conHistoryPtr].conLine;
	conCurrentCharCount = (int)conPrevCommands[conHistoryPtr].conLine.length();
}

//-----------------------------------------------------------------------------
//
// Add a valid command to the history buffer
void con_addHistoryCommand ( string command )
//-----------------------------------------------------------------------------
{
	conNumInHistory++;

	if ( conNumInHistory > NUM_MAX_CON_COMMANDS )
		conNumInHistory = NUM_MAX_CON_COMMANDS;

	for ( int i = NUM_MAX_CON_COMMANDS - 1; i != 0; i-- )
		{
			conPrevCommands[i].conLine = conPrevCommands[i - 1].conLine;
		}

	conPrevCommands[0].conLine = std::move (command);
	conHistoryPtr = 0;
}

//-----------------------------------------------------------------------------
//
// Add a command to the console command list
bool con_addCommand ( string command, string usage, ExternFunc functionPtr )
//-----------------------------------------------------------------------------
{
	_conCommand			tempConCommand;

	//
	// Check values going in
	//
	if ( 0 == command.length() )
		return false;

	if ( 0 == usage.length() )
		usage = "";

	//
	// Next slot
	//
	tempConCommand.command = 		command;
	tempConCommand.usage = 			usage;
	tempConCommand.conFunc = 		functionPtr;
	tempConCommand.type = 			CON_COMMAND_HOST;

	conCommands.push_back ( tempConCommand );

	conCurrentNumCommands++;

	return true;
}

//-----------------------------------------------------------------------------
//
// Add a line to the console
// Pass in type to change the color
void con_print ( int type, bool fileLog, const char *printText, ... )
//-----------------------------------------------------------------------------
{
	va_list		args;
	char		conText[MAX_STRING_SIZE * 2];
	string      finalText;

	//
	// check and make sure we don't overflow our string buffer
	//
	if ( strlen ( printText ) >= MAX_STRING_SIZE - 1 )
		printf ( "String passed to console is too long - Max [ %i ] - [ %i ]", ( MAX_STRING_SIZE - 1 ), strlen ( printText ) - ( MAX_STRING_SIZE - 1 ) );

	//
	// get out the passed in parameters
	//
	va_start ( args, printText );
	vsprintf ( conText, printText, args );
	va_end ( args );

	if ( fileLog )
		io_logToFile ( "Console : %s", conText );

	switch ( type )
		{
			case CON_NOCHANGE:
				break;

			case CON_TEXT:
				con_setColor ( 1.0f, 1.0f, 1.0f, 1.0f );
				break;

			case CON_INFO:
				con_setColor ( 1.0f, 1.f, 0.0f, 1.0f );
				finalText = "INFO: ";
				break;

			case CON_ERROR:
				con_setColor ( 1.0f, 0.0f, 0.0f, 0.0f );
				finalText = "ERROR: ";
				break;

			default:
				break;
		}

		finalText += conText;
		evt_sendEvent(USER_EVENT_CONSOLE, CONSOLE_ADD_LINE, (int)currentConLineColor.red, (int)currentConLineColor.green, (int)currentConLineColor.blue, glm::vec2(), glm::vec2(), finalText);
}

//-----------------------------------------------------------------------------
//
// Process an entered command
void con_processCommand ( string comLine )
//-----------------------------------------------------------------------------
{
	int                 i;
	vector<string>     	tokens;   // one command and one param
	string              buffer;

	string              command;
	string              param;
	string				param2;
	bool    			conMatchFound = false;

	if ( comLine.empty ())
		return;

	//
	// Start the string with known empty value
	command = "";
	param = "";
	//
	// Get the command and any parameters
	//
	// Insert the string into a stream
	istringstream iss ( comLine, istringstream::in );

	//
	// Put each word into the vector
	while ( iss >> buffer )
		{
			tokens.push_back ( buffer );
		}

	command = tokens[0];

	//
	// Make sure there is a paramater to use
	if ( tokens.size() > 1 )
		param = tokens[1];

	else
		param = "";

	if ( tokens.size() > 2 )
		param2 = tokens[2];

	else
		param2 = "";

	//
	// Check each of the commands
	for ( i = 0; i != conCurrentNumCommands; i++ )
		{
			if ( conCommands[i].command == command )
				{
					con_addHistoryCommand ( comLine );
					con_incLine ( currentConLineColor, comLine );

					if ( CON_COMMAND_HOST == conCommands[i].type )
						conCommands[i].conFunc ( param, param2 );
					else
						{
							util_executeScriptFunction ( conCommands[i].command, param );
						}

					conMatchFound = true;
					break;
				}
		}

	if ( !conMatchFound )
		con_print ( false, true, "Command [ %s ] not found.", comLine.c_str() );

	//
	// Clear out the string so it doesn't show
	conCurrentLine.conLine = "";

	conCurrentCharCount = 0;
}

//-----------------------------------------------------------------------------
//
// Set the current color for printing lines to the console
void con_setColor ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
//-----------------------------------------------------------------------------
{
	currentConLineColor.red = red;
	currentConLineColor.green = green;
	currentConLineColor.blue = blue;
	currentConLineColor.alpha = alpha;
}
