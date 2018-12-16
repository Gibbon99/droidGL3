#include <unordered_map>
#include <string>
#include <hdr/gui/gui_button.h>

//#include "hdr/system/sys_leakDetector.h"

#include "hdr/gui/gui_render.h"
#include "hdr/game/gam_render.h"
#include "hdr/game/gam_renderDebug.h"
#include "hdr/game/gam_lightCaster.h"
#include "hdr/console/con_conScript.h"
#include "hdr/game/gam_healing.h"
#include "hdr/game/gam_game.h"
#include "hdr/game/gam_droidAIPatrol.h"
#include "hdr/network/net_client.h"
#include "hdr/game/gam_doors.h"
#include "hdr/opengl/gl_fbo.h"
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
#include "hdr/opengl/gl_renderSprite.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_levels.h"
#include "hdr/game/gam_physics.h"
#include "hdr/game/gam_player.h"
#include "hdr/game/gam_droids.h"

#include "hdr/network/net_client.h"
#include "hdr/network/net_server.h"

bool    quitProgram;
int     loops;
float   interpolation;
int     currentMode;

Uint32 currentServerTick;
Uint32 currentClientTick;
Uint32 networkServerTick;

Uint32 frameStart;
Uint32 frameTime;
Uint32 frameCount;

vec3 quadPosition{640.0f,380.0f,-560.0f};

vec3 testLightPosition;

// TODO: AntiAliasing shader for final view render of texture

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
		case MODE_SPLASH:
			gl_set2DMode (glm::vec2{0,0}, glm::vec2{winWidth, winHeight}, glm::vec3{1,1,1,});
			gl_renderSprite("splash", glm::vec2{0, 0}, 1, glm::vec3{0,0,0});
			break;

		case MODE_CONSOLE:
		case MODE_LOADING:
		case MODE_INIT:
			con_showConsole ();
			break;

		case MODE_GUI:
			gui_displayGUI ();
			break;

		case MODE_GAME:
//			gam_processMovement (interpolation);

			gam_drawFullLevel(lvl_getCurrentLevelName (), "quad3d", tileTextureID, interpolation);

			break;

		default:
			break;
	}

	glViewport (0, 0, winWidth, winHeight);

	fnt_printText (vec2{0,winHeight - 16}, vec4{1,1,1,1}, "FPS [ %i ] Think [ %i ] Inter [ %3.4f ] frameTime [ %3.4f ] Mouse [ %f %f ]", fpsPrint, thinkFpsPrint, interpolation,frameTime / 1000.0f, mousePosition.x, mousePosition.y);

	fnt_printText (vec2{0, winHeight - 32}, vec4{1, 1, 1, 1}, "OutQueue [ %ul ] PacketCount Client [ %i - %s ] Server [ Out %i In %i - %s ]", networkOutQueueSize, networkPacketCountSentClient, clientRunning ? "true" : "false", networkPacketCountSentServer, networkPacketCountReceiveServer,  serverRunning ? "true" : "false");

	fnt_printText (vec2{0, winHeight - 48}, vec4{1, 1, 1, 1}, "networkServerTick [ %i ] currentServerTick [ %i ] Diff [ %i ] Framecount [ %i ]", networkServerTick, currentServerTick, currentServerTick - networkServerTick, frameCount);

	//fnt_printText (vec2{0, winHeight - 48}, vec4{1, 1, 1, 1}, "playerLocation [ %3.3f %3.3f ] velocity [ %3.3f %3.3f ]", playerDroid.worldPos.x, playerDroid.worldPos.y, playerDroid.velocity.x, playerDroid.velocity.y);

//	if ( g_memLeakLastRun)
//		fnt_printText (vec2{0, winHeight - 64}, vec4{1, 1, 1, 1}, "MEM LEAK");

	lib_swapBuffers ();
	frameCount++;

//	printf("PacketCount Client[ %i ] Server[ %i ]\n", networkPacketCountSentClient, networkPacketCountSentServer);
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

		case MODE_LOADING:
			if ((io_allTexturesLoaded()) && allLevelsLoaded)
			{
				sys_changeMode(MODE_INIT);
			}
			break;

		case MODE_INIT:
			sys_changeMode(MODE_CONSOLE);
			break;

		case MODE_INIT_GAME:
			gam_startNewGame ();
			break;

		case MODE_CONSOLE:
			break;

		case MODE_GUI:
			break;

		case MODE_GAME:

            io_processInputActions ();

			drd_animateThisLevel (lvl_getCurrentLevelName ());

			gam_getTileUnderPlayer (lvl_getCurrentLevelName (), playerDroid.worldPos.x / TILE_SIZE, playerDroid.worldPos.y / TILE_SIZE);

			gam_processPlayerMovement ();

			ai_processDroidMovement (lvl_getCurrentLevelName () );

            gam_doorCheckTriggerAreas(lvl_getCurrentLevelName ());

			cpSpaceStep (space, SKIP_TICKS);

			net_processNetworkOutQueue (nullptr);

			sys_changeMode (MODE_GUI);
			break;

		default:
			break;
	}
	evt_handleEvents ();

	if (isServer)
		currentServerTick++;        // Don't update locally if this is the server - get it from the network

	currentClientTick++;
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
	currentServerTick = 0;
	frameCount = 0;

	sys_initAll();

	sys_testPrintValue();

	testLightPosition.x = 843;
	testLightPosition.y = 420;

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
		if (interpolation > 1.0f)
			interpolation = 1.0f;

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
void sys_changeMode ( int newMode )
//-----------------------------------------------------------------------------
{
	static int previousMode = -1;

	SDL_ShowCursor (SDL_ENABLE);

	if ( -1 == newMode )
	{
		currentMode = previousMode;
		return;
	}

	previousMode = currentMode;

	if ( newMode == MODE_GUI)
	{
		gui_timerFocusAnimation(true);
		io_mouseTimerState(true);
	}

	if ( newMode == MODE_PAUSE )
	{
		currentMode = MODE_PAUSE;
		return;
	}

	if ( newMode == MODE_GAME )
	{
		gui_timerFocusAnimation(false);
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GAME_TIMER_OFF");
		SDL_ShowCursor(SDL_DISABLE);
//		SDL_WarpMouseInWindow (NULL, 200, 200);
	}

	if ( newMode == MODE_CONSOLE )
	{
		SDL_StartTextInput ();
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_GAME_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GAME_TIMER_ON");
		gam_setPlayerAnimateState ( false);

	}

	currentMode = newMode;
}
