#pragma once

extern bool 		scriptEngineStarted;
extern unsigned int	numFunctionsInScripts;
extern unsigned int numHostScriptFunctions;

bool con_startScriptEngine ();
bool con_registerVariables ();
bool con_registerFunctions ();
bool con_loadAndCompileScripts ();
bool con_cacheFunctionIDs ();

bool con_addScriptConsoleFunction ( std::string funcName, std::string funcPtr, bool setParam );

// Shutdown the scripting engine
bool con_shutDownScriptEngine ();