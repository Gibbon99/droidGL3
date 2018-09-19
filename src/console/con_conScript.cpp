
#include "hdr/io/io_fileSystem.h"
#include "hdr/script/as_scriptbuilder.h"
#include "hdr/io/io_logfile.h"
#include "hdr/physfs/physfs.h"

unsigned int		numFunctionsInScripts;
unsigned int		numHostScriptFunctions;
bool				scriptEngineStarted = false;
asDWORD				callType;
asIScriptEngine 	*scriptEngine = NULL;
asIScriptContext 	*scriptContext = NULL;
CScriptBuilder 		builder;

// Print messages from script compiler to console
void scr_Output ( const char *msgText, void *outParam );

void MessageCallback ( const asSMessageInfo *msg, void *param );

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

//static char		    *fileLocation;				// Pointer to memory to hold the scriptFile
int				    numScriptFunctions = 0;		// Number of functions declared in script
asIScriptContext    *context = NULL;
asIScriptModule     *myModule= NULL;

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
	{"conCommands.as",            "script"},
	{"",            ""},
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
	{"void printCon_AS(string &in, string &in)",            ( void * )&sys_scriptPrintStr},

	{"void sys_addScriptCommand(string &in, string &in, string &in, bool setParam)", ( void * ) &con_addScriptCommand},

	{"void conPushCommand_AS(string &in)",                              (void * ) con_pushScriptCommand},
	{"bool startPackFile(string &in, string &in)",                      (void * ) io_startFileSystem},
	{"bool startLogFile(string &in)",                                   (void * ) io_startLogFile},
//	{"void lib_resizeWindow(int newWidth, int newHeight)",              (void * ) lib_resizeWindow},
//	{"void sdf_addFontInfo(uint whichFont, string &in, uint fontSize)", (void * ) sdf_addFontInfo},
//	{"bool sdf_initFontSystem()",	                                    (void * ) sdf_initFontSystem},
	{"void sys_changeMode(int newMode)",                                (void * ) changeMode},
//	{"void aud_setAudioGain(int newLevel)",                             (void * ) aud_setAudioGain},
	{"",							NULL},
};

//-----------------------------------------------------------------------------
//
// Structure to hold all the functions in the script
//-----------------------------------------------------------------------------
typedef struct
{
	asIScriptFunction 		*funcID;
	bool 					fromScript; // Is this created from a console script
	string 					functionName;
	string 					scriptName;
	bool 					param1; // Does this function need something passed to it
} _scriptFunctionName;

//
// This is how we call a script from the C program
//
_scriptFunctionName     scriptFunctionName[] =
{
	// Name of function in script			Name we call from host
	{0, false, "void as_addAllScriptCommands()",		"scr_addAllScriptCommands",		NULL},
	{0, false, "void as_setGameVariables()",			"scr_setGameVariables",         NULL},
	{0, false, "void as_loadAllModels()",				"scr_loadAllModels",			NULL},
	{0, false, "void as_loadParticleTexture()",         "scr_loadParticleTextures",     NULL},
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

//-----------------------------------------------------------------------------
//
// Error codes for AngelScript
const char *sys_getScriptError ( int errNo )
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
// Register all the functions to make available to the script
bool util_registerFunctions()
//-----------------------------------------------------------------------------
{
	int 		r = 0;
	int 		count = 0;

	string test1, test2;

	if ( false == scriptEngineStarted )
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
					con_print (CON_ERROR, true, "Error [ %s ]", sys_getScriptError ( r ) );
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
bool util_registerVariables()
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
			result = scriptEngine->RegisterGlobalProperty ( hostVariables[count].scriptFunctionName.c_str(), ( void * ) hostVariables[count].hostFunctionPtr );

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
bool util_cacheFunctionIDs()
//-----------------------------------------------------------------------------
{
	_scriptFunctionName tempFunctionName;

	//
	// Get memory to hold script details
	//
	scriptFunctions.reserve ( numFunctionsInScripts );

	con_print (CON_ERROR, true, "Number of functions in script [ %i ]", numFunctionsInScripts );

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
bool util_loadAndCompileScripts()
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

			fileResults = (char *)malloc(fileLength);
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
					con_print (CON_ERROR, true, "Failed to add script file [ %s ].", scriptInfo[fileCounter].scriptFileName.c_str() );
					return false;
				}

			fileCounter++;

			if (fileResults)
			{
				free (fileResults);        // Prevent memory leak if it fails to build
				fileResults = nullptr;
			}
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
bool util_executeScriptFunction ( string functionName, string funcParam )
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
			con_print (CON_ERROR, false, "Couldn't execute [ %s ]. Script context not ready", functionName.c_str() );
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
			con_print (CON_ERROR, false, "Couldn't find function name [ %s ]", functionName.c_str() );
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

	if ( scriptFunctions[i].param1 )
		{
//		io_logToFile("Parameter to pass to script [ %s ]", funcParam.c_str());
			//
			// See if it's a number or a string - check first character of the string only
			if ( isdigit ( funcParam.c_str() [0] ) )
				{
					//
					// Paramater is a number - convert before passing in
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
				con_print (CON_ERROR, true, "The script ended for some unforeseen reason [ %i ]", ret );

			return false;
		}

	return true;
}

//------------------------------------------------------------------
//
// Return messages to console from script engine
void util_scriptMessageCallback ( const asSMessageInfo *msg, void *param )
//------------------------------------------------------------------
{
	string messageType;

	messageType = "ERROR";

	if ( msg->type == asMSGTYPE_WARNING )
		messageType = "WARN";

	else if ( msg->type == asMSGTYPE_INFORMATION )
		messageType = "INFO";

	printf ( "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, messageType.c_str(), msg->message );
	con_print (CON_ERROR, false, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, messageType.c_str(), msg->message );
}

//----------------------------------------------------------------
//
// Start the script engine - no script registrations yet
bool util_startScriptEngine()
//----------------------------------------------------------------
{
	// Create the script engine
	scriptEngine = asCreateScriptEngine();

	if ( scriptEngine == 0 )
		{
			con_print ( CON_ERROR, false, "Failed to create script engine." );
			scriptEngineStarted = false;
			return false;
		}

	//
	// The script compiler will write any compiler messages to the callback.
	scriptEngine->SetMessageCallback ( asFUNCTION ( util_scriptMessageCallback ), 0, asCALL_CDECL );

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
bool sys_addScriptConsoleFunction ( string funcName, string funcPtr, bool setParam )
//-----------------------------------------------------------------------------
{
	_scriptFunctionName tempScriptFunction;

	tempScriptFunction.functionName = funcPtr;
	tempScriptFunction.scriptName = funcName;
	tempScriptFunction.fromScript = true;
	tempScriptFunction.param1 = setParam;

	tempScriptFunction.funcID = scriptEngine->GetModule ( "ModuleName" )->GetFunctionByDecl ( tempScriptFunction.functionName.c_str() );

	if ( tempScriptFunction.funcID == NULL  )
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
bool sys_shutDownScriptEngine()
//-----------------------------------------------------------------------------
{
	context->Release();
	con_print ( CON_INFO, true, "Script: Script engine released." );
	return true;
}

//-----------------------------------------------------------------------------
//
// Functions to be called from the scripts
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Print messages from script compiler to console
void scr_Output ( const char *msgText, void *outParam )
//-----------------------------------------------------------------------------
{
	con_print ( CON_INFO, true, "Script > [ %s ]", msgText );
}

void MessageCallback ( const asSMessageInfo *msg, void *param )
{
	const char *type = "ERR ";

	if ( msg->type == asMSGTYPE_WARNING )
		type = "WARN";

	else if ( msg->type == asMSGTYPE_INFORMATION )
		type = "INFO";

	con_print(CON_INFO, true, "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	printf ( "%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message );
}

//-----------------------------------------------------------------------------
//
// Print to console from the scripts - String version
void sys_scriptPrintStr ( std::string *msgText, std::string *msgParam )
//-----------------------------------------------------------------------------
{
	con_print ( CON_TEXT, true, "[ %s ] [ %s ]", msgText->c_str(), msgParam->c_str() );
}
