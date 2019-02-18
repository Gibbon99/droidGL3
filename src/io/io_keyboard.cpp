#include "hdr/game/gam_physicsPlayer.h"
#include "hdr/game/gam_database.h"
#include "hdr/game/gam_droidAIShoot.h"
#include "hdr/game/gam_game.h"
#include "hdr/game/gam_render.h"
#include "hdr/game/gam_lightCaster.h"
#include "hdr/game/gam_player.h"
#include "hdr/network/net_client.h"
#include "hdr/game/gam_events.h"
#include "hdr/game/gam_physics.h"
#include "hdr/system/sys_audio.h"
#include "hdr/gui/gui_sideview.h"
#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"

#include "hdr/gui/gui_main.h"

bool eventMoveForward = false;
bool eventMoveBackward = false;
bool eventMoveLeft = false;
bool eventMoveRight = false;
bool eventMoveUp = false;
bool eventMoveDown = false;

//-----------------------------------------------------------------------------
//
// Process keyboard presses
void io_processInputActions ()
//-----------------------------------------------------------------------------
{
	if ( !eventMoveForward )
	{
		g_scaleViewBy += 0.1f;
	}

	if ( !eventMoveBackward )
	{
		g_scaleViewBy -= 0.1f;
	}
}

//-----------------------------------------------------------------------------
//
// Read a unicode character
void io_readChar ( const char *character )
//-----------------------------------------------------------------------------
{
	char compareChar;

	compareChar = *character;

	if ( conCurrentCharCount < MAX_STRING_SIZE - 1 )
	{
		if ( compareChar >= char ( ' ' ))
		{
			conCurrentLine.conLine += compareChar;
			conCurrentCharCount++;
		}
	}
}

//-----------------------------------------------------------------------------
//
// Only read the key to unpause the game
void io_readPauseModeKey ( SDL_Keycode key, int action )
//-----------------------------------------------------------------------------
{
	if ((action == SDL_KEYDOWN) && (key == SDLK_p))
	{
		evt_sendEvent ( USER_EVENT_MODE_PAUSE, 0, 0, 0, 0, glm::vec2 (), glm::vec2 (), "" );
	}
}


//-----------------------------------------------------------------------------
//
// Handle the player action key
void io_processActionKey ( int keyAction )
//-----------------------------------------------------------------------------
{
	if ( keyAction == MY_INPUT_ACTION_RELEASE )
	{
		//
		// Use a lift
		if ((playerDroid.overTile == LIFT_TILE) && (playerDroid.velocity.x == 0.0f) && (playerDroid.velocity.y == 0.0f))
		{
//			sys_destroyPhysicObjects ( currentLevel );
			currentDeckNumber = lvl_getDeckNumber (lvl_getCurrentLevelName ());
			if (-1 == currentDeckNumber)
			{
				con_print(CON_ERROR, true, "Could not locate deck number to move lift.");
				return;
			}

			gam_getTunnelToUse ();
			sys_changeMode ( MODE_LIFT_VIEW );
//			sys_stopAllSounds ();
//			sys_pauseSoundSystem ( true );
//			gam_setHUDState ( HUD_STATE_LIFT );
			return;
		}

		//
		// Use a terminal
		if ((playerDroid.overTile == TERMINAL_TOP) || (playerDroid.overTile == TERMINAL_BOTTOM) ||
		    (playerDroid.overTile == TERMINAL_LEFT) || (playerDroid.overTile == TERMINAL_RIGHT))
			if ((playerDroid.velocity.x == 0.0f) && (playerDroid.velocity.y == 0.0f))
			{
				currentGUIScreen = gui_findIndex ( GUI_OBJECT_SCREEN, "scrTerminal" );
				sys_changeMode ( MODE_GUI );
//				sys_stopAllSounds ();
//				sys_pauseSoundSystem ( true );
				return;
			}


        //
        // Go into transfer mode
        if ((playerDroid.overTile != TERMINAL_TOP) && (playerDroid.overTile != TERMINAL_BOTTOM) &&
            (playerDroid.overTile != TERMINAL_LEFT) && (playerDroid.overTile != TERMINAL_RIGHT) &&
            (playerDroid.overTile != LIFT_TILE) )
		{
			playerDroid.currentMode = DROID_MODE_TRANSFER;
			return;
		}

		//
		// Shoot a bullet - no shooting in transfer mode
		if ( DROID_MODE_TRANSFER != playerDroid.currentMode )
		{
/*
			if ((playerVelocity.x != 0.0f) || (playerVelocity.y != 0.0f))
				if ((inputAction[gameLeft].currentlyDown == true) || (inputAction[gameRight].currentlyDown == true) ||
				    (inputAction[gameUp].currentlyDown == true) || (inputAction[gameDown].currentlyDown == true))
				    */
				{
//					if ( true == playerWeaponReadyToFire )
					{
						cpVect destPosition = cpvadd ( playerDroid.middlePosition, cpvmult ( playerDroid.velocity, 1000 ));
						evt_sendEvent ( MAIN_LOOP_EVENT, MAIN_LOOP_EVENT_ADD_BULLET, dataBaseEntry[0].bulletType, -1, false, glm::vec2{playerDroid.middlePosition.x, playerDroid.middlePosition.y}, glm::vec2{destPosition.x, destPosition.y}, lvl_getCurrentLevelName () );


//						bul_newBullet ( playerDroid.middlePosition, cpvadd ( playerDroid.middlePosition, cpvmult ( playerDroid.velocity, 1000 )), dataBaseEntry[0].bulletType, -1, lvl_getCurrentLevelName () );

//						playerWeaponReadyToFire = false;
                        gam_processWitnessShooting (lvl_getCurrentLevelName ());
					}
				}
		}

	}
	else
	{
//		playerDroid.currentMode = DROID_MODE_NORMAL;
	}
}

//-----------------------------------------------------------------------------
//
// Read and process keys for main game
void io_processGameInputEvents ( _myEventData eventData )
//-----------------------------------------------------------------------------
{
	switch ( currentMode )
	{

		case MODE_GAME:
			if ( MY_INPUT_ACTION_PRESS == eventData.data1 )
			{
				switch ( eventData.data2 )
				{
					case MY_INPUT_CONSOLE:
						sys_changeMode ( MODE_CONSOLE );
						conCurrentCharCount = 0;
						break;

					case MY_INPUT_LEFT:
						eventMoveLeft = true;
						break;

					case MY_INPUT_RIGHT:
						eventMoveRight = true;
						break;

					case MY_INPUT_UP:
						eventMoveUp = true;
						break;

					case MY_INPUT_DOWN:
						eventMoveDown = true;
						break;

					case MY_INPUT_FORWARD:
						eventMoveForward = true;
						break;

					case MY_INPUT_BACKWARD:
						eventMoveBackward = true;
						break;

					case MY_INPUT_ACTION:
						io_processActionKey ( MY_INPUT_ACTION_PRESS );
						break;

					case SDLK_F12:
//				io_saveScreenToFile ();
						break;

					case MY_INPUT_PAUSE:
						con_print ( CON_INFO, true, "Pressed the P key - action is DOWN" );
						evt_sendEvent ( USER_EVENT_MODE_PAUSE, 0, 0, 0, 0, glm::vec2 (), glm::vec2 (), "" );
						break;

					default:
						break;
				}
			}

			if ( MY_INPUT_ACTION_RELEASE == eventData.data1 )
			{
				switch ( eventData.data2 )
				{

					case MY_INPUT_LEFT:
						eventMoveLeft = false;
						break;

					case MY_INPUT_RIGHT:
						eventMoveRight = false;
						break;

					case MY_INPUT_UP:
						eventMoveUp = false;
						break;

					case MY_INPUT_DOWN:
						eventMoveDown = false;
						break;

					case MY_INPUT_FORWARD:
						eventMoveForward = false;
						break;

					case MY_INPUT_BACKWARD:
						eventMoveBackward = false;
						break;

					case MY_INPUT_ACTION:
						io_processActionKey ( MY_INPUT_ACTION_RELEASE );
						break;

					default:
						break;
				}
			}
			break;
	}
}

//-----------------------------------------------------------------------------
//
// Read and process the console keyboard
void io_readConsoleSpecialKeys ( SDL_Keycode key, int action )
//-----------------------------------------------------------------------------
{
	//
	// Check all the other keys, including the backspace key being pressed down
	if ( SDL_KEYDOWN == action )
	{
		switch ( key )
		{
			case SDLK_BACKQUOTE:
				sys_changeMode ( -1 );        // Back to previous mode
				conCurrentCharCount = 0;
				SDL_StopTextInput ();
				break;

			case SDLK_BACKSPACE:
				con_processBackspaceKey ( 0.0 );
				break;

			case SDLK_RETURN:
				con_processCommand ( conCurrentLine.conLine );
				break;

			case SDLK_TAB:
				con_completeCommand ( conCurrentLine.conLine );
				break;

			case SDLK_RIGHT:
				break;

			case SDLK_LEFT:
				break;

			case SDLK_UP:
				con_popHistoryCommand ();
				//
				// Get the next command if we need it
				if ( conHistoryPtr != NUM_MAX_CON_COMMANDS )
					conHistoryPtr++;
				break;

			case SDLK_DOWN:
				//
				// Get the next command if we need it
				if ( conHistoryPtr > 0 )
					conHistoryPtr--;

				con_popHistoryCommand ();
				break;

			default:
				break;
		}
	}

	if ( SDL_KEYUP == action )
	{
		switch ( key )
		{
			case SDLK_BACKSPACE:
//				conBackSpaceDown = 0;
				break;

			default:
				break;
		}
	}
}

//--------------------------------------------------------------------------
//
// Translate a keyboard action into our Game Event value
int io_returnStandardInputActionKeyboard ( Uint32 eventType )
//--------------------------------------------------------------------------
{
	switch ( eventType )
	{
		case SDL_KEYDOWN:
			return MY_INPUT_ACTION_PRESS;
			break;

		case SDL_KEYUP:
			return MY_INPUT_ACTION_RELEASE;
			break;

		default:
			break;
	}

	return -1;
}

//--------------------------------------------------------------------------
//
// Translate keycode values into our Game Event value
int io_returnStandardInputValueKeyboard ( SDL_Keycode keyValue )
//--------------------------------------------------------------------------
{
	//
	// TODO Map different values according to the current MODE ??
	switch ( keyValue )
	{
		case SDLK_BACKQUOTE:
			return MY_INPUT_CONSOLE;
			break;

		case SDLK_LEFT:
			return MY_INPUT_LEFT;
			break;

		case SDLK_RIGHT:
			return MY_INPUT_RIGHT;
			break;

		case SDLK_UP:
			return MY_INPUT_UP;
			break;

		case SDLK_DOWN:
			return MY_INPUT_DOWN;
			break;

		case SDLK_ESCAPE:
			return MY_INPUT_ESCAPE;
			break;

	  case SDLK_LCTRL:
	        return MY_INPUT_ACTION;
	        break;

		case SDLK_RETURN:
			return MY_INPUT_ACTION;
			break;

		case SDLK_F12:
//				io_saveScreenToFile ();
			break;

		case SDLK_PAUSE:
			return MY_INPUT_PAUSE;
			break;

		default:
			return keyValue;
			break;
	}

	return -1;
}

//-----------------------------------------------------------------------------
//
// Handle a keyboard event
void io_handleKeyboardEvent ( SDL_Event event )
//-----------------------------------------------------------------------------
{
	if ( event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN )
		sys_changeMode ( MODE_SHUTDOWN );

	if ( event.key.keysym.sym == SDLK_c && event.type == SDL_KEYDOWN )
	{
		sys_disablePlayerCollision();
		return;
	}

      if (event.key.keysym.sym == SDLK_w && event.type == SDL_KEYDOWN )
        {
          printf ("zDistance [ %3.3f ]\n", zDistance);
          zDistance += 0.1f;
          return;
        }

  if (event.key.keysym.sym == SDLK_s && event.type == SDL_KEYDOWN )
    {
      zDistance -= 0.1f;
      return;
    }


  switch ( currentMode )
	{
		case MODE_CONSOLE:
			if ( event.type == SDL_TEXTINPUT )
				io_readChar ( event.text.text );
			else
				io_readConsoleSpecialKeys ( event.key.keysym.sym, event.type );
			break;

		case MODE_GAME:
			//
			// Put the key event onto the Game Queue as in INPUT event
			//
			// Also send to the server
			evt_sendEvent ( USER_EVENT_GAME, USER_EVENT_KEY_EVENT, io_returnStandardInputActionKeyboard ( event.type ), io_returnStandardInputValueKeyboard ( event.key.keysym.sym ), 0, glm::vec2{
					playerDroid.worldPos.x, playerDroid.worldPos.y}, glm::vec2{playerDroid.velocity.x,
			                                                                   playerDroid.velocity.y}, "" );

//			evt_sendEvent (USER_EVENT_NETWORK_OUT, USER_EVENT_NETWORK_OUT, NET_CURRENT_TICK, (int) frameCount, 0, glm::vec2 (), glm::vec2 (), "");

//            if (event.type == SDL_KEYDOWN)
//                net_sendCurrentLevel(lvl_getCurrentLevelName ());

			break;

		case MODE_GUI:
		case MODE_INTRO:
		case MODE_LIFT_VIEW:
		case MODE_SIDE_VIEW:
		case MODE_DECK_VIEW:
		case MODE_DATABASE:
			evt_sendEvent ( USER_EVENT_GUI, USER_EVENT_KEY_EVENT, io_returnStandardInputActionKeyboard ( event.type ), io_returnStandardInputValueKeyboard ( event.key.keysym.sym ), false, glm::vec2{}, glm::vec2{}, "" );
			break;

		case MODE_PAUSE:
			io_readPauseModeKey ( event.key.keysym.sym, event.type );
			break;

		default:
			break;
	}
}
