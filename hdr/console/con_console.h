#pragma once

#include "hdr/system/sys_main.h"

#include "hdr/script/angelscript.h"
#include "hdr/script/scriptstdstring.h"
#include "hdr/script/as_scriptarray.h"
#include "hdr/opengl/gl_opengl.h"

#define CONSOLE_ADD_LINE        0x00

#define CON_COMMAND_HOST		1
#define CON_COMMAND_SCRIPT		2

#define CON_COMMAND	1
#define CON_USAGE	2
#define CON_FUNC	3

using namespace std;
//
//-----------------------------------------------------------------------------
// Generic callback function pointer
// needs the (...) to allow for any parameter passing without hardcoding
// different types for different functions
//-----------------------------------------------------------------------------
typedef int	( *ExternFunc ) ( ... );

#define NUM_CON_LINES			40
#define NUM_MAX_CON_COMMANDS	30

extern int conFontSize;

/*
typedef struct
{
	char		command[MAX_STRING_SIZE];
	char		usage[MAX_STRING_SIZE];
	ExternFunc	conFunc;
} _conCommand;
*/

//-----------------------------------------------------------------------------
//
// AngelScript engine
//
//-----------------------------------------------------------------------------
extern asIScriptEngine *scriptEngine;

//-----------------------------------------------------------------------------
//
// Struct to hold host function mapping to script function names
//
//-----------------------------------------------------------------------------
typedef struct
{
	string			scriptFunctionName;
	void			*hostFunctionPtr;
} _hostScriptFunctions;

extern _hostScriptFunctions hostVariables[];

typedef struct
{
	int			type;
	string		command;
	string		usage;
	string		scriptFunc;
	ExternFunc	conFunc;
} _conCommand;

extern vector<_conCommand>			conCommands;
extern int							conCurrentNumCommands;

typedef struct
{
	string		conLine;
	_glColor	conLineColor;
} _conLine;

extern bool         consoleIsReady;
extern _conLine		conLines[NUM_CON_LINES];
extern _conLine		conPrevCommands[NUM_MAX_CON_COMMANDS];
extern _conLine		conCurrentLine;

extern unsigned int		    conCurrentCharCount;

extern _glColor     currentConLineColor;

#define CON_CURSOR_MAX_COUNT	100

extern bool			conCursorIsOn;

extern unsigned int		    conHistoryPtr;			// Which history command are we at

// Show the console
void con_showConsole();

// Draw the console screen
//void con_createConsoleScreen ();

// Add a line to the console
void con_print ( int type, bool fileLog, const char *printText, ... );

// Init the console
void con_initConsole();

// Process a entered command
void con_processCommand ( string comLine );

// Push a command into the console for processing
void con_pushCommand ( char *param1 );

// Add a command to the console command list
bool con_addCommand ( string command, string usage, ExternFunc functionPtr );

// Display the current prompt
void con_processBackspaceKey ( float frameInterval );

// Process the cursor
void con_processCursor ( );

// Add a valid command to the history buffer
void con_addHistoryCommand ( string command );

// Pop a command from the history buffer
void con_popHistoryCommand();

// Autocompletion for console commands
void con_completeCommand ( string lookFor );

// Release the memory for the script file
//void con_scriptShutdown();

// Set the current color for printing lines to the console
void con_setColor ( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );

// Add a script console command to the executable list of functions
bool con_addScriptConsoleFunction ( string funcName, string funcPtr, bool setParam );

void con_addScriptCommand ( string command, string usage, string funcPtr, bool setParam );

// Execute a console command from a script
void con_pushScriptCommand ( std::string command );

// Print to console from the scripts - String version
void con_scriptPrintStr ( std::string *msgText, std::string *msgParam );

// Execute a function from the script
bool con_executeScriptFunction ( string functionName, string funcParam );

// Handle a console user event
int con_processConsoleUserEvent ( void *ptr );

// Add a new line to the console - move all the others up
// 0 is the new line added
void con_incLine ( _glColor lineColor, string newLine );

//-----------------------------------------------------------------------------
//
// Commands executed from the console
//
//-----------------------------------------------------------------------------

// Show the available commands
int conHelp();

// Display information about the version of OpenGL we are running
int conOpenGLInfo();

// Show the commands added from a script
int showScriptAddedCommands();

// List the variables we can change
void con_listVariables ();

void con_listFunctions ();

// Get the value of a single variable
void con_getVariableValue ( string whichVar );

// Set the value of a global script variable
void con_setVariableValue ( string whichVar, string newValue );

// Quit the game from the console
int conQuit ();
