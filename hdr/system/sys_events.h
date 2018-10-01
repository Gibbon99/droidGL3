#pragma once

#include "hdr/system/sys_main.h"

using namespace glm;
using namespace std;

#define THREAD_DELAY_MS                 5       // Stop threads using all the CPU time

#define USER_EVENT_MOUSE                0x00
#define USER_EVENT_AUDIO                0x01
#define USER_EVENT_CONSOLE              0x02
#define USER_EVENT_MODE_PAUSE           0x03
#define USER_EVENT_MOUSE_BUTTON_DOWN    0x04
#define USER_EVENT_MOUSE_BUTTON_UP      0x05
#define USER_EVENT_MOUSE_WHEEL          0x06
#define USER_EVENT_MOUSE_MOTION         0x07
#define USER_EVENT_GAME                 0x08
#define USER_EVENT_GAME_TIMER           0x09
#define USER_EVENT_GAME_TIMER_ON        0x10
#define USER_EVENT_GAME_TIMER_OFF       0x11
#define USER_EVENT_GAME_TIMER_CONSOLE   0x12
#define USER_EVENT_LOGGING              0x13
#define USER_EVENT_LOGGING_ADD_LINE     0x14
#define USER_EVENT_LOGGING_START        0x15
#define USER_EVENT_LOGGING_STOP         0x16
#define USER_EVENT_TEXTURE              0x17
#define USER_EVENT_TEXTURE_LOAD         0x18
#define USER_EVENT_TEXTURE_UPLOAD       0x19
#define USER_EVENT_TEXTURE_ERROR        0x20
#define EVENT_TYPE_DO_TEXTURE_UPLOAD    0x21
#define USER_EVENT_TEXTURE_UPLOAD_DONE  0x22
#define USER_EVENT_GAME_LOAD_LEVEL      0x23
#define EVENT_TYPE_DO_LEVEL_LOAD        0x24
#define USER_EVENT_LEVEL_ERROR          0x25
#define USER_EVENT_LEVEL_EXTRAS         0x26
#define USER_EVENT_LEVEL_LOAD_DONE      0x27

typedef struct
{
	unsigned int            eventType;
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

extern SDL_mutex *consoleMutex;
extern SDL_mutex *audioMutex;
extern SDL_mutex *loggingMutex;
extern SDL_mutex *gameMutex;
extern SDL_mutex *levelMutex;
extern SDL_mutex *textureSetMutex;

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