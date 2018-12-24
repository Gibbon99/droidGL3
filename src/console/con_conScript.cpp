#include <utility>
#include "hdr/gui/gui_main.h"
#include "hdr/game/gam_game.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/script/as_scriptbuilder.h"
#include "hdr/io/io_logfile.h"
#include "hdr/physfs/physfs.h"
#include "hdr/gui/gui_checkBox.h"
#include "hdr/gui/gui_language.h"

unsigned int		numFunctionsInScripts;
unsigned int		numHostScriptFunctions;
bool				scriptEngineStarted = false;
asDWORD				callType;
asIScriptEngine 	*scriptEngine = nullptr;
asIScriptContext 	*scriptContext = nullptr;
CScriptBuilder 		builder;

void PrintVariables();

//-----------------------------------------------------------------------------
//
// AngelScript engine
//
//-----------------------------------------------------------------------------

//--------------------------------------------------------
//
// ScriptEngine routines
//
// Creates and manages the AngelScript engine
//
//--------------------------------------------------------

int				    numScriptFunctions = 0;		// Number of functions declared in script
asIScriptContext    *context = nullptr;
asIScriptModule     *myModule= nullptr;

//-----------------------------------------------------------------------------
//
// Structure to hold all the script filenames and name
//-----------------------------------------------------------------------------
typedef struct
{
	string 		scriptFileName;
	string 		scriptName;
} _scriptInfo;

_scriptInfo     scriptInfo[] =
{
	{"conCommands.as",              "script"},
	{"conGuiSetup.as",              "GUI"},
	{"langEnglish.as",              "langEnglish"},
	{"langItalian.as",              "langItalian"},
	{"",            ""},
};

void sys_testVect(cpVect testValue)
	{
		printf("[ Script ] Values [ %3.3f %3.3f ]\n", testValue.x, testValue.y);
	};

//-----------------------------------------------------------------------------
//
// Struct to hold host function mapping to script function names
//
//-----------------------------------------------------------------------------
//typedef struct
//{
//	char			scriptFunctionName[MAX_STRING_SIZE];
//	void			*hostFunctionPtr;
//} _hostScriptFunctions;

_hostScriptFunctions hostScriptFunctions[] =
{
	{"void printCon_AS(string &in, string &in)",                        (void * ) &con_scriptPrintStr},

	{"void sys_addScriptCommand(string &in, string &in, string &in, bool setParam)", ( void * ) &con_addScriptCommand},

	{"void conPushCommand_AS(string &in)",                              (void * ) con_pushScriptCommand},
	{"bool startPackFile(string &in, string &in)",                      (void * ) io_startFileSystem},
	{"bool startLogFile(string &in)",                                   (void * ) io_startLogFile},
	{"void sys_changeMode(int newMode)",                                (void * ) sys_changeMode},
	{"void sys_testVect(cpVect &out)",                                  (void * ) sys_testVect},
	{"void PrintVariables()",                                           (void * ) PrintVariables},

	{"void as_guiCreateNewScreen   (string &in)", 						( void * ) &gui_hostCreateNewScreen},
	{"void as_guiCreateObject	   (int guiObjectType, string &in)", 	( void * ) &gui_hostCreateObject},
	{"int as_guiFindIndex		   (int guiObjectType, string &in)", 	( void * ) &gui_findIndex},
	{"void as_guiSetObjectColor    (int guiObjectType, string &in, int whichColor, int red, int green, int blue, int alpha)", 		( void * ) &gui_hostSetObjectColor},
	{"void as_guiSetObjectPosition (int guiObjectType, string &in, int coordType, int startX, int startY, int width, int height)", 	( void * ) &gui_hostSetObjectPosition},
	{"void as_guiSetObjectLabel    (int guiObjectType, string &in, int labelPos, string &in)", 		( void * ) &gui_hostSetObjectLabel},
	{"void as_guiAddObjectToScreen (int guiObjectType, string &in, string &in)", 					( void * ) &gui_hostAddObjectToScreen},
	{"void as_guiSetObjectFunctions(int guiObjectType, string &in, string &in)", 		            ( void * ) &gui_hostSetObjectFunctions},
	{"void as_updateCheckedStatus  (string &in, bool newState)",                                    ( void * ) &gui_updateCheckedStatus},
	{"void as_guiSetObjectFocus    (string &in)",                          ( void * ) &gui_setObjectFocus},
	{"string gui_getString         (string &in )",                         ( void * ) &gui_getString},
	{"void gui_addKeyAndText       (string &in, string &in)",              ( void * ) &gui_addKeyAndText},
	{"void gam_startNewGame        (int gameType)",                        ( void * ) &gam_startNewGame},
	{"",							nullptr},
};

//{"void as_gui_changeToGUIScreen		(int newScreen)", 					( const void * ) &gui_changeToGUIScreen},


//-----------------------------------------------------------------------------
//
// Structure to hold all the functions in the script
//-----------------------------------------------------------------------------
typedef struct
{
	asIScriptFunction 		*funcID{};
	bool 					fromScript{};         // Is this created from a console script
	string 					functionName;
	string 					scriptName;
	bool 					param1{};             // Does this function need something passed to it
} _scriptFunctionName;

//
// This is how we call a script from the host program
//
_scriptFunctionName     scriptFunctionName[] =
{
	// Name of function in script			                Name we call from host
	{0, false, "void as_addAllScriptCommands()",		    "scr_addAllScriptCommands",		NULL},
	{0, false, "void as_setGameVariables()",			    "scr_setGameVariables",         NULL},
//	{0, false, "cpVect as_vectTest(cpVect cpParam)",        "scr_vectTest",                 NULL},
	{0, false, "void as_setupGUI()",                        "scr_setupGUI",                 NULL},
	{0, false, "void as_guiHandleActionEvent(string &in)",  "scr_guiHandleActionEvent",     true},
	{0, false, "void as_setLanguageStrings()", 		        "as_setLanguageStrings", 		NULL},
	{0, false, "",						"",				NULL},
	{0, false, "",						"",				NULL},
	{0, false, "",						"",				NULL},
	{0, false, "",						"",				NULL},
	{0, false, "",						"",				NULL},
	// Need to have blank slots to hold the added commands from the script
};

//----------------------------------------------------------------------
//
// This is used to hold the function ID and is used when creating functions
// from within a running script. A copy of scriptFunctionName is used as
// the source data.
//
//----------------------------------------------------------------------
vector<_scriptFunctionName> scriptFunctions;

//----------------------------------------------------------------------
//
// Print out the variables that Angelscript knows about internally - DEBUGGING
void PrintVariables()
//----------------------------------------------------------------------
{
	asUINT  stackLevel = 0;

	asIScriptContext *ctx = asGetActiveContext();
	asIScriptEngine *engine = ctx->GetEngine();

	int typeId1 = ctx->GetThisTypeId();
	void *varPointer1 = ctx->GetThisPointer();
	if( typeId1 )
	{
		printf(" this = 0x%x\n", varPointer1);
	}

	int numVars = ctx->GetVarCount();
	for( asUINT n = 0; n < numVars; n++ )
	{
		int typeId = ctx->GetVarTypeId(n);
		void *varPointer = ctx->GetAddressOfVar(n);
		if( typeId == engine->GetTypeIdByDecl("int") )
		{
			printf(" %s = %d\n", ctx->GetVarDeclaration(n, stackLevel), *(int*)varPointer);
		}
		else if( typeId == engine->GetTypeIdByDecl("string") )
		{
			auto *str = (std::string*)varPointer;
			if( str )
				printf(" %s = '%s'\n", ctx->GetVarDeclaration(n, stackLevel), str->c_str());
			else
				printf(" %s = <null>\n", ctx->GetVarDeclaration(n, stackLevel));
		}
		else
		{
			auto *getValue = (cpVect *)varPointer;
			printf(" %s = {...}\n", ctx->GetVarDeclaration(n, stackLevel));
			printf("Debug [ %3.3f %3.3f ]\n", getValue->x, getValue->y);
		}
	}
}


//-----------------------------------------------------------------------------
//
// Error codes for AngelScript
const char *con_getScriptError ( int errNo )
//-----------------------------------------------------------------------------
{
	switch ( errNo )
		{
		case 0:
			return ( "asSUCCESS" );
			break;

		case -1:
			return ( "asERROR" );
			break;

		case -2:
			return ( "asCONTEXT_ACTIVE" );
			break;

		case -3:
			return ( "asCONTEXT_NOT_FINISHED" );
			break;

		case -4:
			return ( "asCONTEXT_NOT_PREPARED" );
			break;

		case -5:
			return ( "asINVALID_ARG" );
			break;

		case -6:
			return ( "asNO_FUNCTION" );
			break;

		case -7:
			return ( "asNOT_SUPPORTED" );
			break;

		case -8:
			return ( "asINVALID_NAME" );
			break;

		case -9:
			return ( "asNAME_TAKEN" );
			break;

		case -10:
			return ( "asINVALID_DECLARATION" );
			break;

		case -11:
			return ( "asINVALID_OBJECT" );
			break;

		case -12:
			return ( "asINVALID_TYPE" );
			break;

		case -13:
			return ( "asALREADY_REGISTERED" );
			break;

		case -14:
			return ( "asMULTIPLE_FUNCTIONS" );
			break;

		case -15:
			return ( "asNO_MODULE" );
			break;

		case -16:
			return ( "asNO_GLOBAL_VAR" );
			break;

		case -17:
			return ( "asINVALID_CONFIGURATION" );
			break;

		case -18:
			return ( "asINVALID_INTERFACE" );
			break;

		case -19:
			return ( "asCANT_BIND_ALL_FUNCTIONS" );
			break;

		case -20:
			return ( "asLOWER_ARRAY_DIMENSION_NOT_REGISTERED" );
			break;

		case -21:
			return ( "asAPP_CANT_INTERFACE_DEFAULT_ARRAY" );
			break;

		default:
			return ( "Unknown error type." );
		}
}

//-----------------------------------------------------------------------------
//
// Test setup passing a struct and values
bool sys_registerObject()
//-----------------------------------------------------------------------------
{
	int returnCode;

//	returnCode = scriptEngine->RegisterObjectType ( "cpVect", sizeof ( cpVect ), asOBJ_VALUE | asOBJ_POD ); //asOBJ_REF | asOBJ_NOCOUNT );
//	returnCode = scriptEngine->RegisterObjectType("cpVect", sizeof (cpVect), asOBJ_REF );

	returnCode = scriptEngine->RegisterObjectType("cpVect", sizeof( cpVect ), asOBJ_APP_CLASS | asOBJ_VALUE | asOBJ_POD);

	if ( returnCode < 0 )
	{
		con_print ( true, true, "Error RegisterObjectType [ %s ]", con_getScriptError ( returnCode ) );
		return false;
	}

	returnCode = scriptEngine->RegisterObjectProperty ( "cpVect", "float x", asOFFSET ( cpVect, x ) );

	if ( returnCode < 0 )
	{
		con_print ( true, true, "Error RegisterObjectProperty [ %s ]", con_getScriptError ( returnCode ) );
		return false;
	}

	returnCode = scriptEngine->RegisterObjectProperty ( "cpVect", "float y", asOFFSET ( cpVect, y ) );

	if ( returnCode < 0 )
	{
		con_print ( true, true, "Error RegisterObjectProperty 1 [ %s ]", con_getScriptError ( returnCode ) );
		return false;
	}

/*
	returnCode = scriptEngine->RegisterGlobalProperty ( "cpVect testVect", &testVecPosition );

	if ( returnCode < 0 )
	{
		con_print ( true, true, "Error RegisterGlobalProperty [ %s ]", con_getScriptError ( returnCode ) );
		return false;
	}
*/
	return true;
}


//-----------------------------------------------------------------------------
//
// Register all the functions to make available to the script
bool con_registerFunctions ()
//-----------------------------------------------------------------------------
{
	int 		r = 0;
	int 		count = 0;

	if ( !scriptEngineStarted )
		{
			con_print ( CON_ERROR, true, "Failed to register functions. ScriptEngine is not ready." );
			return false;
		}

	count = 0;
	numFunctionsInScripts = 0;		// Used later to copy the script info into vector array

	while ( scriptFunctionName[count].functionName.size() > 1 )
		{
			numFunctionsInScripts++;
			count++;
		}

	count = 0;
	numHostScriptFunctions = 0;

	while ( hostScriptFunctions[count].scriptFunctionName.size() > 1 )
		{
			r = scriptEngine->RegisterGlobalFunction ( hostScriptFunctions[count].scriptFunctionName.c_str(), asFUNCTION ( hostScriptFunctions[count].hostFunctionPtr ), callType );

			if ( r < 0 )
				{
					con_print (CON_ERROR, true, "Failed to registerGlobalFunction [ %s ]", hostScriptFunctions[count].scriptFunctionName.c_str() );
					con_print (CON_ERROR, true, "Error [ %s ]", con_getScriptError (r) );
					return false;
				}
			else
				{
//					con_print ( CON_INFO, true, "Registered function [ %s ]", hostScriptFunctions[count].scriptFunctionName.c_str() );
				}
			count++;
			numHostScriptFunctions++;
		}

	return true;
}


//-----------------------------------------------------------------------------
//
// Add global variables to be used by the scriptEngine
bool con_registerVariables ()
//-----------------------------------------------------------------------------
{
	int result = 0;
	int count = 0;

	if ( !scriptEngineStarted )
		{
			con_print (CON_ERROR, true, "Script: Failed to register variables. ScriptEngine is not ready." );
			return false;
		}

	count = 0;

	while ( hostVariables[count].scriptFunctionName.size() > 1 )
		{
			result = scriptEngine->RegisterGlobalProperty ( hostVariables[count].scriptFunctionName.c_str(), hostVariables[count].hostFunctionPtr );

			if ( result < 0 )
				{
					con_print (CON_ERROR, true, "Script: Error: Couldn't register variable - [ %s ]", hostVariables[count].scriptFunctionName.c_str() );
					return false;
				}
//			con_print (CON_INFO, true, "Script: Registered variable - [ %s ]", hostVariables[count].scriptFunctionName.c_str() );
			count++;
		}

	return true;
}

//-----------------------------------------------------------------------------
//
// Get the script function ID's and cache them
bool con_cacheFunctionIDs ()
//-----------------------------------------------------------------------------
{
	_scriptFunctionName tempFunctionName;

	//
	// Get memory to hold script details
	//
	scriptFunctions.reserve ( numFunctionsInScripts );

//	con_print (CON_ERROR, true, "Number of functions in script [ %i ]", numFunctionsInScripts );

	//
	// Get function ID's for each function we will call in the script
	//
	for ( unsigned int i = 0; i != numFunctionsInScripts; i++ )
		{
			tempFunctionName.funcID = scriptEngine->GetModule ( "ModuleName" )->GetFunctionByDecl ( scriptFunctionName[i].functionName.c_str() );

			if ( tempFunctionName.funcID <= 0 )
				{
					con_print (CON_ERROR, true, "Failed to get function ID for [ %s ]", scriptFunctionName[i].functionName.c_str() );
//					return false;
				}

			else
				{
//					con_print (CON_ERROR, false, "Func ID for [ %s ] - [ %i ]", scriptFunctionName[i].functionName.c_str(), tempFunctionName.funcID );
				}

			//
			// Setup malloc'd memory with the funcID and name to execute
			//
			tempFunctionName.param1 = scriptFunctionName[i].param1;
			tempFunctionName.functionName = scriptFunctionName[i].functionName;
			tempFunctionName.scriptName = scriptFunctionName[i].scriptName;
			tempFunctionName.fromScript = scriptFunctionName[i].fromScript;

			scriptFunctions.push_back ( tempFunctionName );

			//		con_print (true, true, "scriptFunctions [ %s ] has ID [ %i ]", scriptFunctions[i].functionName.c_str(), scriptFunctions[i].funcID);
		}

	// Do some preparation before execution
	scriptContext = scriptEngine->CreateContext();

	return true;
}

//-----------------------------------------------------------------------------
//
// Load the scripts into memory
bool con_loadAndCompileScripts ()
//-----------------------------------------------------------------------------
{
	int				fileCounter = 0;
	int				retCode = 0;
	char			*fileResults = nullptr;
	PHYSFS_sint64	fileLength;

	retCode = builder.StartNewModule ( scriptEngine, "ModuleName" );

	if ( retCode < 0 )
		{
			con_print (CON_ERROR, true, "Failed to start script module." );
			return false;
		}

	while ( scriptInfo[fileCounter].scriptFileName.size() > 1 )
		{
			//
			// Load the script file
			//
			fileLength = io_getFileSize(scriptInfo[fileCounter].scriptFileName.c_str());
			if (fileLength < 0)
				{
					con_print(CON_ERROR, true, "Failed to locate script file [ %s ].", scriptInfo[fileCounter].scriptFileName.c_str());
					return false;
				}

			fileResults = (char *)malloc( static_cast<size_t>(fileLength));
			if (nullptr == fileResults)
				{
					con_print(CON_ERROR, true, "Failed to get memory to hold script file [ %s ].", scriptInfo[fileCounter].scriptFileName.c_str());
					return false;
				}

			if (-1 == io_getFileIntoMemory(scriptInfo[fileCounter].scriptFileName.c_str(), fileResults))
				{
					con_print (CON_ERROR, true, "Failed to load script file [ %s ].", scriptInfo[fileCounter].scriptFileName.c_str() );
					return false;
				}

			retCode = builder.AddSectionFromMemory(scriptInfo[fileCounter].scriptName.c_str(), fileResults, (unsigned int)fileLength, 0);
			if ( retCode < 0 )
				{
					free (fileResults);        // Prevent memory leak if it fails to build

					con_print (CON_ERROR, true, "Failed to add script file [ %s ].", scriptInfo[fileCounter].scriptFileName.c_str() );
					return false;
				}

			fileCounter++;

			free (fileResults);
			fileResults = nullptr;
		}

	//
	// Build the script from the loaded sections using ScriptBuilder
	//
	if ( builder.BuildModule() < 0 )
		{
			con_print (CON_ERROR, true, "Failed to build the script module." );
			return false;
		}

	//
	// Free memory used to hold scripts while they are loaded into engine
	// if memory already allocated - free and remalloc it
	//
	if (fileResults)
		{
			free (fileResults);
			fileResults = nullptr;
		}

	return true;
}

//-----------------------------------------------------------------------------
//
// Execute a function from the script
bool con_executeScriptFunction ( string functionName, string funcParam )
//-----------------------------------------------------------------------------
{
	unsigned int i = 0;

	//
	// Check for function name passed in
	if ( functionName.empty ())
		{
			con_print (CON_ERROR, false, "Error: Failed to provide function name." );
			return false;
		}

	//
	// Safety check first
	if ( nullptr == scriptEngine )
		{
			con_print (CON_ERROR, true, "Couldn't execute [ %s ]. Engine not ready.", functionName.c_str() );
			return false;
		}

	if ( nullptr == scriptContext )
		{
			con_print (CON_ERROR, false, "Couldn't execute [ %s ]. Script context not ready.", functionName.c_str() );
			return false;
		}

	//
	// Find the functionID we need for the functionName
	for ( i = 0; i != numHostScriptFunctions; i++ )
		{
			//
			// Do a case insensitive compare - FIXME
			//
#if defined(WIN32)
			if ( _stricmp ( scriptFunctions[i].scriptName.c_str(), functionName.c_str() ) == 0 )
#else
			if ( scriptFunctions[i].scriptName == functionName )
#endif
				break;
		}

	if ( i == numHostScriptFunctions )
		{
			con_print (CON_ERROR, false, "Couldn't find function name [ %s ].", functionName.c_str() );
			return false;
		}

	//	con_print (true, false, "Found [ %s ] to execute", scriptFunctions[i].functionName.c_str());

	//
	// Prepare the engine context to execute the function
	//
	int j = scriptContext->Prepare ( scriptFunctions[i].funcID );

	if ( j < 0 )
		{
			con_print (CON_ERROR, true, "Failed to prepare the context for [ %s ].", scriptFunctions[i].functionName.c_str() );
			return false;
		}

	//	con_print (true, false, "Context for [ %s ] has been prepared.", scriptFunctions[i].functionName.c_str());
	//
	// Pass the parameter to the function if there is one
	//
	int testInt;
/*
	if ("scr_vectTest" == functionName)
	{
		printf("Run vecTest\n");
		vect1.x = 987;
		vect1.y = 456;
		scriptContext->SetArgObject(0, &vect1);
	}
*/

	if ( scriptFunctions[i].param1 )
		{
			//
			// See if it's a number or a string - check first character of the string only
			if ( isdigit ( funcParam.c_str() [0] ) )
				{
					//
					// Parameter is a number - convert before passing in
					//
					testInt = atoi ( funcParam.c_str() );
					scriptContext->SetArgAddress ( 0, &testInt );
				}
			else
				{
					//
					// Send in string value
					//
					scriptContext->SetArgAddress ( 0, &funcParam );
				}
		}

	//
	// Now run it
	//
	int ret = scriptContext->Execute();

	if ( ret != asEXECUTION_FINISHED )
		{
			//
			// The execution didn't finish as we had planned. Determine why.
			//
			if ( ret == asEXECUTION_ABORTED )
				con_print (CON_ERROR, true, "Script: The script was aborted before it could finish. Probably it timed out." );

			else if ( ret == asEXECUTION_EXCEPTION )
				{
					con_print (CON_ERROR, true, "Script: The script ended with an exception." );
					//
					// Write some information about the script exception
					//
					asIScriptFunction *func = scriptContext->GetExceptionFunction();
					con_print (CON_ERROR, true, "Func: [ %s ]", func->GetDeclaration() );
					con_print (CON_ERROR, true, "Module: [ %s ]", func->GetModuleName() );
					con_print (CON_ERROR, true, "Section: [ %s ]", func->GetScriptSectionName() );
					con_print (CON_ERROR, true, "Line: [ %i ]", scriptContext->GetExceptionLineNumber() );
					con_print (CON_ERROR, true, "Desc: [ %s ]", scriptContext->GetExceptionString() );
				}
			else
				con_print (CON_ERROR, true, "The script ended for an unknown reason [ %i ].", ret );

			return false;
		}
	return true;
}

//------------------------------------------------------------------
//
// Return messages to console from script engine
void con_scriptMessageCallback ( const asSMessageInfo *msg, void *param )
//------------------------------------------------------------------
{
	string messageType;

	messageType = "ERROR";

	if ( msg->type == asMSGTYPE_WARNING )
		messageType = "WARN";

	else if ( msg->type == asMSGTYPE_INFORMATION )
		messageType = "INFO";

	printf ( "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, messageType.c_str(), msg->message );
	con_print (CON_ERROR, true, "%s (%d, %d) : %s : %s", msg->section, msg->row, msg->col, messageType.c_str(), msg->message );
}

//----------------------------------------------------------------
//
// Start the script engine - no script registrations yet
bool con_startScriptEngine ()
//----------------------------------------------------------------
{
	// Create the script engine
	scriptEngine = asCreateScriptEngine();

	if ( scriptEngine == nullptr )
		{
			con_print ( CON_ERROR, false, "Failed to create script engine." );
			scriptEngineStarted = false;
			return false;
		}

	//
	// The script compiler will write any compiler messages to the callback.
	scriptEngine->SetMessageCallback (asFUNCTION (con_scriptMessageCallback), nullptr, asCALL_CDECL);

	//
	// Find out how to call functions
	if ( !strstr ( asGetLibraryOptions(), "AS_MAX_PORTABILTY" ) )
		{
			callType = asCALL_CDECL;
//			con_print (CON_ERROR, true, "Script: Call type [ %s ]", "asCALL_CDECL" );
		}

	else
		{
			callType = asCALL_STDCALL;
//			con_print (CON_ERROR, true, "Script: Call type [ %s ]", "asCALL_STDCALL" );
		}

	// Register the script string type
	// Look at the implementation for this function for more information
	// on how to register a custom string type, and other object types.
	RegisterStdString ( scriptEngine );

//	scriptEngine->RegisterObjectMethod("Vec3", "Vec3& opAssign(const Vec3 &in)", asMETHODPR(glm::vec3, operator=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);

//	int r = scriptEngine->RegisterObjectType("Vec3", sizeof(glm::vec3), asOBJ_VALUE | asGetTypeTraits<glm::vec3>()); assert( r >= 0 );

//	scriptEngine->RegisterObjectMethod("Vec3", "Vec3& opAssign(const Vec3 &in)", asMETHODPR(glm::vec3, operator=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);

	sys_registerObject();

	scriptEngineStarted = true;

	return true;
}

//-----------------------------------------------------------------------------
//
// Add a script console command to the executable list of functions
//
// funcName - what the console command is
// funcPtr - which function gets called from the console command
//
bool con_addScriptConsoleFunction ( string funcName, string funcPtr, bool setParam )
//-----------------------------------------------------------------------------
{
	_scriptFunctionName tempScriptFunction;

	tempScriptFunction.functionName = funcPtr; //std::move (funcPtr);
	tempScriptFunction.scriptName = funcName; //std::move (funcName);
	tempScriptFunction.fromScript = true;
	tempScriptFunction.param1 = setParam;

	tempScriptFunction.funcID = scriptEngine->GetModule ( "ModuleName" )->GetFunctionByDecl ( tempScriptFunction.functionName.c_str() );

	if ( tempScriptFunction.funcID == nullptr )
		{
			con_print ( CON_ERROR, true, "Script: Failed to get function ID for [ %s ].", tempScriptFunction.functionName.c_str() );
			return false;
		}

//	con_print ( true, true, "FuncID for script function [ %s ] is [ %i ]", tempScriptFunction.functionName.c_str(), tempScriptFunction.funcID );

	// TODO: Handle adding parameter value

	scriptFunctions.push_back ( tempScriptFunction );
	numScriptFunctions++;
	return true;
}

//-----------------------------------------------------------------------------
//
// Shutdown the scripting engine
bool con_shutDownScriptEngine ()
//-----------------------------------------------------------------------------
{
	scriptContext->Release ();
	scriptEngine->Release ();
	con_print ( CON_INFO, true, "Script engine released." );
	return true;
}

//-----------------------------------------------------------------------------
//
// Functions to be called from the scripts
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Print to console from the scripts - String version
void con_scriptPrintStr ( std::string *msgText, std::string *msgParam )
//-----------------------------------------------------------------------------
{
	con_print ( CON_TEXT, true, "[ %s ] [ %s ]", msgText->c_str(), msgParam->c_str() );
}
