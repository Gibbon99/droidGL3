#include <hdr/io/io_fileSystem.h>
#include <hdr/console/con_conScript.h>
#include "hdr/system/sys_main.h"
#include "hdr/libGL/gl_window.h"
#include "sys_timing.h"

#ifdef GLAD_DEBUG

typedef void (*GLADcallback) ( const char *name, void *funcptr, int len_args, ... );

//-----------------------------------------------------------------------------------------------------
//
// Sets a callback which will be called before every function call to a function loaded by glad.
void pre_gl_call ( const char *name, void *funcptr, int len_args, ... )
//-----------------------------------------------------------------------------------------------------
{
	// printf("Calling: %s (%d arguments)\n", name, len_args);
}

#endif


//-----------------------------------------------------------------------------------------------------
//
/// \param None
/// \return True / False on state of starting a system
//
// Start all the systems up
bool sys_initAll()
//-----------------------------------------------------------------------------------------------------
{
	// Init the console
	con_initConsole ();

	if (!lib_openWindow ())
		return false;

	if ( !gladLoadGL ())
	{
		con_print (CON_ERROR, true, "Failed to initialize GLAD");
		return false;
	}

#ifdef GLAD_DEBUG
	// before every opengl call call pre_gl_call
	glad_set_pre_callback (pre_gl_call);
	// don't use the callback for glClear
	// (glClear could be replaced with your own function)
	//glad_debug_glClear = glad_glClear;
#endif

	con_print (CON_INFO, true, "OpenGL system version [ %d.%d ]", GLVersion.major, GLVersion.minor);
	if ( GLVersion.major < 2 )
	{
		con_print (CON_ERROR, true, "Your system doesn't support OpenGL >= 2.");
		return false;
	}

	sys_initTimingVars ();

	//----------------------------------------------------------------------------------------
	//
	// Console running now
	//
	//----------------------------------------------------------------------------------------

	if (!io_startFileSystem ())
	{
		con_print(CON_ERROR, true, "Could not start filesystem.");
//		return false;
	}
	con_print(CON_INFO, true, "Filesystem started.");

	if ( !util_startScriptEngine ())
	{
		con_print (CON_ERROR, true, "Error: Unable to start scripting engine.");
		return false;
	}

	util_registerVariables ();
	util_registerFunctions ();
	util_loadAndCompileScripts ();
	util_cacheFunctionIDs ();

}