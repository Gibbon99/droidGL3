#include <hdr/game/gam_levels.h>
#include <hdr/gui/gui_main.h>
#include <hdr/game/gam_transfer.h>
#include "hdr/io/io_textures.h"
#include "hdr/system/sys_events.h"
#include "hdr/io/io_logfile.h"
#include "hdr/system/sys_audio.h"
#include "hdr/game/gam_events.h"
#include "hdr/io/io_keyboard.h"
#include "hdr/io/io_mouse.h"
#include "hdr/game/gam_eventsServer.h"
#include "hdr/game/gam_eventsClient.h"

SDL_TimerID testLevelLoad;      // Used to test that all the levels are loaded

SDL_TimerID timerCursorFlash;

SDL_Thread *userEventConsoleThread;
SDL_Thread *userEventAudioThread;
SDL_Thread *userEventLoggingThread;
SDL_Thread *userEventGameThread;
SDL_Thread *userEventGuiThread;
SDL_Thread *userEventServerThread;
SDL_Thread *userEventClientThread;
SDL_Thread *userEventTransferThread;

SDL_mutex *consoleMutex;
SDL_mutex *audioMutex;
SDL_mutex *loggingMutex;
SDL_mutex *gameMutex;
SDL_mutex *guiMutex;
SDL_mutex *levelMutex;
SDL_mutex *textureSetMutex;
SDL_mutex *transferMutex;

SDL_mutex *networkInMutex;

SDL_mutex *serverEventInMutex;
SDL_mutex *clientEventInMutex;

SDL_mutex *mainLoopMutex;

queue <_myEventData> consoleEventQueue;
queue <_myEventData> audioEventQueue;
queue <_myEventData> loggingEventQueue;
queue <_myEventData> gameEventQueue;
queue <_myEventData> guiEventQueue;
queue <_myEventData> mainLoopEventQueue;
queue <_myEventData> transferEventQueue;

queue <_myEventData> clientEventInQueue;
//queue<_myEventData> networkClientOutQueue;

queue <_myEventData> serverEventInQueue;
queue <_myEventData> networkOutQueue;

bool runThreads = true;     // Master flag to control state of detached threads
bool doCursorAnimation = false;     // Flag to animate the cursor in callback

uint32_t        SDL_myEventType;

typedef struct
{
	SDL_TimerID             timerID = 0;
	std::string             timerName;
} _registeredTimers;

vector<_registeredTimers>         registeredTimers;

typedef struct
{
	SDL_Thread      *threadPointer = nullptr;
	std::string     threadName;
} _registeredThreads;

vector<_registeredThreads>          registeredThreads;

typedef struct
{
  _myEventData          timerAndEvent;
  SDL_TimerID           timerID = 0;
  string                eventName;
  Uint32                nextEventTime = 0;
} _timerAndEvent;

vector<_timerAndEvent>                timerAndEvent;

//------------------------------------------------------------------------
//
// Remove a timer event and stop the SDL_Timer
void evt_removeTimerAndEvent(SDL_TimerID whichTimer)
//------------------------------------------------------------------------
{
	for (int timerIndex = 0; timerIndex < timerAndEvent.size(); timerIndex++)
	{
		if (timerAndEvent[timerIndex].timerID == whichTimer)
		{

		    printf ("Removing timer [ %i ]\n", whichTimer);

			SDL_RemoveTimer(whichTimer);
			timerAndEvent.erase(timerAndEvent.begin() + timerIndex);
		}
	}
}

//------------------------------------------------------------------------
//
// Function called by timed Event - run once and then change to next event
Uint32 evt_timerAndEventFunction(Uint32 interval, void *param )
//------------------------------------------------------------------------
{
  _myEventData        nextEventData;
  unsigned long       timerEventIndex;
  Uint32              nextEventTime = 0;

  timerEventIndex = (unsigned long)(param);
  nextEventData = timerAndEvent[timerEventIndex].timerAndEvent;

  nextEventTime = timerAndEvent[timerEventIndex].nextEventTime;

  evt_sendEvent ( nextEventData.eventType, nextEventData.eventAction,
                  nextEventTime, nextEventData.data2, nextEventData.data3,
                  nextEventData.vec2_1, nextEventData.vec2_2, nextEventData.eventString );

  printf ("Change to event [ %s ] number [ %lu ]\n", timerAndEvent[timerEventIndex].eventName.c_str(), timerEventIndex);

  timerAndEvent.erase(timerAndEvent.begin() + timerEventIndex);

  return 0;
}

//------------------------------------------------------------------------
//
// Add a timer with next mode to change too
SDL_TimerID evt_registerTimerAndEvent(Uint32 timerInterval, const _myEventData &nextEventData, const std::string &eventName)
//------------------------------------------------------------------------
{
  unsigned long       nextEventID = 0;
  _timerAndEvent      nextTimerEvent;
  SDL_TimerID         nextTimerEventTimerID = 0;

  nextTimerEvent.timerAndEvent = nextEventData;
  nextTimerEvent.eventName = eventName;
  nextTimerEvent.nextEventTime = timerInterval;

  timerAndEvent.push_back(nextTimerEvent);

  nextEventID = timerAndEvent.size() - 1;
  nextTimerEventTimerID = evt_registerTimer (timerInterval, evt_timerAndEventFunction, eventName, nextEventID);
  timerAndEvent[nextEventID + 1].timerID = nextTimerEventTimerID;

  return nextTimerEventTimerID;
}

//------------------------------------------------------------------------
//
// Keep a list of threads that have been created. They will be detached and remove themselves
// at shutdown
SDL_Thread *evt_registerThread(SDL_ThreadFunction threadFunction, const std::string &threadName)
//------------------------------------------------------------------------
{
	_registeredThreads      newThread;

	newThread.threadPointer = nullptr;

	newThread.threadPointer = SDL_CreateThread(threadFunction, threadName.c_str(), (void *)nullptr);
	if (nullptr == newThread.threadPointer)
	{
		con_print(CON_ERROR, true, "Unable to create new thread [ %s ]", threadName.c_str());
		return nullptr;
	}

	newThread.threadName = threadName;

	registeredThreads.push_back(newThread);

	con_print(CON_INFO, true, "Created new thread [ %s ]", threadName.c_str());

	return newThread.threadPointer;
}

//------------------------------------------------------------------------
//
// Keep a list of the timers and remove them at shutdown
SDL_TimerID evt_registerTimer (Uint32 timerInterval, SDL_TimerCallback timerFunction, const std::string timerName, unsigned long dataIndex)
//------------------------------------------------------------------------
{
	_registeredTimers       newTimer;

	newTimer.timerID = 0;

	newTimer.timerID = SDL_AddTimer ( timerInterval, timerFunction, (void *)dataIndex );   // Time in milliseconds
	if (newTimer.timerID == 0)
	{
		con_print(CON_ERROR, true, "Unable to create new timer for [ %s ] - [ %s ]", timerName.c_str(), SDL_GetError ());
		return 0;
	}

	newTimer.timerName = timerName;

	registeredTimers.push_back(newTimer);

	con_print(CON_INFO, true, "Added new timer [ %s ]", timerName.c_str());

	return newTimer.timerID;
}

//------------------------------------------------------------------------
//
// Remove a single timer - pass in SDL_TimerID as index
void evt_removeTimer(SDL_TimerID whichTimerID)
//------------------------------------------------------------------------
{
	for ( unsigned int i = 0; i < registeredTimers.size(); i++)
	{
		if (registeredTimers[i].timerID == whichTimerID)
		{
			SDL_RemoveTimer (registeredTimers[i].timerID);
			registeredTimers.erase ( registeredTimers.begin () + i);
		}
	}
}

//------------------------------------------------------------------------
//
// Remove all the timers
void evt_removeAllTimers()
//------------------------------------------------------------------------
{
	for ( const auto &timerIndex : registeredTimers)
	{
		SDL_RemoveTimer(timerIndex.timerID);
		con_print(CON_INFO, true, "Removed timer [ %s ]", timerIndex.timerName.c_str());
	}
	registeredTimers.clear();
	registeredTimers.resize(0);
}

//------------------------------------------------------------------------
//
// Thread to keep checking if all the level information has been loaded
Uint32 evt_getLevelInfo ( Uint32 interval, void *param)
//------------------------------------------------------------------------
{
	if (levelInfo.size() == numTotalLevelsToLoad)
	{
		evt_sendEvent (USER_EVENT_GAME, USER_EVENT_LEVEL_LOAD_DONE, 0, 0, 0, vec2 (), vec2 (), "");
		return 0;       // Stop checking now
	}

	return interval;
}

//------------------------------------------------------------------------
//
// Callback for timer function - cursor flash
Uint32 evt_cursorTimerCallback ( Uint32 interval, void *param )
//------------------------------------------------------------------------
{
	if (!doCursorAnimation)
		return interval;

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
		case USER_EVENT_TIMER_OFF:
		{
			doCursorAnimation = false;
			break;
		}

		case USER_EVENT_TIMER_ON:
		{
			doCursorAnimation = true;
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
	SDL_myEventType = SDL_RegisterEvents (1);
	if (SDL_myEventType == (Uint32) - 1)
	{
		printf("SDL_RegisterEvents failed. Not enough memory for user-defined event. Exiting.\n");
		sys_shutdownToSystem ();
	}

	testLevelLoad = evt_registerTimer (500, evt_getLevelInfo, "Check level loading", 0);

	timerCursorFlash = evt_registerTimer (500, evt_cursorTimerCallback, "Console cursor animation", 0);

	userEventConsoleThread = evt_registerThread(con_processConsoleUserEvent, "userEventConsoleThread");
	if ( nullptr == userEventConsoleThread )
		return false;

	userEventAudioThread = evt_registerThread(aud_processAudioEventQueue, "userEventAudioThread");
	if ( nullptr == userEventAudioThread )
		return false;

	userEventLoggingThread = evt_registerThread(io_processLoggingEventQueue, "userEventLoggingThread");
	if ( nullptr == userEventLoggingThread )
		return false;

	userEventGuiThread = evt_registerThread ( gui_processGuiEventQueue, "gui_processGuiEventQueue" );
	if ( nullptr == userEventGuiThread )
		return false;

	userEventGameThread = evt_registerThread(gam_processGameEventQueue, "userEventGameThread");
	if ( nullptr == userEventGameThread )
		return false;

	userEventServerThread = evt_registerThread(gam_processServerEventQueue, "userEventServerThread");
	if ( nullptr == userEventServerThread )
		return false;

	userEventClientThread = evt_registerThread(gam_processClientEventQueue, "userEventClientThread");
	if ( nullptr == userEventClientThread )
		return false;

    userEventTransferThread = evt_registerThread(gam_processTransferEventQueue, "userEventTransferThread");
    if ( nullptr == userEventTransferThread )
      return false;

	consoleMutex = SDL_CreateMutex ();
	if ( !consoleMutex )
	{
		printf ("Couldn't create mutex - consoleMutex");
		return false;
	}

	SDL_DetachThread (userEventConsoleThread);

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

	guiMutex = SDL_CreateMutex ();
	if ( !guiMutex )
	{
		printf ("Couldn't create mutex - guiMutex");
		return false;
	}

	gameMutex = SDL_CreateMutex ();
	if ( !gameMutex )
	{
		printf ("Couldn't create mutex - gameMutex");
		return false;
	}

	levelMutex = SDL_CreateMutex ();
	if ( !levelMutex )
	{
		printf ("Couldn't create mutex - levelMutex");
		return false;
	}

	textureSetMutex = SDL_CreateMutex ();
	if ( !textureSetMutex )
	{
		printf ("Couldn't create mutex - textureSetMutex");
		return false;
	}

	serverEventInMutex = SDL_CreateMutex ();
	if ( !serverEventInMutex )
	{
		printf ("Couldn't create mutex - serverEventInMutex");
		return false;
	}

	clientEventInMutex = SDL_CreateMutex ();
	if ( !clientEventInMutex )
	{
		printf ("Couldn't create mutex - clientEventInMutex");
		return false;
	}

	mainLoopMutex = SDL_CreateMutex ();
	if ( !mainLoopMutex )
	{
		printf ("Couldn't create mutex - mainLoopMutex\n");
		return false;
	}

    transferMutex = SDL_CreateMutex ();
    if ( !transferMutex )
    {
      printf ("Couldn't create mutex - transferMutex\n");
      return false;
    }

	SDL_DetachThread (userEventAudioThread);
	SDL_DetachThread (userEventLoggingThread);
	SDL_DetachThread (userEventGameThread);
	SDL_DetachThread (userEventServerThread);
	SDL_DetachThread (userEventTransferThread);

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
	SDL_DestroyMutex (levelMutex);
	SDL_DestroyMutex (textureSetMutex);
}

//-----------------------------------------------------------------------------------------------------
//
/// \param Pass in user event code, data1 and data2
/// \return None
//
// Send an event passing an index to the array entry holding the memory pointer to upload the texture
void evt_sendSDLEvent(int myEventCode, int myEventData1, int myEventData2)
//-----------------------------------------------------------------------------------------------------
{
	SDL_Event event;

	SDL_memset (&event, 0, sizeof (event)); /* or SDL_zero(event) */
	event.type = SDL_myEventType;
	event.user.code = myEventCode;
	event.user.data1 = reinterpret_cast<void *>(myEventData1);
	event.user.data2 = reinterpret_cast<void *>(myEventData2);
	SDL_PushEvent (&event);
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
        case USER_EVENT_TRANSFER:
          if ( SDL_LockMutex (transferMutex) == 0 )
            {
              transferEventQueue.push (eventData);
              SDL_UnlockMutex (transferMutex);
            }
            break;

		case MAIN_LOOP_EVENT:
			if ( SDL_LockMutex (mainLoopMutex) == 0 )
			{
				mainLoopEventQueue.push (eventData);
				SDL_UnlockMutex (mainLoopMutex);
			}
			break;

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

		case USER_EVENT_GUI:
			if ( SDL_LockMutex (guiMutex) == 0 )
			{
				guiEventQueue.push (eventData);
				SDL_UnlockMutex (guiMutex);
			}
			break;

		case USER_EVENT_GAME:
		case USER_EVENT_TEXTURE:
			if ( SDL_LockMutex (gameMutex) == 0 )
			{
				gameEventQueue.push (eventData);
				SDL_UnlockMutex (gameMutex);
			}
			break;

		case USER_EVENT_NETWORK_OUT:    // Send a packet OUT to clients
			if ( SDL_LockMutex (networkInMutex) == 0 )
			{
				networkOutQueue.push (eventData);
				SDL_UnlockMutex (networkInMutex);
			}
			break;

		case USER_EVENT_SERVER_EVENT:       // Take some action from a packet IN from the clients
			if ( SDL_LockMutex (serverEventInMutex) == 0 )
			{
				serverEventInQueue.push(eventData);
				SDL_UnlockMutex (serverEventInMutex);
			}
			break;

			/*
		case USER_EVENT_NETWORK_FROM_CLIENT:    // Send packet OUT to the server
			if ( SDL_LockMutex (networkInMutex) == 0 )
			{
				networkClientOutQueue.push (eventData);
				SDL_UnlockMutex (networkInMutex);
			}
			break;
*/
		case USER_EVENT_CLIENT_EVENT:     // Take some action from a packet IN from the server
			if ( SDL_LockMutex (clientEventInMutex) == 0 )
			{
				clientEventInQueue.push (eventData);
				SDL_UnlockMutex (clientEventInMutex);
			}
			break;

		default:
			break;
	}

	eventData.eventString = "";
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
//			con_print (CON_INFO, true, "Window %d shown", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_HIDDEN:
//			con_print (CON_INFO, true, "Window %d hidden", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_EXPOSED:
//			con_print (CON_INFO, true, "Window %d exposed", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_MOVED:
//			con_print (CON_INFO, true, "Window %d moved to %d,%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_RESIZED:
//			con_print (CON_INFO, true, "Window %d resized to %dx%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_SIZE_CHANGED:
//			con_print (CON_INFO, true, "Window %d size changed to %dx%d", event.window.windowID, event.window.data1, event.window.data2);
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
//			con_print (CON_INFO, true, "Window %d minimized", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
//			con_print (CON_INFO, true, "Window %d maximized", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_RESTORED:
//			con_print (CON_INFO, true, "Window %d restored", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_ENTER:
//			con_print (CON_INFO, true, "Mouse entered window %d", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_LEAVE:
//			con_print (CON_INFO, true, "Mouse left window %d", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
//			con_print (CON_INFO, true, "Window %d gained keyboard focus", event.window.windowID);
//			sys_changeMode(-1);
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
//			con_print (CON_INFO, true, "Window %d lost keyboard focus", event.window.windowID);
//			sys_changeMode (MODE_PAUSE);
			break;

		case SDL_WINDOWEVENT_CLOSE:
//			sys_changeMode (MODE_SHUTDOWN);
			break;

#if SDL_VERSION_ATLEAST (2, 0, 5)
		case SDL_WINDOWEVENT_TAKE_FOCUS:
//			con_print(CON_INFO, true, "Window %d is offered a focus", event.window.windowID);
			break;

		case SDL_WINDOWEVENT_HIT_TEST:
//			con_print(CON_INFO, true, "Window %d has a special hit test", event.window.windowID);
			break;
#endif
		default:
//			con_print (CON_INFO, true, "Window %d got unknown event %d", event.window.windowID, event.window.event);
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
//			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEBUTTONDOWN:
				io_handleMouseEvent ( event );
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
				switch (event.user.code)
				{
					case EVENT_TYPE_DO_TEXTURE_UPLOAD:
						io_uploadTextureIntoGL ((intptr_t)event.user.data1 );
						break;

					case EVENT_TYPE_DO_LEVEL_LOAD:
						lvl_loadLevel ((intptr_t) event.user.data1);
						break;

					default:
						break;
				}
			break;  // end of SDL_USEREVENT

			default:
				break;
		}
	}
}
