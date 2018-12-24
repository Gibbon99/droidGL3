#include "hdr/system/sys_main.h"
#include "hdr/console/con_console.h"
#include "hdr/libGL/gl_window.h"

SDL_Window      *mainWindow;
SDL_GLContext   mainContext;    // The OpenGL context handle

int             winWidth, winHeight;    // Screen size
bool            fullScreen;
int             vsyncType;      // -1 is adaptive vsync; 0 = off

//-----------------------------------------------------------------------------
//
// Swap buffers for displaying screen
void lib_swapBuffers ()
//-----------------------------------------------------------------------------
{
	SDL_GL_SwapWindow (mainWindow);;
}

//-----------------------------------------------------------------------------
//
// Shutdown the library windowing system
void lib_destroyWindow ()
//-----------------------------------------------------------------------------
{
	SDL_DestroyWindow (mainWindow);
}

//-----------------------------------------------------------------------------
//
// Get the version of the window library
bool lib_getVersion ()
//-----------------------------------------------------------------------------
{
	SDL_version compiled;
	SDL_version linked;

	SDL_VERSION (&compiled);
	SDL_GetVersion (&linked);

	con_print (CON_INFO, true, "Compiled against SDL version [ %d.%d.%d ]", compiled.major, compiled.minor, compiled.patch);
	con_print (CON_INFO, true, "Linked against SDL version [ %d.%d.%d ]", linked.major, linked.minor, linked.patch);

	return true;
}

//-----------------------------------------------------------------------------
//
// Resize the current displayed window
void lib_resizeWindow ( int newWidth, int newHeight )
//-----------------------------------------------------------------------------
{
//	al_resize_display(al_mainWindow, newWidth, newHeight);
	// TODO: Make this work for SDL2

	winWidth = newWidth;
	winHeight = newHeight;
}

int lib_getWindowInfo()
{
	static int display_in_use = 0; /* Only using first display */

	int i, display_mode_count;
	SDL_DisplayMode mode;
	Uint32 f;

	con_print (CON_INFO, true, "SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays ());

	display_mode_count = SDL_GetNumDisplayModes (display_in_use);
	if ( display_mode_count < 1 )
	{
		con_print (CON_INFO, true, "SDL_GetNumDisplayModes failed : [ %s ]", SDL_GetError ());
		return 1;
	}

	SDL_Log ("SDL_GetNumDisplayModes: %i", display_mode_count);

	for ( i = 0; i < display_mode_count; ++i )
	{
		if ( SDL_GetDisplayMode (display_in_use, i, &mode) != 0 )
		{
			SDL_Log ("SDL_GetDisplayMode failed: %s", SDL_GetError ());
			return 1;
		}

		f = mode.format;

		SDL_Log ("Mode %i\tbpp %i\t%s\t%i x %i Refresh [ %i ]", i, SDL_BITSPERPIXEL(f), SDL_GetPixelFormatName (f), mode.w, mode.h, mode.refresh_rate);
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
// Start the GL Helper library and open the window
bool lib_openWindow ()
//-----------------------------------------------------------------------------
{
	int numVideoDrivers = 0;

	// Initialize SDL
	if ( SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
		return false;
	/*
	numVideoDrivers = SDL_GetNumVideoDrivers ();

	for ( int i = 0; i != numVideoDrivers; i++ )
	{
		con_print (CON_INFO, true, "Video driver [ %i ] - [ %s ]", i, SDL_GetVideoDriver (i));
	}

	con_print (CON_INFO, true, "Num video displays [ %i ]", SDL_GetNumVideoDisplays ());
*/

	// Create the window centered at resolution
	Uint32 windowFlags;

	windowFlags = SDL_WINDOW_OPENGL;
	if (fullScreen)
		windowFlags |= SDL_WINDOW_FULLSCREEN;

	mainWindow = SDL_CreateWindow ("droidGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, windowFlags);

	// Was the window created ok
	if ( !mainWindow )
	{
		con_print (CON_ERROR, true, "Unable to create window : [ %s ]", SDL_GetError ());
		return false;
	}

	SDL_Delay(1000);

	/*
	// TODO: Parse the modes and get the refresh rate
	if ( fullScreen )
	{
		SDL_DisplayMode setDisplayMode;

		setDisplayMode.refresh_rate = 75;
		setDisplayMode.w = winWidth;
		setDisplayMode.h = winHeight;
		setDisplayMode.format = SDL_PIXELFORMAT_RGB888;

		if ( SDL_SetWindowDisplayMode (mainWindow, &setDisplayMode) != 0 )
		{
			con_print (CON_ERROR, true, "Unable to set displaymode [ %s ]", SDL_GetError ());
			printf ("Display error [ %s ]\n", SDL_GetError ());
			sys_shutdownToSystem ();
		}
	}
*/

	// Set the OpenGL version.
	// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

#if (DEBUG_LEVEL > 0)
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	// Create the opengl context and attach it to the window
	mainContext = SDL_GL_CreateContext (mainWindow);
	if ( nullptr == mainContext )
	{
		con_print (CON_ERROR, true, "Unable to create OpenGL context : [ %s ]", SDL_GetError ());
		return false;
	}

	if ( SDL_GL_MakeCurrent (mainWindow, mainContext) < 0 )
	{
		con_print (CON_ERROR, true, "Context error [ %s ]", SDL_GetError ());
		return false;
	}

	// Turn on double buffering with a 24bit Z buffer.
	// May need to change this to 16 or 32
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	if (-1 == SDL_GL_SetSwapInterval (vsyncType))
	{
		con_print(CON_ERROR, true, "Unable to use selected vsync method : [ %s ]", SDL_GetError ());
		SDL_GL_SetSwapInterval (1);
	}

	int value = 0;

	SDL_GL_GetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, &value);
	con_print (CON_INFO, true, "SDL_GL_CONTEXT_MAJOR_VERSION : %i ", value);

	SDL_GL_GetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, &value);
	con_print (CON_INFO, true, "SDL_GL_CONTEXT_MINOR_VERSION: %i ", value);

	SDL_GL_GetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &value);
	con_print (CON_INFO, true, "SDL_GL_FRAMEBUFFER_SRGB_CAPABLE: %i :", value);

	sys_changeMode (MODE_CONSOLE);

	int width, height;

	SDL_GetWindowSize(mainWindow, &width, &height);

	printf("SDL_GetWindowSize size [ %i %i ]\n", width, height);

//	lib_getWindowInfo ();
	return true;
}
