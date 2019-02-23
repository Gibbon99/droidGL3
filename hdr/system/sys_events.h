#pragma once

#include "hdr/system/sys_main.h"
#include "hdr/network/net_server.h"
#include "hdr/network/net_common.h"

using namespace glm;
using namespace std;

#define THREAD_DELAY_MS                 5       // Stop threads using all the CPU time

enum myEvents
{
USER_EVENT_MOUSE = 0,
USER_EVENT_AUDIO,
USER_EVENT_CONSOLE,
USER_EVENT_MODE_PAUSE,
USER_EVENT_MOUSE_BUTTON_DOWN,
USER_EVENT_MOUSE_BUTTON_UP,
USER_EVENT_MOUSE_WHEEL,
USER_EVENT_MOUSE_MOTION,
USER_EVENT_GAME,
USER_EVENT_GAME_TIMER,
USER_EVENT_TIMER_ON,
USER_EVENT_TIMER_OFF,
USER_EVENT_GAME_TIMER_CONSOLE,
USER_EVENT_LOGGING,
USER_EVENT_LOGGING_ADD_LINE,
USER_EVENT_LOGGING_START,
USER_EVENT_LOGGING_STOP,
USER_EVENT_TEXTURE,
USER_EVENT_TEXTURE_LOAD,
USER_EVENT_TEXTURE_UPLOAD,
USER_EVENT_TEXTURE_ERROR,
EVENT_TYPE_DO_TEXTURE_UPLOAD,
USER_EVENT_TEXTURE_UPLOAD_DONE,
USER_EVENT_GAME_LOAD_LEVEL,
EVENT_TYPE_DO_LEVEL_LOAD,
USER_EVENT_LEVEL_ERROR,
USER_EVENT_LEVEL_EXTRAS,
USER_EVENT_LEVEL_LOAD_DONE,

USER_EVENT_KEY_EVENT,

USER_EVENT_NETWORK_OUT,
NETWORK_RECEIVE_DATA,
USER_EVENT_NETWORK_FROM_CLIENT,
NETWORK_SEND_DATA,

USER_EVENT_CLIENT_EVENT,
USER_EVENT_SERVER_EVENT,

USER_EVENT_TTF_ERROR,

MY_INPUT_ACTION_PRESS,
MY_INPUT_ACTION_RELEASE,

MY_INPUT_LEFT,
MY_INPUT_RIGHT,
MY_INPUT_UP,
MY_INPUT_DOWN,
MY_INPUT_ACTION,
MY_INPUT_ESCAPE,
MY_INPUT_CONSOLE,
MY_INPUT_FORWARD,
MY_INPUT_BACKWARD,
MY_INPUT_PAUSE,

LEVEL_LOAD_ERROR_NOT_FOUND,
LEVEL_LOAD_ERROR_FILESYSTEM,
LEVEL_LOAD_MEMORY_ERROR,
LEVEL_LOAD_MALLOC_ERROR,

SIDEVIEW_FILE_NOT_FOUND,
SIDEVIEW_MALLOC_ERROR,
SIDEVIEW_LOAD_MEMORY_ERROR,
SIDEVIEW_LOAD_ERROR_FILESYSTEM,

USER_EVENT_TTF_BAD_LOAD,
USER_EVENT_TTF_INIT_FAIL,

USER_EVENT_GUI,
USER_EVENT_GUI_MOUSE_TIMER,
USER_EVENT_GUI_ANIMATE_TIMER,

MAIN_LOOP_EVENT,
MAIN_LOOP_EVENT_ADD_BULLET,
MAIN_LOOP_EVENT_REMOVE_BULLET,
MAIN_LOOP_EVENT_REMOVE_DROID_PHYSICS,

USER_EVENT_TRANSFER,
TRANSFER_EVENT_INTRO_0,
TRANSFER_EVENT_INTRO_1,
TRANSFER_EVENT_INTRO_2
};

typedef struct
{
	unsigned int    eventType;
	int             eventAction;
	int             data1;
	int             data2;
	int             data3;
	vec2            vec2_1;
	vec2            vec2_2;
	string          eventString;
} _myEventData;

extern std::queue<_myEventData> consoleEventQueue;
extern std::queue<_myEventData> audioEventQueue;
extern std::queue<_myEventData> loggingEventQueue;
extern std::queue<_myEventData> gameEventQueue;
extern std::queue<_myEventData> guiEventQueue;
extern std::queue<_myEventData> mainLoopEventQueue;
extern std::queue<_myEventData> transferEventQueue;

extern std::queue<_myEventData> clientEventInQueue;
extern std::queue<_myEventData> serverEventInQueue;
extern std::queue<_myEventData> networkOutQueue;

extern SDL_mutex *consoleMutex;
extern SDL_mutex *audioMutex;
extern SDL_mutex *loggingMutex;
extern SDL_mutex *gameMutex;
extern SDL_mutex *guiMutex;
extern SDL_mutex *levelMutex;
extern SDL_mutex *textureSetMutex;
extern SDL_mutex *transferMutex;

extern SDL_mutex *networkInMutex;

extern SDL_mutex *serverEventInMutex;
extern SDL_mutex *clientEventInMutex;

extern SDL_mutex *mainLoopMutex;

extern bool runThreads;

// Main event routine - handle all the events coming in and
// farm them to the correct routine
//
// Called from the main thread
void evt_handleEvents ();

// Shutdown mutex
void evt_shutdownMutex ();

// Setup user event source and event data
bool evt_registerUserEventSetup ();

/// \param Pass in user event code, data1 and data2
/// \return None
void evt_sendSDLEvent ( int myEventCode, int myEventData1, int myEventData2 );

// Create a custom event to be sent
void evt_sendEvent ( uint type, int action, int data1, int data2, int data3, vec2 vec2_1, vec2 vec2_2, string textString );

// Change the state of the console timer cursor animation
void evt_cursorChangeState ( int newState );

// Keep a list of the timers and remove them at shutdown
SDL_TimerID
evt_registerTimer (Uint32 timerInterval, SDL_TimerCallback timerFunction, const std::string timerName, unsigned long dataIndex);

//
// Remove all the timers
void evt_removeAllTimers();

// Remove a single timer - pass in SDL_TimerID as index
void evt_removeTimer(SDL_TimerID whichTimerID);

// Add a timer with next mode to change too
SDL_TimerID evt_registerTimerAndEvent(Uint32 timerInterval, const _myEventData &nextEventData, const std::string &eventName);

// Remove a timer event and stop the SDL_Timer
void evt_removeTimerAndEvent(SDL_TimerID whichTimer);