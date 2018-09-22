#include <unordered_map>
#include <string>
#include "hdr/io/io_textures.h"
#include "hdr/system/sys_audio.h"
#include "hdr/io/io_keyboard.h"
#include "hdr/io/io_mouse.h"
#include "hdr/game/s_shadows.h"
#include "hdr/libGL/gl_window.h"
#include "hdr/system/sys_embedFont.h"
#include "hdr/system/sys_main.h"
#include "hdr/system/sys_startup.h"
#include "hdr/system/sys_timing.h"
#include "hdr/opengl/gl_opengl.h"

bool    quitProgram;
int     loops;
float   interpolation;
int     currentMode;

Uint32 frameStart;
Uint32 frameTime;

vec3 quadPosition;
vec3 currentVelocity;

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

			gam_drawAllObjects (interpolation);

//			gl_drawAllQuads ("quad2d", "splash.png", interpolation);
			/*
			gl_draw2DQuad (vec2(500, 500), glm::vec2 (32, 32), "quad2d", io_getTextureID ("white_square.jpg"), interpolation);

			gl_draw2DQuad (quadPosition, glm::vec2(600,600), "colorDisc", io_getTextureID ("splash.png"), interpolation );

			gl_drawLine (vec2(50, 50), vec2(100, 200), "colorLine", vec4(1, 1, 1, 1));

			gam_drawAllObjects (interpolation );
			 */
			io_renderMouseCursor ();
			break;

		default:
			break;
	}

	fnt_printText (vec2{0,winHeight - 16}, vec4{1,1,1,1}, "FPS [ %i ] Think [ %i ] Inter [ %3.4f ] frameTime [ %3.4f ] Mouse [ %f %f ]", fpsPrint, thinkFpsPrint, interpolation,
			frameTime / 1000.0f, mousePosition.x, mousePosition.y);
	fnt_printText (vec2{0, winHeight - 32}, vec4{1, 1, 1, 1}, "Velocity [ %3.3f %3.3f %3.3f ] Pos [ %3.3f %3.3f %3.3f ]", currentVelocity.x, currentVelocity.y, currentVelocity.z,
			quadPosition.x, quadPosition.y,quadPosition.z);

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
		frameStart = SDL_GetTicks ();

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
		frameTime = SDL_GetTicks () - frameStart;
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
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GAME_TIMER_OFF");
		SDL_ShowCursor(SDL_DISABLE);
		SDL_WarpMouseInWindow (NULL, 200, 200);
	}

	if ( newMode == MODE_CONSOLE )
	{
		SDL_StartTextInput ();
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GAME_TIMER_ON");
		SDL_ShowCursor (SDL_ENABLE);
	}

	currentMode = newMode;
}