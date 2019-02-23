#include <unordered_map>
#include <string>
#include <hdr/gui/gui_terminal.h>
#include <hdr/game/gam_transfer.h>
#include "hdr/game/gam_events.h"
#include "hdr/gui/gui_button.h"
#include "hdr/gui/gui_scrollBox.h"

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

bool quitProgram;
int loops;
float interpolation;
int currentMode;
cpVect viewableScreenCoord;

Uint32 currentServerTick;
Uint32 currentClientTick;
Uint32 networkServerTick;

Uint32 frameStart;
Uint32 frameTime;
Uint32 frameCount;

vec3 quadPosition{640.0f, 380.0f, -560.0f};

vec2 testLightPosition{};

// TODO: AntiAliasing shader for final view render of texture

//-----------------------------------------------------------------------------------------------------
//
/// \param Pass in interpolation for the frame
/// \return None
void sys_displayScreen (float interpolation)
//-----------------------------------------------------------------------------------------------------
{
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glViewport (0, 0, winWidth, winHeight);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  switch (currentMode)
    {
      case MODE_TEST_ROTATE:
        bul_testRotate();
        break;

      case MODE_SPLASH: gl_set2DMode (glm::vec2{0, 0}, glm::vec2{winWidth, winHeight}, glm::vec3{1, 1, 1,});
      gl_renderSprite ("splash", 0, glm::vec2{0, 0}, 0, 1, glm::vec3{0, 0, 0});
      break;

      case MODE_LOADING:
      case MODE_CONSOLE: con_showConsole ();
      break;

      case MODE_INIT: break;

      case MODE_GUI:
        gui_displayGUI ();
      break;

      case MODE_INTRO:
        gui_displayGUI ();
      break;

      case MODE_LIFT_VIEW:
      case MODE_SIDE_VIEW:
      case MODE_DECK_VIEW:
      case MODE_DATABASE:
        gui_displayGUI ();
      break;

      case MODE_TRANSFER_INTRO_0:         // Show commencing message
      case MODE_TRANSFER_INTRO_1:       // Show current droid information
      case MODE_TRANSFER_INTRO_2:       // Show target droid information
      case MODE_TRANSFER_START:         //
      case MODE_TRANSFER_SELECT_SIDE:   // Countdown to choose side
      case MODE_TRANSFER_SELECT:        // Play the transfer game
      case MODE_TRANSFER_LOST:          // Lost the transfer
      case MODE_TRANSFER_COPY:          // Won the transfer
      case MODE_TRANSFER_DEADLOCK:      // Deadlock - start again
      case MODE_TRANSFER_FINISH:        // Clean up
        gui_displayGUI ();
      break;

      case MODE_GAME:
//			gam_processMovement (interpolation);

        gam_renderFullLevel (lvl_getCurrentLevelName (), "quad3d", tileTextureID, interpolation);

      break;

      default: break;
    }

  glViewport (0, 0, winWidth, winHeight);

  fnt_printText (vec2{0, winHeight - 16}, vec4{1, 1, 1,
                                               1}, "FPS [ %i ] Think [ %i ] Inter [ %3.4f ] frameTime [ %3.4f ] Mouse [ %f %f ]", fpsPrint, thinkFpsPrint, interpolation,
                 frameTime / 1000.0f, mousePosition.x, mousePosition.y);

  fnt_printText (vec2{0, winHeight - 32}, vec4{1, 1, 1,
                                               1}, "OutQueue [ %i ] PacketCount Client [ %i - %s ] Server [ Out %i In %i - %s ]", networkOutQueueSize, networkPacketCountSentClient, clientRunning
                                                                                                                                                                                     ? "true"
                                                                                                                                                                                     : "false", networkPacketCountSentServer, networkPacketCountReceiveServer, serverRunning
                                                                                                                                                                                                                                                               ? "true"
                                                                                                                                                                                                                                                               : "false");

  fnt_printText (vec2{0, winHeight - 48}, vec4{1, 1, 1,
                                               1}, "networkServerTick [ %i ] currentServerTick [ %i ] Diff [ %i ] Framecount [ %i ]", networkServerTick, currentServerTick,
                 currentServerTick - networkServerTick, frameCount);

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
void sys_gameTickRun ()
//-----------------------------------------------------------------------------------------------------
{
  switch (currentMode)
    {
      case MODE_TEST_ROTATE:
        break;

      case MODE_SHUTDOWN: quitProgram = true;
      break;

      case MODE_LOADING:
        if ((io_allTexturesLoaded ()) && allLevelsLoaded)
          sys_changeMode (MODE_INIT);
      break;

      case MODE_INIT:
        sys_changeMode (MODE_GUI);         // change here
      break;

      case MODE_CONSOLE:
        break;

      case MODE_GUI:
        break;

      case MODE_INIT_GAME:
        //gam_startNewGame ();
        break;

      case MODE_GAME:

        io_processInputActions ();

        gam_processPlayerMovement (interpolation);

        gam_processMainLoopEventQueue ();

        net_processWorldStep ();

        bul_moveBullet ();

        cpSpaceStep (space, SKIP_TICKS);

        net_processNetworkOutQueue (nullptr);
      break;

      case MODE_TRANSFER_INTRO_0:         // Show commencing message
      case MODE_TRANSFER_INTRO_1:       // Show current droid information
      case MODE_TRANSFER_INTRO_2:       // Show target droid information
      case MODE_TRANSFER_START:         //
      case MODE_TRANSFER_SELECT_SIDE:   // Countdown to choose side
      case MODE_TRANSFER_SELECT:        // Play the transfer game
      case MODE_TRANSFER_LOST:          // Lost the transfer
      case MODE_TRANSFER_COPY:          // Won the transfer
      case MODE_TRANSFER_DEADLOCK:      // Deadlock - start again
      case MODE_TRANSFER_FINISH:        // Clean up
        gam_processTransfer();
      break;

      default: break;
    }

  evt_handleEvents ();

  io_cleanTextureMap ();
  gl_cleanSpriteMap ();
  gui_checkDataBaseTextureReload ();

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
int main (int argc, char *argv[])
//-----------------------------------------------------------------------------------------------------
{
  Uint32 next_game_tick = SDL_GetTicks ();
  currentServerTick = 0;
  frameCount = 0;

  sys_initAll ();

//	sys_testPrintValue();

  testLightPosition.x = 843;
  testLightPosition.y = 420;

  while (!quitProgram)
    {
      loops = 0;
      frameStart = SDL_GetTicks ();

      evt_handleEvents ();

      while (SDL_GetTicks () > next_game_tick && loops < MAX_FRAMESKIP)
        {
          sys_gameTickRun ();
          next_game_tick += SKIP_TICKS;
          loops++;
          thinkFPS++;
        }

      interpolation = float (SDL_GetTicks () + SKIP_TICKS - next_game_tick) / float (SKIP_TICKS);
      if (interpolation > 1.0f)
        interpolation = 1.0f;

      sys_displayScreen (interpolation);

      fps++;
      frameTime = SDL_GetTicks () - frameStart;
    }

  sys_shutdownToSystem ();

  return 0;
}

//-----------------------------------------------------------------------------
//
// Change game mode
void sys_changeMode (int newMode)
//-----------------------------------------------------------------------------
{
#define DEBUG_CHANGE_MODE 1

  static int previousMode = -1;

  if ((-1 == newMode) && (-1 == previousMode))
    {
      printf ("ERROR: Need to call changeMode with a valid value.\n");
      sys_shutdownToSystem ();
    }

  if (-1 == newMode)
    {
      currentMode = previousMode;
      sys_changeMode (currentMode);    // Call again to restart the timers
      return;
    }

  previousMode = currentMode;

  currentMode = newMode;

  switch (newMode)
    {
      case MODE_GUI:
        evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_TIMER_OFF");
        evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_ANIMATE_TIMER, USER_EVENT_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_ANIMATE_TIMER");
        evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_MOUSE_TIMER, USER_EVENT_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_MOUSE_TIMER");
        SDL_ShowCursor (SDL_ENABLE);
#ifdef DEBUG_CHANGE_MODE
      printf ("New mode [ %s ]\n", "MODE_GUI");
#endif
      break;

      case MODE_PAUSE:
        evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_TIMER_OFF");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_ANIMATE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_ANIMATE_TIMER");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_MOUSE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_MOUSE_TIMER");
#ifdef DEBUG_CHANGE_MODE
      printf ("New mode [ %s ]\n", "MODE_PAUSE");
#endif
      break;

      case MODE_GAME:
        gui_timerFocusAnimation (false);
      evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_TIMER_OFF");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_MOUSE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_MOUSE_TIMER");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_ANIMATE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_ANIMATE_TIMER");
      SDL_ShowCursor (SDL_DISABLE);
      gam_setPlayerAnimateState (true);
      //		SDL_WarpMouseInWindow (NULL, 200, 200);
#ifdef DEBUG_CHANGE_MODE
      printf ("New mode [ %s ]\n", "MODE_GAME");
#endif
      break;

      case MODE_CONSOLE:
        SDL_StartTextInput ();
      evt_sendEvent (USER_EVENT_GAME, USER_EVENT_GAME_TIMER, USER_EVENT_GAME_TIMER_CONSOLE, USER_EVENT_TIMER_ON, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_TIMER_ON");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_MOUSE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_MOUSE_TIMER");
      evt_sendEvent (USER_EVENT_GUI, USER_EVENT_GAME_TIMER, USER_EVENT_GUI_ANIMATE_TIMER, USER_EVENT_TIMER_OFF, 0, glm::vec2 (), glm::vec2 (), "USER_EVENT_GUI_ANIMATE_TIMER");
      gam_setPlayerAnimateState (false);
#ifdef DEBUG_CHANGE_MODE
      printf ("New mode [ %s ]\n", "MODE_CONSOLE");
#endif
      break;

      case MODE_INTRO:
        gui_timerScrollSpeedIntro (USER_EVENT_TIMER_ON, introScrollBox.scrollSpeed);
      break;

      default: break;
    }
}
