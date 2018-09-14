#include <hdr/system/sys_embedFont.h>
#include "hdr/system/sys_main.h"

#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_startup.h"
#include "data/scripts/commonDefines.h"
#include "sys_timing.h"

bool    quitProgram;
int     loops;
float   interpolation;
int     currentMode;

//-----------------------------------------------------------------------------------------------------
//
/// \param Pass in interpolation for the frame
/// \return None
void sys_displayScreen(float interpolation)
//-----------------------------------------------------------------------------------------------------
{
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glViewport (0, 0, winWidth, winHeight);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (currentMode)
	{
		case MODE_CONSOLE:
			con_showConsole ();
			break;

		default:
			break;
	}

	fnt_printText (vec2{0,winHeight - 16}, vec4{1,1,1,1}, "FPS [ %i ] Think [ %i ]", fpsPrint, thinkFpsPrint);
	lib_swapBuffers ();
}

//-----------------------------------------------------------------------------------------------------
//
/// \param Called to update all elements according to the set thinkFPS
/// \return None
void sys_gameTickRun()
//-----------------------------------------------------------------------------------------------------
{
	switch (currentMode)
	{
		case MODE_SHUTDOWN:
			quitProgram = true;
			break;

		default:
			break;
	}

	evt_handleEvents ();
}

//-----------------------------------------------------------------------------------------------------
//
// Run main loop
//
// Fixed update frames
// Render as fast as possible
int main (int argc, char *argv[] )
//-----------------------------------------------------------------------------------------------------
{
	Uint32 next_game_tick = SDL_GetTicks();

	sys_initAll();

	while ( !quitProgram )
	{
		loops = 0;

		evt_handleEvents();

		while (SDL_GetTicks() > next_game_tick && loops < MAX_FRAMESKIP)
		{
			sys_gameTickRun ();
			next_game_tick += SKIP_TICKS;
			loops++;
			thinkFPS++;
		}

		interpolation = float( SDL_GetTicks() + SKIP_TICKS - next_game_tick ) / float( SKIP_TICKS );

		sys_displayScreen ( interpolation );

		fps++;
	}

sys_shutdownToSystem();
}


//-----------------------------------------------------------------------------
//
// Change game mode
void changeMode ( int newMode )
//-----------------------------------------------------------------------------
{
	static int previousMode = -1;

	if ( -1 == newMode )
	{
		currentMode = previousMode;
		return;
	}

	previousMode = currentMode;

	if ( newMode == MODE_PAUSE )
	{
		currentMode = MODE_PAUSE;
		return;
	}

	if ( newMode == MODE_GAME )
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "");
	}

	if ( newMode == MODE_CONSOLE )
	{
		SDL_StartTextInput ();
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "");
	}

	currentMode = newMode;
}