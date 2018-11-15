#include <hdr/game/s_render.h>
#include <hdr/game/s_lightCaster.h>
#include <hdr/game/s_player.h>
#include <hdr/network/net_client.h>
#include <hdr/game/s_gameEvents.h>
#include "hdr/system/sys_main.h"
#include "hdr/system/sys_events.h"

bool keyForwardDown = false;
bool keyBackwardDown = false;
bool keyLeftDown = false;
bool keyRightDown = false;
bool keyUpDown = false;
bool keyDownDown = false;

//-----------------------------------------------------------------------------
//
// Process keyboard presses
void io_processInputActions ()
//-----------------------------------------------------------------------------
{
	float moveSpeed = 1.0f / 20.0f;

	if (keyLeftDown)
		currentVelocity.x -= moveSpeed;

	if (keyRightDown)
		currentVelocity.x += moveSpeed;

	if (keyUpDown)
		currentVelocity.y -= moveSpeed;

	if (keyDownDown)
		currentVelocity.y += moveSpeed;

	if (!keyForwardDown)
	{
		g_scaleViewBy += 0.1f;
	}


	if (!keyBackwardDown)
	{
		g_scaleViewBy -= 0.1f;
	}

	if ( !keyLeftDown )
	{
		if ( currentVelocity.x < 0.0f )
		{
			currentVelocity.x += moveSpeed;
			if ( currentVelocity.x > 0.0f )
				currentVelocity.x = 0.0f;
		}
	}

	if ( !keyRightDown )
	{
		if (currentVelocity.x > 0.0f)
		{
			currentVelocity.x -= moveSpeed;
			if (currentVelocity.x < 0.0f)
				currentVelocity.x = 0.0f;
		}
	}

	if ( !keyDownDown )
	{
		if (currentVelocity.y > 0.0f)
		{
			currentVelocity.y -= moveSpeed;
			if (currentVelocity.y < 0.0f)
				currentVelocity.y = 0.0f;
		}
	}


	if ( !keyUpDown )
	{
		if (currentVelocity.y < 0.0f)
		{
			currentVelocity.y += moveSpeed;
			if ( currentVelocity.y > 0.0f )
				currentVelocity.y = 0.0f;
		}
	}


	// Check bounds
	if (currentVelocity.x < -5.0f)
		currentVelocity.x = -5.0f;

	if (currentVelocity.x > 5.0f)
		currentVelocity.x = 5.0f;

	if (currentVelocity.y < -5.0f)
		currentVelocity.y = -5.0f;

	if (currentVelocity.y > 5.0f)
		currentVelocity.y = 5.0f;

	playerDroid.worldPos.x += currentVelocity.x;
	playerDroid.worldPos.y += currentVelocity.y;
}

//-----------------------------------------------------------------------------
//
// Read a unicode character
void io_readChar ( const char *character )
//-----------------------------------------------------------------------------
{
	char    compareChar;

	compareChar = *character;

	if ( conCurrentCharCount < MAX_STRING_SIZE - 1 )
	{
		if ( compareChar >= char(' ') )
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
		evt_sendEvent (USER_EVENT_MODE_PAUSE, 0, 0, 0, 0, glm::vec2(), glm::vec2(), "");
	}
}

//-----------------------------------------------------------------------------
//
// Read and process keys for main game
void io_processGameInputEvents (_myEventData eventData)
//-----------------------------------------------------------------------------
{
	if ( MY_INPUT_ACTION_PRESS == eventData.data1 )
	{
		switch ( eventData.data2 )
		{
			case MY_INPUT_CONSOLE:
				sys_changeMode (MODE_CONSOLE);
				conCurrentCharCount = 0;
				break;

			case MY_INPUT_LEFT:
				keyLeftDown = true;
				break;

			case MY_INPUT_RIGHT:
				keyRightDown = true;
				break;

			case MY_INPUT_UP:
				keyUpDown = true;
				break;

			case MY_INPUT_DOWN:
				keyDownDown = true;
				break;

			case MY_INPUT_FORWARD:
				keyForwardDown = true;
				break;

			case MY_INPUT_BACKWARD:
				keyBackwardDown = true;
				break;

			case SDLK_F12:
//				io_saveScreenToFile ();
				break;

			case MY_INPUT_PAUSE:
				con_print (CON_INFO, true, "Pressed the P key - action is DOWN");
				evt_sendEvent (USER_EVENT_MODE_PAUSE, 0, 0, 0, 0, glm::vec2(), glm::vec2(), "");
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
				keyLeftDown = false;
				break;

			case MY_INPUT_RIGHT:
				keyRightDown = false;
				break;

			case MY_INPUT_UP:
				keyUpDown = false;
				break;

			case MY_INPUT_DOWN:
				keyDownDown = false;
				break;

			case MY_INPUT_FORWARD:
				keyForwardDown = false;
				break;

			case MY_INPUT_BACKWARD:
				keyBackwardDown = false;
				break;

			default:
				break;
		}
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
				sys_changeMode (MODE_GAME);
				conCurrentCharCount = 0;
				SDL_StopTextInput ();
				break;

			case SDLK_BACKSPACE:
				con_processBackspaceKey (0.0);
				break;

			case SDLK_RETURN:
				con_processCommand (conCurrentLine.conLine);
				break;

			case SDLK_TAB:
				con_completeCommand (conCurrentLine.conLine);
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
int io_returnStandardInputActionKeyboard (Uint32 eventType)
//--------------------------------------------------------------------------
{
  switch (eventType)
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
}

//--------------------------------------------------------------------------
//
// Translate keycode values into our Game Event value
int io_returnStandardInputValueKeyboard (SDL_Keycode keyValue)
//--------------------------------------------------------------------------
{
  switch (keyValue)
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

      case SDLK_w:
        return MY_INPUT_FORWARD;
      break;

      case SDLK_s:
        return MY_INPUT_BACKWARD;
      break;

      case SDLK_F12:
//				io_saveScreenToFile ();
        break;

      case SDLK_p:
        return MY_INPUT_PAUSE;
      break;

    }
}

//-----------------------------------------------------------------------------
//
// Handle a keyboard event
void io_handleKeyboardEvent ( SDL_Event event )
//-----------------------------------------------------------------------------
{
	if ( event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN )
		sys_changeMode (MODE_SHUTDOWN);

	switch ( currentMode )
	{
		case MODE_CONSOLE:
			if ( event.type == SDL_TEXTINPUT )
				io_readChar (event.text.text);
			else
				io_readConsoleSpecialKeys (event.key.keysym.sym, event.type);
			break;

		case MODE_GAME:
			//
			// Put the key event onto the Game Queue as in INPUT event
			//
			// Also send to the server
			evt_sendEvent (USER_EVENT_GAME, USER_EVENT_KEY_EVENT,
                           io_returnStandardInputActionKeyboard (event.type), io_returnStandardInputValueKeyboard (event.key.keysym.sym) , 0, glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y}, glm::vec2{playerDroid.velocity.x, playerDroid.velocity.y},"");
//			evt_sendEvent (USER_EVENT_NETWORK_CLIENT, NETWORK_SEND_DATA, event.type, event.key.keysym.sym, 0, glm::vec2{playerDroid.worldPos.x, playerDroid.worldPos.y}, glm::vec2{playerDroid.velocity.x, playerDroid.velocity.y}, "Keyboard event");

      if (event.type == SDL_KEYDOWN)
        net_sendCurrentLevel(lvl_getCurrentLevelName ());

			break;

		case MODE_PAUSE:
			io_readPauseModeKey (event.key.keysym.sym, event.type);
			break;

		default:
			break;
	}
}