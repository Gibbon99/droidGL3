#include <unordered_map>
#include <string>
#include <hdr/io/io_textures.h>
#include <hdr/system/sys_audio.h>
#include <hdr/io/io_keyboard.h>
#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_embedFont.h"
#include "hdr/system/sys_main.h"
#include "hdr/system/sys_startup.h"
#include "hdr/system/sys_timing.h"

bool    quitProgram;
int     loops;
float   interpolation;
int     currentMode;

vec2 quadPosition;
vec2 currentVelocity;

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

		case MODE_GAME:
			gl_draw2DQuad (vec2(500, 500), glm::vec2 (32, 32), "quad2d", io_getTextureID ("white_square.jpg"), interpolation);

			gl_draw2DQuad (quadPosition, glm::vec2(200,200), "quad2d", io_getTextureID ("splash.png"), interpolation );

			gl_drawLine (vec2(50, 50), vec2(100, 200), "colorLine", vec4(1, 1, 1, 1));

			break;

		default:
			break;
	}

	fnt_printText (vec2{0,winHeight - 16}, vec4{1,1,1,1}, "FPS [ %i ] Think [ %i ] Inter [ %3.4f ]", fpsPrint, thinkFpsPrint, interpolation);
	fnt_printText (vec2{0, winHeight - 32}, vec4{1, 1, 1, 1}, "Velocity [ %3.3f %3.3f ] Pos [ %3.3f %3.3f ]", currentVelocity.x, currentVelocity.y, quadPosition.x, quadPosition.y);
	if ( g_memLeakLastRun)
		fnt_printText (vec2{0, winHeight - 64}, vec4{1, 1, 1, 1}, "MEM LEAK");

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

		case MODE_GAME:
			io_processKeyboard ();
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

			printf ("loops [ %i ] interpolation [ %3.4f ] Pos x [ %3.3f ]\n", loops, interpolation, quadPosition.x);
		}

		interpolation = float( SDL_GetTicks() + SKIP_TICKS - next_game_tick ) / float( SKIP_TICKS );

		sys_displayScreen ( interpolation );

		fps++;
	}

	sys_shutdownToSystem();

return 0;
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