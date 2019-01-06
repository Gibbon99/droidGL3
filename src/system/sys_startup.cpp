#include "hdr/io/io_gameprefs.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_render.h"
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_doors.h"
#include "hdr/game/gam_physicsCollisions.h"
#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/gui/gui_main.h"
#include "hdr/gui/gui_sideview.h"
#include "hdr/opengl/gl_fbo.h"
#include "hdr/opengl/gl_shaders.h"
#include "hdr/io/io_fileSystem.h"
#include "hdr/console/con_conScript.h"
#include "hdr/game/gam_physics.h"
#include "hdr/system/sys_main.h"
#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_timing.h"
#include "hdr/system/sys_audio.h"

#ifdef GLAD_DEBUG

typedef void (*GLADcallback) ( const char *name, void *funcptr, int len_args, ... );

bool g_memLeakLastRun;

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

	if (!io_getGamePrefs(GAME_PREFS_FILENAME))
		return false;

	/*
	if ( sys_checkMemLeak ("leakReport.txt"))
	{
		con_print (CON_INFO, true, "No memory leak from last run.");
		g_memLeakLastRun = false;
	}
	else
	{
		con_print (CON_ERROR, true, "MEMORY LEAK: Check logfile for details.");
		g_memLeakLastRun = true;
	}
*/
	if (!lib_openWindow ())
		return false;

#ifdef GLAD_DEBUG
	// before every opengl call call pre_gl_call
//	glad_set_pre_callback (pre_gl_call);
	// don't use the callback for glClear
	// (glClear could be replaced with your own function)
	//glad_debug_glClear = glad_glClear;
#endif


	if ( !gladLoadGL ())
	{
		con_print (CON_ERROR, true, "Failed to initialize GLAD");
		return false;
	}

#if (DEBUG_LEVEL > 0)
	gl_registerDebugCallback ();
#endif

	con_print (CON_INFO, true, "OpenGL system version [ %d.%d ]", GLVersion.major, GLVersion.minor);
	if ( GLVersion.major < 3 )
	{
		con_print (CON_ERROR, true, "Your system doesn't support OpenGL >= 3.x");
		return false;
	}

	if ( !evt_registerUserEventSetup ())      // Start all the threads
		return false;

	sys_initTimingVars ();

	sys_changeMode (MODE_LOADING);

	//----------------------------------------------------------------------------------------
	//
	// Console running now
	//
	//----------------------------------------------------------------------------------------
	if (!io_startFileSystem ())
	{
		con_print(CON_ERROR, true, "Could not start filesystem.");
		return false;
	}
	else    // Can now run everything needing the filesystem
	{
		con_print(CON_INFO, true, "Filesystem started.");

		evt_sendEvent(USER_EVENT_LOGGING, USER_EVENT_LOGGING_START, 0, 0, 0, vec2(), vec2(), "logfile.log");

		if (!con_startScriptEngine ())
		{
			con_print(CON_ERROR, true, "Error: Unable to start scripting engine.");
			return false;
		}
		else
		{
			con_registerVariables ();
			con_registerFunctions ();
			con_loadAndCompileScripts ();
			con_cacheFunctionIDs ();

			con_executeScriptFunction ("scr_setGameVariables", "");
			//
			// Crashes with an exception around the strings on Windows
//	con_executeScriptFunction ("scr_addAllScriptCommands", "");
		}

		sys_setupPhysicsEngine ();
		sys_setupPlayerPhysics ();
		sys_setupCollisionHandlers ();

		if (!gl_addShaders())
		{
			con_print (CON_ERROR, true, "Unable to add shaders.");
			return false;
		}

		evt_sendEvent (USER_EVENT_AUDIO, AUDIO_INIT_ENGINE, 0, 0, 0, vec2(), vec2(), "");

		io_loadTileTextureFile("retro", "blue");

		io_replaceTileTexture ("retro", "gray" );

		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_LOAD, 0, 0, 0, vec2(), vec2(), "splash.png");
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_LOAD, 0, 0, 0, vec2(), vec2(), "lightmap.bmp");
		evt_sendEvent (USER_EVENT_TEXTURE, USER_EVENT_TEXTURE_LOAD, 0, 0, 0, vec2(), vec2(), "hud.tga");

		gl_setupDroidToSpriteLookup ();

		lvl_loadAllLevels ();

		gam_setHealingState (false);
		gam_initHealingAnimateTimer (healingAnimateInterval);

		gam_setDoorAnimateState (false);
        gam_initDoorAnimateTimer (doorAnimateInterval);

		gam_setPlayerAnimateState ( false );
		gam_initPlayerAnimateTimer (150 );

		if (!gui_initGUI())
		{
			con_print(CON_ERROR, true, "Unable to start GUI system.");
			return false;
		}

		if (!gui_loadTTFFont("Digital.ttf", 48, "fontDigital"))
			return false;

		if (!gui_loadTTFFont("Digital.ttf", 32, "fontDigital32"))
			return false;

		if (!gui_loadSideViewData( "sideview.dat" ))
			return false;

		gui_prepareGUI();

	}   // end of file system check

//	gl_getFramebufferLimits();
//	gl_getFramebufferInfo (GL_DRAW_FRAMEBUFFER_BINDING, gl_getFrameBufferTexture ());

	return true;
}
