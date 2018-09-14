#pragma once

extern bool 		scriptEngineStarted;
extern unsigned int	numFunctionsInScripts;
extern unsigned int numHostScriptFunctions;

bool util_startScriptEngine();
bool util_registerVariables();
bool util_registerFunctions();
bool util_loadAndCompileScripts();
bool util_cacheFunctionIDs();

bool sys_addScriptConsoleFunction ( string funcName, string funcPtr, bool setParam );
