#include "hdr/system/sys_events.h"

#include "hdr/io/io_logfile.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/s_gameEvents.h"
#include "hdr/io/io_keyboard.h"

SDL_TimerID timerCursorFlash;

SDL_Thread *userEventConsoleThread;
SDL_Thread *userEventAudioThread;
SDL_Thread *userEventLoggingThread;
SDL_Thread *userEventGameThread;

SDL_mutex *consoleMutex;
SDL_mutex *audioMutex;
SDL_mutex *loggingMutex;
SDL_mutex *gameMutex;

queue <_myEventData> consoleEventQueue;
queue <_myEventData> audioEventQueue;
queue <_myEventData> loggingEventQueue;
queue <_myEventData> gameEventQueue;

bool runThreads = true;     // Master flag to control state of detached threads

//------------------------------------------------------------------------
//
// Callback for timer function - cursor flash
Uint32 evt_cursorTimerCallback ( Uint32 interval, void *param )
//------------------------------------------------------------------------
{
	con_processCursor ();

	return interval;
}

//------------------------------------------------------------------------
//
// Change the state of the console timer cursor animation
void evt_cursorChangeState ( int newState )
//------------------------------------------------------------------------
{
	switch ( newState )
	{
		case USER_EVENT_GAME_TIMER_OFF:
		{
			SDL_RemoveTimer (timerCursorFlash);
			timerCursorFlash = 0;
			break;
		}
		case USER_EVENT_GAME_TIMER_ON:
		{
			timerCursorFlash = SDL_AddTimer (500, evt_cursorTimerCallback, nullptr);   // Time in milliseconds
			break;
		}
		default:
			break;
	}
}

//------------------------------------------------------------------------
//
// Setup user event source and event data
bool evt_registerUserEventSetup ()
//------------------------------------------------------------------------
{
	timerCursorFlash = SDL_AddTimer (500, evt_cursorTimerCallback, nullptr);   // Time in milliseconds

	userEventConsoleThread = SDL_CreateThread (con_processConsoleUserEvent, "userEventConsoleThread", (void *) nullptr);
	if ( nullptr == userEventConsoleThread )
	{
		printf ("SDL_CreateThread - userEventConsoleThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	userEventAudioThread = SDL_CreateThread (aud_processAudioEventQueue, "userEventAudioThread", (void *) nullptr);
	if ( nullptr == userEventAudioThread )
	{
		printf ("SDL_CreateThread - userEventConsoleThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	userEventLoggingThread = SDL_CreateThread (io_processLoggingEventQueue, "userEventLoggingThread", (void *) nullptr);
	if ( nullptr == userEventLoggingThread )
	{
		printf ("SDL_CreateThread - userEventLoggingThread - failed: %s\n", SDL_GetError ());
		return false;
	}

	userEventGameThread = SDL_CreateThread (gam_processGameEventQueue, "userEventGameThread", (void *) nullptr);
	if ( nullptr == userEventGameThread )
	{
		printf ("SDL_CreateThread = userEventGameThread = failed: %s\n", SDL_GetError ());
		return false;
	}

	consoleMutex = SDL_CreateMutex ();
	if ( !consoleMutex )
	{
		printf ("Couldn't create mutex - consoleMutex");
		return false;
	}

	audioMutex = SDL_CreateMutex ();
	if ( !audioMutex )
	{
		printf ("Couldn't create mutex - audioMutex");
		return false;
	}

	loggingMutex = SDL_CreateMutex ();
	if ( !loggingMutex )
	{
		printf ("Couldn't create mutex - loggingMutex");
		return false;
	}

	gameMutex = SDL_CreateMutex ();
	if ( !gameMutex )
	{
		printf ("Couldn't create mutex - gameMutex");
		return false;
	}

	SDL_DetachThread (userEventConsoleThread);
	SDL_DetachThread (userEventAudioThread);
	SDL_DetachThread (userEventLoggingThread);
	SDL_DetachThread (userEventGameThread);

	return true;
}

//------------------------------------------------------------------------
//
// Shutdown mutex
void evt_shutdownMutex ()
//------------------------------------------------------------------------
{
	SDL_DestroyMutex (consoleMutex);
	SDL_DestroyMutex (audioMutex);
	SDL_DestroyMutex (loggingMutex);
	SDL_DestroyMutex (gameMutex);
}

//------------------------------------------------------------------------
//
// Create a custom event to be sent
void evt_sendEvent ( uint type, int action, int data1, int data2, int data3, const vec2 vec2_1, const vec2 vec2_2, string textString )
//------------------------------------------------------------------------
{
	_myEventData eventData;

	eventData.eventType = type;
	eventData.eventAction = action;
	eventData.data1 = data1;
	eventData.data2 = data2;
	eventData.data3 = data3;
	eventData.vec2_1 = vec2_1;
	eventData.vec2_2 = vec2_2;
	eventData.eventString = std::move (textString);

	switch ( type )
	{
		case USER_EVENT_CONSOLE:
			if ( SDL_LockMutex (consoleMutex) == 0 )
			{
				consoleEventQueue.push (eventData);
				SDL_UnlockMutex (consoleMutex);
			}
			break;

		case USER_EVENT_AUDIO:
			if ( SDL_LockMutex (audioMutex) == 0 )
			{
				audioEventQueue.push (eventData);
				SDL_UnlockMutex (audioMutex);
			}
			break;

		case USER_EVENT_LOGGING:
			if ( SDL_LockMutex (loggingMutex) == 0 )
			{
				loggingEventQueue.push (eventData);
				SDL_UnlockMutex (loggingMutex);
			}
			break;

		case USER_EVENT_GAME:
			if ( SDL_LockMutex (gameMutex) == 0 )
			{
				gameEventQueue.push (eventData);
				SDL_UnlockMutex (gameMutex);
			}
			break;

		default:
			break;

	}
}

//------------------------------------------------------------------------
//
// Handle DISPLAY events
void evt_handleDisplayEvents ( SDL_Event event )
//------------------------------------------------------------------------
{
	switch ( event.window.event )
	{
		case SDL_WINDOWEVENT_SHOWN:
			con_print (CON_INFO, true, "Window %d shown", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_HIDDEN:
			con_print (CON_INFO, true, "Window %d hidden", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_EXPOSED:
			con_print (CON_INFO, true, "Window %d exposed", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_MOVED:
			con_print (CON_INFO, true, "Window %d moved to %d,%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_RESIZED:
			con_print (CON_INFO, true, "Window %d resized to %dx%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_SIZE_CHANGED:
			con_print (CON_INFO, true, "Window %d size changed to %dx%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			con_print (CON_INFO, true, "Window %d minimized", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			con_print (CON_INFO, true, "Window %d maximized", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_RESTORED:
			con_print (CON_INFO, true, "Window %d restored", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_ENTER:
			con_print (CON_INFO, true, "Mouse entered window %d", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_LEAVE:
			con_print (CON_INFO, true, "Mouse left window %d", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
			con_print (CON_INFO, true, "Window %d gained keyboard focus", event.window.windowID);
//			changeMode(-1);
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			con_print (CON_INFO, true, "Window %d lost keyboard focus", event.window.windowID);
//			changeMode (MODE_PAUSE);
			break;

		case SDL_WINDOWEVENT_CLOSE:
//			changeMode (MODE_SHUTDOWN);
			break;

#if SDL_VERSION_ATLEAST (2, 0, 5)
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			con_print(CON_INFO, true, "Window %d is offered a focus", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_HIT_TEST:
			con_print(CON_INFO, true, "Window %d has a special hit test", event.window.windowID);
			break;
#endif
		default:
			con_print (CON_INFO, true, "Window %d got unknown event %d", event.window.windowID, event.window.event);
			break;
	}
}

//------------------------------------------------------------------------
//
// Handle JOYSTICK events
void evt_handleJoystickEvents ( SDL_Event event )
//------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------
//
// Main event routine - handle all the events coming in and
// farm them to the correct routine
//
// Called from the main thread
void evt_handleEvents ()
//------------------------------------------------------------------------
{
	SDL_Event event;

	while ( SDL_PollEvent (&event))
	{
		switch ( event.type )
		{
			case SDL_MOUSEMOTION:
				evt_sendEvent (USER_EVENT_MOUSE, USER_EVENT_MOUSE_MOTION, event.motion.x, event.motion.y, event.motion.state, glm::vec2 (), glm::vec2 (), "");
				break;

			case SDL_MOUSEBUTTONUP:
				evt_sendEvent (USER_EVENT_MOUSE, USER_EVENT_MOUSE_BUTTON_UP, event.motion.state, 0, 0, glm::vec2 (), glm::vec2 (), "");
				break;

			case SDL_MOUSEWHEEL:
				break;

			case SDL_JOYAXISMOTION:     // Joystick axis motion
			case SDL_JOYBALLMOTION:     // Joystick trackball motion
			case SDL_JOYHATMOTION:      // Joystick hat position change
			case SDL_JOYBUTTONDOWN:     // Joystick button pressed
			case SDL_JOYBUTTONUP:       // Joystick button released
			case SDL_JOYDEVICEADDED:    // Joystick connected
			case SDL_JOYDEVICEREMOVED:  // Joystick disconnected
				evt_handleJoystickEvents (event);
				break;

			case SDL_KEYDOWN:           // key pressed
			case SDL_KEYUP:             // Key released
			case SDL_TEXTEDITING:       // Keyboard text editing (composition)
			case SDL_TEXTINPUT:         // Keyboard text input
			case SDL_KEYMAPCHANGED:     // Keymap changed due to a system event such as an input language or keyboard layout change (>= SDL 2.
				io_handleKeyboardEvent (event);
				break;

			case SDL_FINGERDOWN:        // User has touched input device
			case SDL_FINGERUP:          // User stopped touching input device
			case SDL_FINGERMOTION:      // User is dragging finger on input device
				break;

			case SDL_WINDOWEVENT:
				evt_handleDisplayEvents (event);
				break;

			case SDL_USEREVENT:     // A user-specified event
				break;

			default:
				break;
		}
	}
}